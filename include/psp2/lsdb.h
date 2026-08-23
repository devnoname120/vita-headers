/**
 * \usergroup{SceLsdb}
 * \usage{psp2/lsdb.h,SceLsdb_stub}
 */

#ifndef _PSP2_LSDB_H_
#define _PSP2_LSDB_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>
#include <psp2common/kernel/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Errors returned by the FW 3.60 SceLsdb implementation.
 */
typedef enum SceLsdbErrorCode {
	SCE_LSDB_ERROR_INVALID_ARGUMENT                = 0x80101001,
	SCE_LSDB_ERROR_NO_MEMORY                       = 0x80101002,
	SCE_LSDB_ERROR_INVALID_HANDLE                  = 0x80101003,
	SCE_LSDB_ERROR_NOT_FOUND                       = 0x80101005, //!< No matching database row or no requested change.
	SCE_LSDB_ERROR_INVALID_NEW_EVENT_IDENTIFIER    = 0x80101008, //!< Neither titleId/itemId nor rowId identifies a NewEvent row.
	SCE_LSDB_ERROR_REQUIRED_APPINFO_FIELD_MISSING  = 0x80101009, //!< A required AppInfo field, such as TITLE_ID, is missing.
	SCE_LSDB_ERROR_DATABASE_LANGUAGE_MISMATCH      = 0x801010F0,
	SCE_LSDB_ERROR_DATABASE_CONFIGURATION_MISMATCH = 0x801010F1,
	SCE_LSDB_ERROR_DATABASE_SCHEMA_TOO_NEW         = 0x801010F2,
	SCE_LSDB_ERROR_DATABASE_VERSION_MISMATCH       = 0x801010F3,
	SCE_LSDB_ERROR_INVALID_DATABASE_VERSION        = 0x801010F4
} SceLsdbErrorCode;

/** Values accepted by ::sceLsdbOpen on FW 3.60. */
typedef enum SceLsdbOpenMode {
	SCE_LSDB_OPEN_MODE_NORMAL  = 59,  //!< Open and validate the existing AppDB.
	SCE_LSDB_OPEN_MODE_REBUILD = -59  //!< Remove, rebuild, and open the AppDB.
} SceLsdbOpenMode;

/**
 * String object used by SceLsdb.
 *
 * This is the PAF string ABI used on FW 3.60, not a plain NUL-terminated
 * string. A const input string is borrowed for the duration of the call; its
 * buffer is neither modified nor retained.
 *
 * A nonempty standalone string returned by SceLsdb owns a writable
 * `length + 1` byte PAF allocation. Release its \a data with ::sce_paf_free,
 * then clear the object. An empty returned string points to shared immutable
 * storage and must not be freed. Strings nested in a reference-counted object
 * are owned by that parent object and must not be freed separately.
 *
 * There is no spare capacity beyond the terminating NUL. The \a reserved word
 * is not a capacity or ownership flag.
 */
typedef struct SceLsdbString {
	char *data;          //!< NUL-terminated string data.
	SceSize length;      //!< String length, excluding the terminating NUL.
	SceUInt32 reserved;  //!< Not read or written on FW 3.60.
} SceLsdbString;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbString); // size is from FW 3.60

/**
 * UTF-16 string object used by localized SceLsdb records.
 *
 * It follows the same borrowed-input, parent-owned nested value, and
 * standalone output-buffer cleanup rules as ::SceLsdbString.
 */
typedef struct SceLsdbUtf16String {
	SceUInt16 *data;
	SceSize length;      //!< Number of UTF-16 code units, excluding the terminator.
	SceUInt32 reserved;  //!< Not read or written on FW 3.60.
} SceLsdbUtf16String;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbUtf16String); // size is from FW 3.60

/** App database wrapper used by the explicit-database entry points. */
typedef struct SceLsdbAppDatabase {
	void *handle;
} SceLsdbAppDatabase;
VITASDK_BUILD_ASSERT_EQ(0x4, SceLsdbAppDatabase); // size is from FW 3.60

/** Query context for the process-global AppDB connection. */
typedef struct SceLsdbQueryContext {
	void *handle;
} SceLsdbQueryContext;
VITASDK_BUILD_ASSERT_EQ(0x4, SceLsdbQueryContext); // size is from FW 3.60

/**
 * Reference-counted map of AppDB values for one title.
 *
 * The value getters return pointers owned by this object. Keep the object alive
 * until the returned value is no longer used.
 *
 * This type and every two-word `SceLsdb*Ref` type in this header use the FW
 * 3.60 PAF shared-reference ABI. An output object transfers one strong
 * reference to the caller. The object and its nested values remain mutable
 * while that reference is held; mutations are visible through every other
 * reference to the same object.
 *
 * A newly returned reference has a 16-byte PAF-allocated control block. It
 * stores the object pointer, strong-reference count, weak-reference count, and
 * deleter at byte offsets 0, 4, 8, and 12, initially using counts 1 and 1.
 * Copying a nonempty reference requires an atomic increment of the strong
 * count. Releasing it requires an atomic decrement; when that count reaches
 * zero, invoke the deleter on the object, decrement the weak count, and free
 * the control block with ::sce_paf_free when the weak count reaches zero. Then
 * clear both words in the holder. An empty reference has a NULL control block
 * and needs no release. Never free the object, its nested values, or the
 * control block directly. A raw structure assignment or `memcpy` does not
 * retain the object and will cause a double release or dangling reference.
 *
 * SceLsdb exports no generic C retain/release entry point for this or the other
 * reference types, so their callers must use a PAF-compatible
 * shared-reference wrapper.
 */
typedef struct SceLsdbAppInfo {
	void *data;
	void *control;
} SceLsdbAppInfo;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbAppInfo); // size is from FW 3.60

/** AppInfo operation reported to an ::SceLsdbAppInfoObserverCallback. */
typedef enum SceLsdbAppInfoEventType {
	SCE_LSDB_APP_INFO_EVENT_INSERT_APPLICATION = 0, //!< Follows ::sceLsdbInsertApplication.
	SCE_LSDB_APP_INFO_EVENT_DELETE_TITLE_DATA   = 1, //!< Follows ::sceLsdbDeleteTitleData.
	SCE_LSDB_APP_INFO_EVENT_UPDATE_APPLICATION = 2, //!< Follows ::sceLsdbUpdateApplication.
	SCE_LSDB_APP_INFO_EVENT_UPDATE_APP_INFO     = 3  //!< Follows AppInfo-only refresh or masked-update operations.
} SceLsdbAppInfoEventType;

/**
 * Process-local AppInfo observer callback.
 *
 * \a appInfo is borrowed and remains valid only for the duration of the
 * callback. The callback runs synchronously on the thread performing the
 * database operation while SceLsdb holds its observer-list lock. It must not
 * register or unregister observers or invoke another operation which dispatches
 * AppInfo observers. FW 3.60 SceShell does perform a read-only icon query from
 * this callback.
 */
typedef void (*SceLsdbAppInfoObserverCallback)(SceLsdbAppInfoEventType eventType, const SceLsdbAppInfo *appInfo, void *userData);

/**
 * Storage for one process-local AppInfo observer.
 *
 * Initialize both fields to zero before the first registration. SceLsdb keeps
 * the address of this object, so it must remain allocated at the same address
 * until ::sceLsdbUnregisterAppInfoObserver returns. It is not a
 * reference-counted object.
 */
typedef struct SceLsdbAppInfoObserver {
	SceLsdbAppInfoObserverCallback callback;
	void *userData;
} SceLsdbAppInfoObserver;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbAppInfoObserver); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbAppInfo objects.
 *
 * An output vector owns its backing allocation and every contained reference.
 * Release each element according to the shared-reference rules above, then
 * release \a data with ::sce_paf_free and clear the vector.
 */
typedef struct SceLsdbAppInfoArray {
	SceLsdbAppInfo *data;
	SceSize count;
	SceSize capacity;
} SceLsdbAppInfoArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbAppInfoArray); // size is from FW 3.60

/**
 * PAF vector of signed 32-bit values used by the AppDB page operations.
 * Release an output vector's \a data with ::sce_paf_free, then clear it.
 */
typedef struct SceLsdbInt32Array {
	SceInt32 *data;
	SceSize count;
	SceSize capacity;
} SceLsdbInt32Array;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbInt32Array); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbString objects.
 *
 * For an output vector, release every nonempty standalone string buffer, then
 * release \a data with ::sce_paf_free and clear the vector.
 */
typedef struct SceLsdbStringArray {
	SceLsdbString *data;
	SceSize count;
	SceSize capacity;
} SceLsdbStringArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbStringArray); // size is from FW 3.60

/** One row of tbl_appinfo_page. */
typedef struct SceLsdbPageInfo {
	SceLsdbString themeFile;
	SceUInt32 backgroundColor;
	SceUInt32 textureWidth;
	SceUInt32 textureHeight;
	SceUInt32 imageWidth;
	SceUInt32 imageHeight;
	SceUInt32 reserved;
} SceLsdbPageInfo;
VITASDK_BUILD_ASSERT_EQ(0x24, SceLsdbPageInfo); // size is from FW 3.60

/**
 * Reference-counted AppDB page-information object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbPageInfoRef {
	SceLsdbPageInfo *object;
	void *control;
} SceLsdbPageInfoRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbPageInfoRef); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbPageInfoRef objects. An output vector owns its
 * backing allocation and every contained strong reference.
 */
typedef struct SceLsdbPageInfoRefArray {
	SceLsdbPageInfoRef *data;
	SceSize count;
	SceSize capacity;
} SceLsdbPageInfoRefArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbPageInfoRefArray); // size is from FW 3.60

/** Information associated with an icon on a special negative page. */
typedef struct SceLsdbSpecialPageInfo {
	SceLsdbStringArray titleIds;
	SceUInt32 reserved;          //!< Its purpose is unknown.
	SceSize processedRowCount;
	SceInt32 pageNo;             //!< Special page number from -20000000 through -10000001.
} SceLsdbSpecialPageInfo;
VITASDK_BUILD_ASSERT_EQ(0x18, SceLsdbSpecialPageInfo); // size is from FW 3.60

/**
 * Reference-counted ::SceLsdbSpecialPageInfo object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbSpecialPageInfoRef {
	SceLsdbSpecialPageInfo *object;
	void *control;
} SceLsdbSpecialPageInfoRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbSpecialPageInfoRef); // size is from FW 3.60

/** Known values of ::SceLsdbIconInfo::type on FW 3.60. */
typedef enum SceLsdbIconType {
	SCE_LSDB_ICON_TYPE_FOLDER = 5, //!< Folder bubble/layout category.
	SCE_LSDB_ICON_TYPE_POWER  = 6  //!< Built-in power bubble/layout category.
} SceLsdbIconType;

/** Known values of ::SceLsdbIconInfo::icon0Type on FW 3.60. */
typedef enum SceLsdbIcon0Type {
	SCE_LSDB_ICON0_TYPE_FOLDER = 7, //!< Folder icon0 source classification.
	SCE_LSDB_ICON0_TYPE_POWER  = 8  //!< Built-in power icon source classification.
} SceLsdbIcon0Type;

/**
 * One row of tbl_appinfo_icon.
 *
 * \a type selects the icon's database/layout category. \a icon0Type is a
 * separate metadata classification for the icon0 source; it does not select
 * path storage versus BLOB storage or describe the image encoding. FW 3.60
 * also uses values below the known folder and power values, but their
 * individual meanings are unknown.
 *
 * The table also contains five reserved columns which are not represented as
 * five direct structure members. `reserved01` is the stable numeric icon ID
 * used by ::sceLsdbGetIconId and also links icons to special negative pages.
 * `reserved02` is an alternate icon path which can override \a iconPath when a
 * query requests it. No FW 3.60 reader or writer for `reserved03` or
 * `reserved04` was found. `reserved05` stores a serialized, rendered PAF icon
 * cache and is returned by ::sceLsdbGetIconReservedBlob.
 *
 * The source icon remains a path in \a iconPath, or in `reserved02` when an
 * alternate path is selected. The `reserved05` blob is generated after the
 * source image is decoded and rendered; it is not a copy of `icon0.png`. The
 * path and rendered-cache BLOB can coexist in the same row.
 *
 * \a status is a separately persisted and updateable integer. No FW 3.60
 * consumer traced during this analysis interprets its value, and database
 * recovery does not copy it; its purpose and valid values are unknown.
 */
typedef struct SceLsdbIconInfo {
	SceInt64 rowId; //!< SQLite row ID; this is distinct from `reserved01`.
	SceInt32 position; //!< Position within the containing page.
	SceLsdbString title; //!< Localized bubble title stored in the icon table.
	SceLsdbString iconPath; //!< Source-image path.
	SceInt32 type; //!< One of the known ::SceLsdbIconType values, or another FW-defined value.
	SceLsdbString titleId;
	SceInt32 icon0Type; //!< One of the known ::SceLsdbIcon0Type values, or another FW-defined value.
	SceLsdbString command;
	SceInt32 parentalLockLevel;
	SceInt32 status; //!< Persisted icon status; its purpose and valid values are unknown.
	SceLsdbSpecialPageInfoRef specialPage; //!< Derived special-page relationship, when present.
	SceUInt32 reserved; //!< Its purpose is unknown.
} SceLsdbIconInfo;
VITASDK_BUILD_ASSERT_EQ(0x58, SceLsdbIconInfo); // size is from FW 3.60

/**
 * Reference-counted AppDB icon-information object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbIconInfoRef {
	SceLsdbIconInfo *object;
	void *control;
} SceLsdbIconInfoRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbIconInfoRef); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbIconInfoRef objects. An output vector owns its
 * backing allocation and every contained strong reference.
 */
typedef struct SceLsdbIconInfoRefArray {
	SceLsdbIconInfoRef *data;
	SceSize count;
	SceSize capacity;
} SceLsdbIconInfoRefArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbIconInfoRefArray); // size is from FW 3.60

/** Field-selection bits accepted by ::sceLsdbUpdateIconInfo. */
typedef enum SceLsdbIconInfoUpdateFlag {
	SCE_LSDB_ICON_INFO_UPDATE_TITLE               = 0x00000001, //!< Update ::SceLsdbIconInfo::title.
	SCE_LSDB_ICON_INFO_UPDATE_ICON_PATH           = 0x00000002, //!< Update ::SceLsdbIconInfo::iconPath.
	SCE_LSDB_ICON_INFO_UPDATE_ICON0_TYPE          = 0x00000004, //!< Update ::SceLsdbIconInfo::icon0Type.
	SCE_LSDB_ICON_INFO_UPDATE_COMMAND             = 0x00000008, //!< Update ::SceLsdbIconInfo::command.
	SCE_LSDB_ICON_INFO_UPDATE_PARENTAL_LOCK_LEVEL = 0x00000010, //!< Update ::SceLsdbIconInfo::parentalLockLevel.
	SCE_LSDB_ICON_INFO_UPDATE_STATUS              = 0x00000020, //!< Update ::SceLsdbIconInfo::status.
	SCE_LSDB_ICON_INFO_UPDATE_RENDERED_ICON_BLOB  = 0x40000000, //!< Update `reserved05` from the supplied byte buffer.
	SCE_LSDB_ICON_INFO_UPDATE_ALTERNATE_ICON_PATH = 0x80000000 //!< Update `reserved02` from ::SceLsdbIconInfo::iconPath.
} SceLsdbIconInfoUpdateFlag;

typedef enum SceLsdbDbBlobCallbackPhase {
	SCE_LSDB_DB_BLOB_CALLBACK_PHASE_ACQUIRE = 0,
	SCE_LSDB_DB_BLOB_CALLBACK_PHASE_RELEASE = 1
} SceLsdbDbBlobCallbackPhase;

/**
 * Callback used by a database blob to acquire and release its database handle.
 *
 * \a phase is one of ::SceLsdbDbBlobCallbackPhase. On FW 3.60, \a mode is 1
 * for a read acquisition and 0 for a write
 * acquisition. During release it is 1 after a read or a failed write and 0
 * after a successful write.
 */
typedef int (*SceLsdbDbBlobCallback)(SceInt32 phase, SceInt32 mode, void *userData, void **databaseHandle);

/** Database location and lifetime callback held by a ::SceLsdbDbBlob. */
typedef struct SceLsdbDbBlobConfig {
	SceUInt32 typeTag; //!< Set to 0x102 on FW 3.60.
	SceLsdbString databaseName;
	SceLsdbString tableName;
	SceLsdbString columnName;
	SceInt64 rowId;
	SceBool writable;
	SceLsdbDbBlobCallback callback;
	void *userData;
} __attribute__((packed, aligned(4))) SceLsdbDbBlobConfig;
VITASDK_BUILD_ASSERT_EQ(0x3C, SceLsdbDbBlobConfig); // size is from FW 3.60

/**
 * Database-backed PAF stream returned by the blob accessors.
 *
 * The database and blob handles are acquired lazily. The stream's current
 * byte position is advanced by reads and writes and changed by seeks.
 */
typedef struct SceLsdbDbBlob {
	void *vtable; //!< Internal FW 3.60 PAF stream virtual-function table.
	SceUInt32 reserved; //!< Not initialized on FW 3.60.
	SceUInt8 mutex[0x20]; //!< Internal PAF mutex.
	SceLsdbDbBlobConfig *config;
	void *databaseHandle;
	void *blobHandle;
	SceSize position;
} SceLsdbDbBlob;
VITASDK_BUILD_ASSERT_EQ(0x38, SceLsdbDbBlob); // size is from FW 3.60

/**
 * Reference-counted database blob returned by SceLsdb.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbDbBlobRef {
	SceLsdbDbBlob *object;
	void *control;
} SceLsdbDbBlobRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbDbBlobRef); // size is from FW 3.60

#define SCE_LSDB_THEME_PAGE_LAYOUT_COUNT        10
#define SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_COUNT 20
#define SCE_LSDB_THEME_LOCALIZED_STRING_COUNT   20

/** Fields from one Shell theme page consumed by ::sceLsdbUpdateThemeLayout. */
typedef struct SceLsdbThemePageLayout {
	SceLsdbString themeFile; //!< Path relative to ::SceLsdbThemeLayout::themeRootPath.
	SceLsdbString shellData; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceUInt32 backgroundColor;
	SceUInt32 reserved01Color; //!< Low 24 bits written to ::SceLsdbPageInfo::reserved.
	SceUInt8 reserved01Flag; //!< Nonzero to also set bit 24 of ::SceLsdbPageInfo::reserved.
	SceUInt8 padding[3];
} SceLsdbThemePageLayout;
VITASDK_BUILD_ASSERT_EQ(0x24, SceLsdbThemePageLayout); // size is from FW 3.60

/** One system-icon path consumed by ::sceLsdbUpdateThemeLayout. */
typedef struct SceLsdbThemeSystemIconLayout {
	SceLsdbString iconPath; //!< Path relative to ::SceLsdbThemeLayout::themeRootPath.
	SceInt32 shellData[2]; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
} SceLsdbThemeSystemIconLayout;
VITASDK_BUILD_ASSERT_EQ(0x14, SceLsdbThemeSystemIconLayout); // size is from FW 3.60

/** Indices used by ::SceLsdbThemeLayout::systemIconLayouts on FW 3.60. */
typedef enum SceLsdbThemeSystemIconLayoutIndex {
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10003             = 0,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10010             = 1,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10009             = 2,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10012             = 3,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10001             = 4,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10008             = 5,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10005             = 6,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10000             = 7,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10026             = 8,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10007             = 9,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10013_OR_NPXS10098 = 10,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10006             = 11,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10072             = 12,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10091             = 13,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10002_OR_NPXS10034 = 14, //!< SceLsdb maps NPXS10002; Shell maps NPXS10034.
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10014             = 15,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10094             = 16,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10004             = 17,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_NPXS10015             = 18,
	SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_ICON_TYPE_6           = 19
} SceLsdbThemeSystemIconLayoutIndex;

/** Fields selected by ::sceLsdbUpdateThemeLayout. */
typedef enum SceLsdbThemeLayoutUpdateFlag {
	SCE_LSDB_THEME_LAYOUT_UPDATE_SYSTEM_ICONS = 0x1,
	SCE_LSDB_THEME_LAYOUT_UPDATE_PAGES        = 0x2
} SceLsdbThemeLayoutUpdateFlag;

/** Selected and per-language strings parsed from a Shell theme. */
typedef struct SceLsdbLocalizedThemeStrings {
	SceLsdbUtf16String selected; //!< String selected by Shell for the current language.
	SceLsdbUtf16String localized[SCE_LSDB_THEME_LOCALIZED_STRING_COUNT]; //!< Values stored for language indices 0 through 19.
} SceLsdbLocalizedThemeStrings;
VITASDK_BUILD_ASSERT_EQ(0xFC, SceLsdbLocalizedThemeStrings); // size is from FW 3.60

/**
 * Shell theme-layout object consumed by ::sceLsdbUpdateThemeLayout.
 *
 * ::sceLsdbUpdateThemeLayout reads only the root path, page layouts, and
 * system-icon layouts. Shell uses the later fields to construct the arguments
 * passed to ::sceLsdbReplaceTheme.
 */
typedef struct SceLsdbThemeLayout {
	SceLsdbString themeRootPath;
	SceInt32 shellData0; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceLsdbThemePageLayout pageLayouts[SCE_LSDB_THEME_PAGE_LAYOUT_COUNT];
	SceLsdbString shellStrings[3]; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceLsdbThemeSystemIconLayout systemIconLayouts[SCE_LSDB_THEME_SYSTEM_ICON_LAYOUT_COUNT];
	SceUInt8 shellMetadata[0x48]; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceUInt32 contentVersion;
	SceLsdbLocalizedThemeStrings localizedTitles;
	SceLsdbLocalizedThemeStrings localizedProviders;
	SceLsdbString packageImageFilePath;
	SceLsdbString startPreviewFilePath;
	SceLsdbString homePreviewFilePath;
	SceLsdbString shellString; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceUInt32 shellData1[2]; //!< Used by Shell but ignored by SceLsdb on FW 3.60.
	SceRtcTick lastModifiedTime;
} SceLsdbThemeLayout;
VITASDK_BUILD_ASSERT_EQ(0x5B0, SceLsdbThemeLayout); // size is from FW 3.60

/**
 * Reference-counted shell theme-layout object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbThemeLayoutRef {
	SceLsdbThemeLayout *object;
	void *control;
} SceLsdbThemeLayoutRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbThemeLayoutRef); // size is from FW 3.60

/** One localized row of tbl_theme. */
typedef struct SceLsdbThemeInfo {
	SceUInt32 reserved0; //!< Its purpose is unknown.
	SceLsdbString themeId;
	SceInt32 type; //!< Must be 100 when passed to ::sceLsdbReplaceTheme on FW 3.60.
	SceLsdbString packageImageFilePath;
	SceLsdbString homePreviewFilePath;
	SceLsdbString startPreviewFilePath;
	SceLsdbUtf16String title;
	SceLsdbUtf16String provider;
	SceUInt32 contentVersion;
	SceUInt32 reserved1; //!< Its purpose is unknown.
	SceRtcTick lastModifiedTime;
	SceSize size;
} __attribute__((packed, aligned(4))) SceLsdbThemeInfo;
VITASDK_BUILD_ASSERT_EQ(0x64, SceLsdbThemeInfo); // size is from FW 3.60

typedef enum SceLsdbValueType {
	SCE_LSDB_VALUE_TYPE_INT64  = 0,
	SCE_LSDB_VALUE_TYPE_STRING = 1
} SceLsdbValueType;

/** Integer-or-string value stored in tbl_appinfo or tbl_config. */
typedef struct SceLsdbValue {
	SceInt32 type; //!< One of ::SceLsdbValueType.
	SceUInt32 reserved;
	union {
		SceInt64 int64;
		struct {
			SceLsdbString *value;
			void *reserved;
		} string;
	} data;
} SceLsdbValue;
VITASDK_BUILD_ASSERT_EQ(0x10, SceLsdbValue); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbValue objects. An output vector owns its backing
 * allocation and the storage owned by its elements.
 */
typedef struct SceLsdbValueArray {
	SceLsdbValue *data;
	SceSize count;
	SceSize capacity;
} SceLsdbValueArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbValueArray); // size is from FW 3.60

/** Comparison performed by an ::SceLsdbAppInfoFilter node. */
typedef enum SceLsdbFilterOperator {
	SCE_LSDB_FILTER_OPERATOR_EQUAL         = 0, //!< Compare for equality.
	SCE_LSDB_FILTER_OPERATOR_PREFIX        = 1, //!< Match a string prefix using SQL LIKE with escaping.
	SCE_LSDB_FILTER_OPERATOR_NOT_EQUAL     = 2, //!< Compare for inequality.
	SCE_LSDB_FILTER_OPERATOR_BITMASK_EQUAL = 3  //!< Test `(storedValue & mask) == value` for integer values.
} SceLsdbFilterOperator;

/**
 * One condition used to filter AppInfo records.
 *
 * Values within one node are joined with OR. Nodes linked through \a next are
 * joined with AND. An empty \a values array tests only whether \a key exists.
 * The prefix operator accepts string values, while the bit-mask operator
 * accepts integer values and uses \a mask.
 */
typedef struct SceLsdbAppInfoFilter {
	SceUInt32 key;                         //!< FNV-1a hash of the AppInfo key.
	SceLsdbValueArray values;              //!< Values to compare against.
	SceUInt32 reserved;                    //!< Ignored on FW 3.60.
	SceInt32 operation;                   //!< One of ::SceLsdbFilterOperator.
	SceUInt64 mask;                        //!< Mask used by ::SCE_LSDB_FILTER_OPERATOR_BITMASK_EQUAL.
	const struct SceLsdbAppInfoFilter *next;
} __attribute__((packed, aligned(4))) SceLsdbAppInfoFilter;
VITASDK_BUILD_ASSERT_EQ(0x24, SceLsdbAppInfoFilter); // size is from FW 3.60

/** Flags accepted by the filtered-title queries. */
typedef enum SceLsdbFilterFlags {
	SCE_LSDB_FILTER_FLAG_APPINFO_ONLY                   = 0x1, //!< Query tbl_appinfo without requiring an icon record.
	SCE_LSDB_FILTER_FLAG_INCLUDE_PAGE_MINUS_100000000   = 0x2, //!< Do not exclude icon records on page -100000000.
	SCE_LSDB_FILTER_FLAG_OMIT_ICON_TITLE_SORT           = 0x4  //!< Omit the icon-title ORDER BY clause.
} SceLsdbFilterFlags;

/** New-event database wrapper. */
typedef struct SceLsdbNewEventDatabase {
	void *handle;
} SceLsdbNewEventDatabase;
VITASDK_BUILD_ASSERT_EQ(0x4, SceLsdbNewEventDatabase); // size is from FW 3.60

/**
 * Selector accepted by the NewEvent update and deletion operations.
 *
 * A selector uses either both \a titleId and \a itemId, or a nonzero \a rowId.
 */
typedef struct SceLsdbNewEventSelector {
	SceLsdbString titleId;
	SceLsdbString itemId;
	SceInt64 rowId;
} SceLsdbNewEventSelector;
VITASDK_BUILD_ASSERT_EQ(0x20, SceLsdbNewEventSelector); // size is from FW 3.60

/**
 * One row of the FW 3.60 tbl_newEvent database.
 *
 * The action type is stored as two 32-bit words in the ABI. It must not be
 * treated as a contiguous SceUInt64 field.
 */
typedef struct SceLsdbNewEvent {
	SceLsdbString titleId;
	SceLsdbString itemId;
	SceInt64 rowId;
	SceInt64 messageType;
	SceUInt32 actionTypeLow;
	SceUInt8 newFlag;
	SceUInt8 popupFlag;
	SceUInt8 padding[2];
	SceLsdbString iconPath;
	void *iconData;
	SceSize iconDataSize;
	SceLsdbString messageArgs[9];
	SceLsdbString title;
	SceLsdbString description;
	SceUInt32 execMode;
	SceLsdbString execTitleId;
	SceLsdbString execArgument;
	SceLsdbString updateTime;
	SceInt32 soundId;
	float soundVolume;
	SceUInt32 hash;
	SceUInt32 actionTypeHigh;
} SceLsdbNewEvent;
VITASDK_BUILD_ASSERT_EQ(0x100, SceLsdbNewEvent); // size is from FW 3.60

/**
 * Reference-counted ::SceLsdbNewEvent object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbNewEventRef {
	SceLsdbNewEvent *object;
	void *control;
} SceLsdbNewEventRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbNewEventRef); // size is from FW 3.60

/**
 * PAF vector of ::SceLsdbNewEventRef objects. An output vector owns its
 * backing allocation and every contained strong reference.
 */
typedef struct SceLsdbNewEventRefArray {
	SceLsdbNewEventRef *data;
	SceSize count;
	SceSize capacity;
} SceLsdbNewEventRefArray;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbNewEventRefArray); // size is from FW 3.60

/** Field-selection bits accepted by ::sceLsdbUpdateNewEvents. */
typedef enum SceLsdbNewEventUpdateFlag {
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_ICON_PATH   = 0x00000001, //!< Compare and update iconPath.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_ICON_DATA   = 0x00000002, //!< Compare and update iconData.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_MESSAGE     = 0x00000004, //!< Compare and update messageType and messageArgs.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_ACTION_TYPE = 0x00000008, //!< Compare and update actionTypeLow.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_TITLE       = 0x00000010, //!< Compare and update title.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_DESCRIPTION = 0x00000020, //!< Compare and update description.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_EXECUTION   = 0x00000040, //!< Compare and update execMode, execTitleId, and execArgument.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_NEW_FLAG    = 0x00000080, //!< Compare and update newFlag.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_TIME        = 0x00000100, //!< Compare and update updateTime.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_SOUND       = 0x00000200, //!< Compare and update soundId and soundVolume.
	SCE_LSDB_NEW_EVENT_UPDATE_FLAG_POPUP       = 0x00000400  //!< Compare popupFlag and assign popup_no.
} SceLsdbNewEventUpdateFlag;

/** Inclusive start and end times used by ::sceLsdbIsCurrentTimeInRange. */
typedef struct SceLsdbTimeRange {
	SceRtcTick start;
	SceRtcTick end;
} SceLsdbTimeRange;
VITASDK_BUILD_ASSERT_EQ(0x10, SceLsdbTimeRange); // size is from FW 3.60

/** PCC state stored in tbl_livearea for one title. */
typedef struct SceLsdbLiveAreaPccInfo {
	SceRtcTick updateDate;
	SceInt32 resourceStatus; //!< Stored pccResStatus plus one.
	SceInt32 signOff;
} SceLsdbLiveAreaPccInfo;
VITASDK_BUILD_ASSERT_EQ(0x10, SceLsdbLiveAreaPccInfo); // size is from FW 3.60

/** Fields selected by ::sceLsdbUpdateLiveAreaPccInfo. */
typedef enum SceLsdbLiveAreaPccUpdateFlag {
	SCE_LSDB_LIVEAREA_PCC_UPDATE_FLAG_DATE            = 0x00000001,
	SCE_LSDB_LIVEAREA_PCC_UPDATE_FLAG_RESOURCE_STATUS = 0x00000002,
	SCE_LSDB_LIVEAREA_PCC_UPDATE_FLAG_SIGN_OFF        = 0x00000004
} SceLsdbLiveAreaPccUpdateFlag;

/** One sfType/sfTarg pair stored in tbl_livearea. */
typedef struct SceLsdbLiveAreaSfEntry {
	SceInt32 type;
	SceLsdbString target;
} SceLsdbLiveAreaSfEntry;
VITASDK_BUILD_ASSERT_EQ(0x10, SceLsdbLiveAreaSfEntry); // size is from FW 3.60

/** Object types stored in a parsed LiveArea frame list. */
typedef enum SceLsdbLiveAreaObjectType {
	SCE_LSDB_LIVEAREA_OBJECT_TYPE_BACKGROUND_IMAGE     = 1,
	SCE_LSDB_LIVEAREA_OBJECT_TYPE_SYSTEM_FUNCTION_ZONE = 2,
	SCE_LSDB_LIVEAREA_OBJECT_TYPE_STARTUP_IMAGE        = 3,
	SCE_LSDB_LIVEAREA_OBJECT_TYPE_FRAME                = 4,
	SCE_LSDB_LIVEAREA_OBJECT_TYPE_TITLE_COLOR          = 5
} SceLsdbLiveAreaObjectType;

/** Common prefix of every object in a parsed LiveArea object list. */
typedef struct SceLsdbLiveAreaObject {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< One of ::SceLsdbLiveAreaObjectType.
} SceLsdbLiveAreaObject;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbLiveAreaObject); // size is from FW 3.60

/** Parsed LiveArea background image. */
typedef struct SceLsdbLiveAreaBackgroundImage {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< Always ::SCE_LSDB_LIVEAREA_OBJECT_TYPE_BACKGROUND_IMAGE.
	SceLsdbString sourcePath;
} SceLsdbLiveAreaBackgroundImage;
VITASDK_BUILD_ASSERT_EQ(0x14, SceLsdbLiveAreaBackgroundImage); // size is from FW 3.60

/** Parsed system-function-zone entries. */
typedef struct SceLsdbLiveAreaSystemFunctionZone {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< Always ::SCE_LSDB_LIVEAREA_OBJECT_TYPE_SYSTEM_FUNCTION_ZONE.
	SceLsdbLiveAreaSfEntry *entries;
	SceSize entryCount;
	SceSize entryCapacity;
	SceUInt32 reserved; //!< Not initialized or interpreted on FW 3.60.
} SceLsdbLiveAreaSystemFunctionZone;
VITASDK_BUILD_ASSERT_EQ(0x18, SceLsdbLiveAreaSystemFunctionZone); // size is from FW 3.60

/** Parsed gate startup image. */
typedef struct SceLsdbLiveAreaStartupImage {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< Always ::SCE_LSDB_LIVEAREA_OBJECT_TYPE_STARTUP_IMAGE.
	SceLsdbString sourcePath;
} SceLsdbLiveAreaStartupImage;
VITASDK_BUILD_ASSERT_EQ(0x14, SceLsdbLiveAreaStartupImage); // size is from FW 3.60

/** Parsed LiveArea title color, normalized to the range 0.0 through 1.0. */
typedef struct SceLsdbLiveAreaTitleColor {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< Always ::SCE_LSDB_LIVEAREA_OBJECT_TYPE_TITLE_COLOR.
	float red;
	float green;
	float blue;
	float alpha;
} SceLsdbLiveAreaTitleColor;
VITASDK_BUILD_ASSERT_EQ(0x18, SceLsdbLiveAreaTitleColor); // size is from FW 3.60

/** Node in the parsed LiveArea object list. The sentinel has no object. */
typedef struct SceLsdbLiveAreaObjectListNode {
	struct SceLsdbLiveAreaObjectListNode *previous;
	struct SceLsdbLiveAreaObjectListNode *next;
	SceLsdbLiveAreaObject *object;
} SceLsdbLiveAreaObjectListNode;
VITASDK_BUILD_ASSERT_EQ(0xC, SceLsdbLiveAreaObjectListNode); // size is from FW 3.60

/** Element types stored in a parsed LiveArea frame item. */
typedef enum SceLsdbLiveAreaFrameElementType {
	SCE_LSDB_LIVEAREA_FRAME_ELEMENT_TYPE_TARGET     = 3,
	SCE_LSDB_LIVEAREA_FRAME_ELEMENT_TYPE_BACKGROUND = 4,
	SCE_LSDB_LIVEAREA_FRAME_ELEMENT_TYPE_IMAGE      = 5,
	SCE_LSDB_LIVEAREA_FRAME_ELEMENT_TYPE_TEXT       = 6
} SceLsdbLiveAreaFrameElementType;

/** Horizontal alignment values used by parsed LiveArea visual elements. */
typedef enum SceLsdbLiveAreaHorizontalAlignment {
	SCE_LSDB_LIVEAREA_HORIZONTAL_ALIGNMENT_CENTER        = 0,
	SCE_LSDB_LIVEAREA_HORIZONTAL_ALIGNMENT_LEFT          = 1,
	SCE_LSDB_LIVEAREA_HORIZONTAL_ALIGNMENT_RIGHT         = 2,
	SCE_LSDB_LIVEAREA_HORIZONTAL_ALIGNMENT_OUTSIDE_LEFT  = 3,
	SCE_LSDB_LIVEAREA_HORIZONTAL_ALIGNMENT_OUTSIDE_RIGHT = 4
} SceLsdbLiveAreaHorizontalAlignment;

/** Vertical alignment values used by parsed LiveArea visual elements. */
typedef enum SceLsdbLiveAreaVerticalAlignment {
	SCE_LSDB_LIVEAREA_VERTICAL_ALIGNMENT_CENTER         = 0,
	SCE_LSDB_LIVEAREA_VERTICAL_ALIGNMENT_TOP            = 1,
	SCE_LSDB_LIVEAREA_VERTICAL_ALIGNMENT_BOTTOM         = 2,
	SCE_LSDB_LIVEAREA_VERTICAL_ALIGNMENT_OUTSIDE_TOP    = 3,
	SCE_LSDB_LIVEAREA_VERTICAL_ALIGNMENT_OUTSIDE_BOTTOM = 4
} SceLsdbLiveAreaVerticalAlignment;

/** Coordinate origin accepted by parsed LiveArea image and text elements. */
typedef enum SceLsdbLiveAreaElementOrigin {
	SCE_LSDB_LIVEAREA_ELEMENT_ORIGIN_FRAME      = 2,
	SCE_LSDB_LIVEAREA_ELEMENT_ORIGIN_BACKGROUND = 4,
	SCE_LSDB_LIVEAREA_ELEMENT_ORIGIN_IMAGE      = 5
} SceLsdbLiveAreaElementOrigin;

/** Selection mode derived from a LiveArea frame's `multi` attribute. */
typedef enum SceLsdbLiveAreaFrameMultiMode {
	SCE_LSDB_LIVEAREA_FRAME_MULTI_MODE_ORDERED = 0, //!< `multi="o"`; select items cyclically in document order.
	SCE_LSDB_LIVEAREA_FRAME_MULTI_MODE_RANDOM  = 1  //!< `multi="r"`; select an item randomly, avoiding the current item when possible.
} SceLsdbLiveAreaFrameMultiMode;

/** Compatibility selector used for a LiveArea liveitem's `pokesute` attribute. */
typedef enum SceLsdbLiveAreaCompatibilityMode {
	SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_ANY         = 0, //!< Disable `pokesute` filtering.
	SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_UNSPECIFIED = 1, //!< No recognized `pokesute` attribute.
	SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_ON          = 2, //!< `pokesute="on"`.
	SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_OFF         = 3  //!< `pokesute="off"`.
} SceLsdbLiveAreaCompatibilityMode;

/** Common prefix of a parsed target, background, image, or text element. */
typedef struct SceLsdbLiveAreaFrameElement {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< One of ::SceLsdbLiveAreaFrameElementType.
} SceLsdbLiveAreaFrameElement;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbLiveAreaFrameElement); // size is from FW 3.60

/** Common prefix of parsed background, image, and text elements. */
typedef struct SceLsdbLiveAreaVisualElement {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 type; //!< One of ::SceLsdbLiveAreaFrameElementType.
	SceInt32 horizontalAlignment; //!< One of ::SceLsdbLiveAreaHorizontalAlignment.
	SceInt32 verticalAlignment; //!< One of ::SceLsdbLiveAreaVerticalAlignment.
	SceInt32 width;
	SceInt32 height;
	SceInt32 x;
	SceInt32 y;
	SceUInt8 hasX; //!< Nonzero when the XML element specifies `x`.
	SceUInt8 hasY; //!< Nonzero when the XML element specifies `y`.
	SceUInt8 padding[2];
} SceLsdbLiveAreaVisualElement;
VITASDK_BUILD_ASSERT_EQ(0x24, SceLsdbLiveAreaVisualElement); // size is from FW 3.60

/** Parsed LiveArea target element. */
typedef struct SceLsdbLiveAreaTargetElement {
	SceLsdbLiveAreaFrameElement element;
	SceLsdbString target;
	SceLsdbString requiredAppVersion; //!< Value of `req-appver` for template formats newer than 1.00.
} SceLsdbLiveAreaTargetElement;
VITASDK_BUILD_ASSERT_EQ(0x20, SceLsdbLiveAreaTargetElement); // size is from FW 3.60

/** Parsed LiveArea background element. */
typedef struct SceLsdbLiveAreaBackgroundElement {
	SceLsdbLiveAreaVisualElement visual;
	SceLsdbString sourcePath;
} SceLsdbLiveAreaBackgroundElement;
VITASDK_BUILD_ASSERT_EQ(0x30, SceLsdbLiveAreaBackgroundElement); // size is from FW 3.60

/** Parsed LiveArea image element. */
typedef struct SceLsdbLiveAreaImageElement {
	SceLsdbLiveAreaVisualElement visual;
	SceLsdbString sourcePath;
	SceInt32 origin; //!< ::SCE_LSDB_LIVEAREA_ELEMENT_ORIGIN_FRAME or ::SCE_LSDB_LIVEAREA_ELEMENT_ORIGIN_BACKGROUND.
} SceLsdbLiveAreaImageElement;
VITASDK_BUILD_ASSERT_EQ(0x34, SceLsdbLiveAreaImageElement); // size is from FW 3.60

/** Parsed LiveArea text element. */
typedef struct SceLsdbLiveAreaTextElement {
	SceLsdbLiveAreaVisualElement visual;
	SceInt32 textAlignment; //!< One of ::SceLsdbLiveAreaHorizontalAlignment.
	SceInt32 verticalTextAlignment; //!< One of ::SceLsdbLiveAreaVerticalAlignment.
	SceInt32 lineAlignment; //!< One of ::SceLsdbLiveAreaHorizontalAlignment.
	SceInt32 origin; //!< One of ::SceLsdbLiveAreaElementOrigin.
	SceInt32 lineSpacing;
	SceUInt8 lineBreak;
	SceUInt8 wordWrap;
	SceUInt8 ellipsis;
	SceUInt8 wordScroll;
	SceUInt8 preBreak;
	SceUInt8 padding[3];
	SceInt32 marginTop;
	SceInt32 marginBottom;
	SceInt32 marginLeft;
	SceInt32 marginRight;
	SceInt32 titleSegmentIndex; //!< Index of the `title` segment, or -1 when absent.
	SceUInt8 textLayout[0x1C]; //!< Internal FW 3.60 PAF text-layout state.
} SceLsdbLiveAreaTextElement;
VITASDK_BUILD_ASSERT_EQ(0x70, SceLsdbLiveAreaTextElement); // size is from FW 3.60

/** One liveitem parsed from a LiveArea frame. */
typedef struct SceLsdbLiveAreaFrameItem {
	SceLsdbTimeRange timeRange; //!< Inclusive `from` and `until` interval.
	SceUInt8 isDefault; //!< Nonzero when the XML liveitem has `default="on"`.
	SceUInt8 padding;
	SceUInt16 minimumAge; //!< Selected `age-limit`, or 0 when absent.
	SceUInt16 maximumAge; //!< Selected `uage-limit`, or 255 when absent.
	SceUInt16 compatibilityMode; //!< One of ::SceLsdbLiveAreaCompatibilityMode, derived from `pokesute`.
	SceLsdbLiveAreaTargetElement *target; //!< Selected target element, or NULL.
	SceLsdbLiveAreaFrameElement **elements;
	SceSize elementCount;
	SceSize elementCapacity;
	SceUInt32 reserved1[2]; //!< Not initialized or interpreted on FW 3.60.
} SceLsdbLiveAreaFrameItem;
VITASDK_BUILD_ASSERT_EQ(0x30, SceLsdbLiveAreaFrameItem); // size is from FW 3.60

/** One parsed row of tbl_livearea_frame. */
typedef struct SceLsdbLiveAreaFrame {
	void *vtable; //!< Internal FW 3.60 virtual-function table.
	SceInt32 objectType; //!< Always ::SCE_LSDB_LIVEAREA_OBJECT_TYPE_FRAME.
	SceLsdbString frameId;
	SceInt32 multiMode; //!< One of ::SceLsdbLiveAreaFrameMultiMode.
	SceInt32 autoFlipInterval; //!< Automatic item-rotation interval from the `autoflip` attribute.
	SceInt64 revision;
	SceUInt8 isRetailOverride; //!< Nonzero for a database row whose type is 0.
	SceLsdbString userData;
	SceLsdbLiveAreaFrameItem **items;
	SceSize itemCount;
	SceSize itemCapacity;
} SceLsdbLiveAreaFrame;
VITASDK_BUILD_ASSERT_EQ(0x48, SceLsdbLiveAreaFrame); // size is from FW 3.60

/** Parsed LiveArea template and its heterogeneous object list. */
typedef struct SceLsdbLiveAreaFrameList {
	SceLsdbString style;
	SceLsdbString formatVersion;
	SceInt64 contentRevision;
	SceInt32 parsedFormatVersion; //!< Result of ::sceLsdbParseVersion.
	SceLsdbLiveAreaObjectListNode *objectListHead; //!< Circular-list sentinel; begin iteration at `objectListHead->next`.
	SceSize objectCount;
	SceRtcTick modifiedDate;
} SceLsdbLiveAreaFrameList;
VITASDK_BUILD_ASSERT_EQ(0x38, SceLsdbLiveAreaFrameList); // size is from FW 3.60

/**
 * Reference-counted ::SceLsdbLiveAreaFrameList object.
 * See ::SceLsdbAppInfo for the shared-reference ownership and cleanup rules.
 */
typedef struct SceLsdbLiveAreaFrameListRef {
	SceLsdbLiveAreaFrameList *object;
	void *control;
} SceLsdbLiveAreaFrameListRef;
VITASDK_BUILD_ASSERT_EQ(0x8, SceLsdbLiveAreaFrameListRef); // size is from FW 3.60

/** State used while parsing one LiveArea template. */
typedef struct SceLsdbLiveAreaParser {
	SceLsdbString contentsPath;
	SceUInt8 filterHardwareModel; //!< Nonzero to apply the XML `model` selector.
	SceUInt8 flag2; //!< Affects initial template acquisition; its purpose is unknown.
	SceUInt8 reserved; //!< Set to 0 on FW 3.60.
	SceUInt8 padding;
	SceLsdbLiveAreaFrameList *parsedFrames;
	void *parsedFramesControl; //!< Reference-count control block for parsedFrames.
	SceInt32 *languageIndex;
	const char *country;
	SceInt32 contentRating;
	SceUInt8 parseFlags[4]; //!< Their purposes are unknown.
	SceInt32 compatibilityMode; //!< One of ::SceLsdbLiveAreaCompatibilityMode; selects liveitems by their `pokesute` value.
	SceInt32 selectedLanguageIndex;
	SceLsdbString selectedCountry;
} SceLsdbLiveAreaParser;
VITASDK_BUILD_ASSERT_EQ(0x3C, SceLsdbLiveAreaParser); // size is from FW 3.60

/** Entry stored in ux0:iconlayout.ini. */
typedef struct SceLsdbIconLayoutEntry {
	SceInt32 pageNo;
	SceInt32 position;
	SceInt32 parentPageNo;
	SceInt32 parentPosition;
	char *parentTitleId;
	SceSize parentTitleIdLength;
} SceLsdbIconLayoutEntry;
VITASDK_BUILD_ASSERT_EQ(0x18, SceLsdbIconLayoutEntry); // size is from FW 3.60

/** Callback used to obtain the current content-rating configuration. */
typedef int (*SceLsdbContentRatingCallback)(SceInt32 *enabled, SceInt32 *rating);

/** Callback invoked once for each title enumerated by ::sceLsdbGetAllAppInfo. */
typedef void (*SceLsdbAppInfoCallback)(const SceLsdbAppInfo *appInfo, void *userData);

/**
 * Initialize an explicit AppDB wrapper.
 *
 * Both this function and ::sceLsdbAppDatabaseInit2 allocate the same backing
 * object on FW 3.60. The reason for exporting two entry points is unknown.
 */
SceLsdbAppDatabase *sceLsdbAppDatabaseInit(SceLsdbAppDatabase *database);
SceLsdbAppDatabase *sceLsdbAppDatabaseInit2(SceLsdbAppDatabase *database);

/** Release an explicit AppDB wrapper. Both entry points have the same behavior. */
SceLsdbAppDatabase *sceLsdbAppDatabaseEnd(SceLsdbAppDatabase *database);
SceLsdbAppDatabase *sceLsdbAppDatabaseEnd2(SceLsdbAppDatabase *database);

/** Query icon records on pages in the half-open interval [firstPageNo, endPageNo). */
int sceLsdbGetIconsInPageRange(SceLsdbAppDatabase *database, SceInt32 firstPageNo, SceInt32 endPageNo, SceLsdbIconInfoRefArray *icons);

/** Query \a pageCount consecutive page records beginning at \a firstPageNo. */
int sceLsdbGetPageInfo(SceLsdbAppDatabase *database, SceInt32 firstPageNo, SceSize pageCount, SceLsdbPageInfoRefArray *pages);

/** Delete the icon records belonging to one page. */
int sceLsdbDeleteIconsInPage(SceLsdbAppDatabase *database, SceInt32 pageNo, SceUInt32 flags);

/**
 * Update selected fields of one icon record.
 *
 * The record is selected by \a pageNo and ::SceLsdbIconInfo::position. Only
 * ::SceLsdbIconInfoUpdateFlag bits are interpreted; unlisted bits must not be
 * set. The selected scalar/string fields are read from \a iconInfo.
 *
 * ::SCE_LSDB_ICON_INFO_UPDATE_RENDERED_ICON_BLOB stores \a iconData and
 * \a iconDataSize in `tbl_appinfo_icon.reserved05`.
 * ::SCE_LSDB_ICON_INFO_UPDATE_ALTERNATE_ICON_PATH stores
 * ::SceLsdbIconInfo::iconPath as `reserved02`; it does not use \a iconData.
 * Input objects and buffers are borrowed for the duration of the call.
 *
 * @param[in] database - Open explicit AppDB wrapper.
 * @param[in] pageNo - Page containing the icon.
 * @param[in] iconInfo - Icon object whose position and selected fields are read.
 * @param[in] updateMask - Bitwise OR of ::SceLsdbIconInfoUpdateFlag values.
 * @param[in] iconData - Rendered PAF icon-cache bytes, or NULL when the blob
 *                       update bit is not set.
 * @param[in] iconDataSize - Size of \a iconData in bytes.
 *
 * @retval 0 The record was updated.
 * @retval SCE_LSDB_ERROR_INVALID_ARGUMENT \a iconInfo is invalid or
 *         \a updateMask is zero.
 * @retval SCE_LSDB_ERROR_INVALID_HANDLE The database is not open or the
 *         transaction could not be started.
 * @retval SCE_LSDB_ERROR_NOT_FOUND No matching row was changed.
 * @retval SCE_LSDB_ERROR_NO_MEMORY An internal allocation failed.
 * @return Other negative ActivityDb/SQLite errors can also be propagated.
 */
int sceLsdbUpdateIconInfo(SceLsdbAppDatabase *database, SceInt32 pageNo, const SceLsdbIconInfoRef *iconInfo, SceUInt32 updateMask, const void *iconData, SceSize iconDataSize);

/** Insert one icon record on \a pageNo. */
int sceLsdbInsertIcon(SceLsdbAppDatabase *database, SceInt32 pageNo, const SceLsdbIconInfoRef *iconInfo);

/** Delete the icon record and icon-layout entry belonging to \a titleId. */
int sceLsdbDeleteIconByTitleId(SceLsdbAppDatabase *database, const SceLsdbString *titleId);

/** Move corresponding source page/position entries to their destinations. */
int sceLsdbMoveIcons(SceLsdbAppDatabase *database, const SceLsdbInt32Array *sourcePageNos, const SceLsdbInt32Array *sourcePositions, const SceLsdbInt32Array *destinationPageNos, const SceLsdbInt32Array *destinationPositions);

/** Return the reference-counted blob in tbl_appinfo_icon.reserved05. */
SceLsdbDbBlobRef sceLsdbGetIconReservedBlob(SceLsdbAppDatabase *database, const SceInt64 *iconId, int *error);

/** Update one AppInfo page record. */
int sceLsdbUpdatePage(SceLsdbAppDatabase *database, SceInt32 pageNo, const SceLsdbPageInfo *pageInfo);

/** Insert a page record followed by its icon records. */
int sceLsdbInsertPage(SceLsdbAppDatabase *database, SceInt32 pageNo, const SceLsdbIconInfoRefArray *icons, const SceLsdbPageInfo *pageInfo);

/** Delete an AppInfo page record. */
int sceLsdbDeletePage(SceLsdbAppDatabase *database, SceInt32 pageNo);

/**
 * Insert an icon, creating its special negative page and parent relationship
 * when required.
 *
 * \a iconData and \a iconDataSize replace the icon blob only when both are
 * nonzero.
 */
int sceLsdbInsertIconWithParent(SceLsdbAppDatabase *database, SceInt32 pageNo, const SceLsdbIconInfoRef *parentIconInfo, const SceLsdbIconInfoRef *iconInfo, const void *iconData, SceSize iconDataSize);

/** Return the number of nonnegative AppInfo pages. */
int sceLsdbGetPageCount(SceLsdbAppDatabase *database);

/** Count icons whose page number is in the inclusive requested range. */
int sceLsdbGetIconCountInPageRange(SceLsdbAppDatabase *database, SceInt32 firstPageNo, SceInt32 lastPageNo);

/** Count top-level icons which count against the shell icon limit. */
int sceLsdbGetCountedIconCount(SceLsdbAppDatabase *database);

/** Get tbl_appinfo_icon.reserved01 for the first matching title and icon type. */
int sceLsdbGetIconId(SceLsdbAppDatabase *database, const SceLsdbString *title, SceInt32 iconType);

/** Load all AppInfo values for one title from an explicit database. */
int sceLsdbGetAppInfoFromDatabase(SceLsdbAppDatabase *database, const SceLsdbString *titleId, SceLsdbAppInfo *appInfo);

/** Read one typed AppInfo value from an explicit database. */
int sceLsdbGetAppInfoValueFromDatabase(SceLsdbAppDatabase *database, const SceLsdbString *titleId, SceUInt32 key, SceLsdbValue *value);

/** Return nonzero when tbl_appinfo contains at least one row for \a titleId. */
int sceLsdbAppInfoExists(SceLsdbAppDatabase *database, const SceLsdbString *titleId);

/**
 * Read icon information for \a titleId.
 *
 * When \a useOriginalPathFallback is nonzero and the title is not found, the
 * value of the title's `_org_path` AppInfo key is tried as a second title ID.
 */
int sceLsdbGetIconInfoByTitleId(SceLsdbAppDatabase *database, const SceLsdbString *titleId, SceLsdbIconInfoRef *iconInfo, SceInt32 *pageNo, SceBool useOriginalPathFallback);

/** Read an icon record by its tbl_appinfo_icon.reserved01 value. */
int sceLsdbGetIconInfoById(SceLsdbAppDatabase *database, const SceInt64 *iconId, SceInt32 *pageNo, SceLsdbIconInfoRef *iconInfo);

/**
 * Return title IDs matching a chain of AppInfo filters.
 *
 * Without ::SCE_LSDB_FILTER_FLAG_APPINFO_ONLY, FW 3.60 returns title IDs from
 * type-zero icon records whose AppInfo rows match \a filter. The other flags
 * affect only that icon-record query. Existing contents of \a titleIds are
 * released before the result is written. \a filter must not be NULL.
 *
 * \a flags is a bitwise OR of ::SceLsdbFilterFlags values.
 */
int sceLsdbGetFilteredTitleIdsFromDatabase(SceLsdbAppDatabase *database, const SceLsdbAppInfoFilter *filter, SceLsdbStringArray *titleIds, SceUInt32 flags);

/**
 * Return recently used titles in any requested application category.
 *
 * Results are ordered by the `#_lltime` AppInfo value in descending order.
 * A \a maxTitleCount value less than or equal to zero removes the result
 * limit. When \a categoryCount is zero, \a categories may be NULL and every
 * row which has a CATEGORY value is eligible. Existing contents of \a titleIds
 * are released before the result is written.
 */
int sceLsdbGetRecentlyUsedTitleIdsByCategoriesFromDatabase(SceLsdbAppDatabase *database, const char *const *categories, SceSize categoryCount, SceInt32 maxTitleCount, SceLsdbStringArray *titleIds);

/**
 * Resolve a URI to the title whose registered host/path is its longest prefix.
 *
 * Rows with the same prefix are ordered by descending last-launch and creation
 * times. A nonzero \a flags value enables the additional URI-row flag filter
 * used by FW 3.60.
 */
int sceLsdbResolveUriTitleIdFromDatabase(SceLsdbAppDatabase *database, const char *uri, SceLsdbString *titleId, SceUInt32 flags);

/** Collect distinct `scheme:host/path` prefixes for the requested schemes. */
int sceLsdbGetUriPrefixesFromDatabase(SceLsdbAppDatabase *database, const char *const *schemes, SceSize schemeCount, SceLsdbStringArray *uriPrefixes);

/** Insert the AppInfo, icon, URI, and icon-layout records for one application. */
int sceLsdbInsertApplication(SceLsdbAppDatabase *database, const SceLsdbAppInfo *appInfo, SceInt32 pageNo, const SceLsdbIconInfoRef *iconInfo, const SceLsdbAppInfoArray *additionalAppInfo);

/** Delete all AppDB data belonging to \a titleId. */
int sceLsdbDeleteTitleData(SceLsdbAppDatabase *database, const SceLsdbString *titleId, SceBool keepSpecialPage, SceInt32 *pageNo);

/** Update the AppInfo, icon, and URI records for one application. */
int sceLsdbUpdateApplication(SceLsdbAppDatabase *database, const SceLsdbAppInfo *appInfo, SceLsdbIconInfoRef *iconInfo, const SceLsdbAppInfoArray *additionalAppInfo);

/** Replace the AppInfo and URI records for one application, optionally updating its icon. */
int sceLsdbReplaceApplication(SceLsdbAppDatabase *database, const SceLsdbAppInfo *appInfo, const SceLsdbIconInfoRef *iconInfo, SceBool updateIcon, const SceLsdbAppInfoArray *additionalAppInfo);

/**
 * Apply a masked update to a title's AppInfo value through an explicit database.
 *
 * For integer values, bits selected by \a mask are copied from \a value. String
 * values are replaced.
 */
int sceLsdbSetAppInfoValueMaskedFromDatabase(SceLsdbAppDatabase *database, const SceLsdbString *titleId, SceUInt32 key, const SceLsdbValue *value, SceUInt64 mask);

/** Return a config string, or \a defaultValue when the key is absent. */
SceLsdbString sceLsdbGetConfigStringFromDatabase(SceLsdbAppDatabase *database, SceUInt32 key, const char *defaultValue);

/** Return a 64-bit config value, or \a defaultValue when the key is absent. */
SceInt64 sceLsdbGetConfigInt64FromDatabase(SceLsdbAppDatabase *database, SceUInt32 key, SceInt64 defaultValue);

/** Store a string in tbl_config through an explicit database. */
int sceLsdbSetConfigStringFromDatabase(SceLsdbAppDatabase *database, SceUInt32 key, const char *value);

/**
 * Apply a masked update to an integer tbl_config value through an explicit database.
 *
 * The resulting value is (oldValue & ~mask) | (value & mask).
 */
int sceLsdbSetConfigInt64MaskedFromDatabase(SceLsdbAppDatabase *database, SceUInt32 key, SceInt64 value, SceUInt64 mask);

/**
 * Synchronize page and system-icon rows with a Shell theme-layout object.
 *
 * \a layout and \a layout->object must not be NULL. \a updateFlags is a
 * bitwise OR of ::SceLsdbThemeLayoutUpdateFlag values. Unselected portions of
 * the layout are not read.
 */
int sceLsdbUpdateThemeLayout(SceLsdbAppDatabase *database, const SceLsdbThemeLayoutRef *layout, SceUInt32 updateFlags);

/**
 * Replace or insert one theme record and its localized strings.
 *
 * The selected member at the start of each localized-string collection is not
 * read. The 20 localized members are stored for language indices 0 through 19.
 */
int sceLsdbReplaceTheme(SceLsdbAppDatabase *database, const SceLsdbThemeInfo *themeInfo, const SceLsdbLocalizedThemeStrings *localizedTitles, const SceLsdbLocalizedThemeStrings *localizedProviders);

/** Delete one theme record. */
int sceLsdbDeleteTheme(SceLsdbAppDatabase *database, const SceLsdbString *themeId);

/** Delete all theme records. */
int sceLsdbDeleteAllThemes(SceLsdbAppDatabase *database);

/** Return the number of theme records. */
int sceLsdbGetThemeCount(SceLsdbAppDatabase *database);

/**
 * Begin an explicit AppDB transaction.
 *
 * @retval 0 The transaction was started.
 * @retval SCE_LSDB_ERROR_INVALID_HANDLE The database is not open.
 * @return Other negative ActivityDb/SQLite errors can also be propagated.
 */
int sceLsdbAppDatabaseBeginTransaction(SceLsdbAppDatabase *database);

/**
 * End an explicit AppDB transaction.
 *
 * Despite earlier tentative documentation, the second argument is not a
 * notification type and this function does not send an AppDB notification.
 * Any negative \a databaseVersionOrRollback rolls the transaction back. A
 * nonnegative value is written as the primary AppDB version before the
 * transaction is committed. Use -1 to roll back or 59 to commit a FW 3.60
 * AppDB. Another nonnegative version can make the database fail its next
 * compatibility check.
 *
 * A successful commit also rotates the previous `<database-path>-err` file and
 * sets `/CONFIG/SHELL/appdb_rebuild_flag` to 1. It does not invalidate
 * SceShell's in-memory icon cache.
 *
 * @retval 0 The transaction was committed or rolled back.
 * @retval SCE_LSDB_ERROR_INVALID_HANDLE The database is not open or no valid
 *         transaction exists.
 * @return Other negative ActivityDb/SQLite errors can also be propagated.
 */
int sceLsdbAppDatabaseEndTransaction(SceLsdbAppDatabase *database, SceInt32 databaseVersionOrRollback);

/**
 * Enumerate all AppInfo objects ordered by title ID.
 *
 * \a callback is invoked once for each title. The callback receives a temporary
 * reference which is released after it returns.
 */
int sceLsdbGetAllAppInfo(SceLsdbAppDatabase *database, SceLsdbAppInfoCallback callback, void *userData);

/**
 * Register a process-local AppInfo observer.
 *
 * Re-registering an initialized \a observer first removes its previous
 * registration. The callback and user-data pointers are stored directly in
 * \a observer; they are not copied into separately owned storage. A NULL
 * \a callback removes an existing registration without adding another one.
 *
 * @param[in,out] observer - Non-NULL, zero-initialized holder which must remain
 *                           alive and stationary until it is unregistered.
 * @param[in] callback - Callback invoked synchronously for AppInfo changes.
 * @param[in] userData - Opaque value passed to \a callback.
 *
 * @retval 0 The observer was registered.
 * @retval SCE_LSDB_ERROR_INVALID_HANDLE The process-global AppDB is not open.
 */
int sceLsdbRegisterAppInfoObserver(SceLsdbAppInfoObserver *observer, SceLsdbAppInfoObserverCallback callback, void *userData);

/**
 * Unregister a process-local AppInfo observer and clear its two fields.
 *
 * \a observer must be non-NULL. It may already be empty.
 *
 * @retval 0 The observer was unregistered or was already empty.
 */
int sceLsdbUnregisterAppInfoObserver(SceLsdbAppInfoObserver *observer);

/** Initialize a query-context object to the closed state. */
SceLsdbQueryContext *sceLsdbQueryContextInit(SceLsdbQueryContext *context);
SceLsdbQueryContext *sceLsdbQueryContextInit2(SceLsdbQueryContext *context);

/** Close a query context. */
int sceLsdbQueryContextClose(SceLsdbQueryContext *context);

/** Release a query-context object. */
SceLsdbQueryContext *sceLsdbQueryContextEnd(SceLsdbQueryContext *context);
SceLsdbQueryContext *sceLsdbQueryContextEnd2(SceLsdbQueryContext *context);

/** Open the process-global AppDB in a query context. */
int sceLsdbQueryContextOpen(SceLsdbQueryContext *context);

/** Load all AppInfo values for one title. */
int sceLsdbQueryGetAppInfo(SceLsdbQueryContext *context, const SceLsdbString *titleId, SceLsdbAppInfo *appInfo);

/** Read one typed AppInfo value. */
int sceLsdbQueryGetAppInfoValue(SceLsdbQueryContext *context, const SceLsdbString *titleId, SceUInt32 key, SceLsdbValue *value);

/** Return nonzero when a title has AppInfo rows. */
int sceLsdbQueryAppInfoExists(SceLsdbQueryContext *context, const SceLsdbString *titleId);

/** Read icon information for a title. */
int sceLsdbQueryGetIconInfo(SceLsdbQueryContext *context, const SceLsdbString *titleId, SceLsdbIconInfoRef *iconInfo, SceBool includeHidden);

/**
 * Return title IDs matching a chain of AppInfo filters.
 *
 * This is the process-global AppDB counterpart of
 * ::sceLsdbGetFilteredTitleIdsFromDatabase.
 */
int sceLsdbQueryGetFilteredTitleIds(SceLsdbQueryContext *context, const SceLsdbAppInfoFilter *filter, SceLsdbStringArray *titleIds, SceUInt32 flags);

/**
 * Return recently used titles in any requested application category.
 *
 * This is the process-global AppDB counterpart of
 * ::sceLsdbGetRecentlyUsedTitleIdsByCategoriesFromDatabase.
 */
int sceLsdbQueryGetRecentlyUsedTitleIdsByCategories(SceLsdbQueryContext *context, const char *const *categories, SceSize categoryCount, SceInt32 maxTitleCount, SceLsdbStringArray *titleIds);

/** Resolve a URI through the process-global AppDB connection. */
int sceLsdbQueryResolveUriTitleId(SceLsdbQueryContext *context, const char *uri, SceLsdbString *titleId, SceUInt32 flags);

/** Collect distinct URI prefixes through the process-global AppDB connection. */
int sceLsdbQueryGetUriPrefixes(SceLsdbQueryContext *context, const char *const *schemes, SceSize schemeCount, SceLsdbStringArray *uriPrefixes);

/** Return a config string, or \a defaultValue when the key is absent. */
SceLsdbString sceLsdbQueryGetConfigString(SceLsdbQueryContext *context, SceUInt32 key, const char *defaultValue);

/** Return a 64-bit config value, or \a defaultValue when the key is absent. */
SceInt64 sceLsdbQueryGetConfigInt64(SceLsdbQueryContext *context, SceUInt32 key, SceInt64 defaultValue);

/**
 * Test the query-context notification flag and optionally clear it.
 *
 * When \a clear is nonzero, FW 3.60 atomically changes the exact value 1 to 0.
 * A zero value only tests the flag.
 */
int sceLsdbQueryContextTestAndClearFlag(SceLsdbQueryContext *context, SceBool clear);

/** Return theme IDs ordered by descending modification time. */
int sceLsdbQueryGetThemeIds(SceLsdbQueryContext *context, SceLsdbStringArray *themeIds);

/** Read one localized theme record. */
int sceLsdbQueryGetTheme(SceLsdbQueryContext *context, const SceLsdbString *themeId, SceInt32 languageIndex, SceLsdbThemeInfo *themeInfo);

/** Return the sum of tbl_theme.size. */
SceInt64 sceLsdbQueryGetTotalThemeSize(SceLsdbQueryContext *context);

/**
 * Apply a masked update to a title's AppInfo value.
 *
 * For integer values, bits selected by \a mask are copied from \a value. String
 * values are replaced.
 */
int sceLsdbSetAppInfoValueMasked(const SceLsdbString *titleId, SceUInt32 key, const SceLsdbValue *value, SceUInt64 mask);

/** Read one AppInfo value through the process-global AppDB connection. */
int sceLsdbGetAppInfoValue(const SceLsdbString *titleId, SceUInt32 key, SceLsdbValue *value);

/** Store a string in tbl_config through the process-global AppDB connection. */
int sceLsdbSetConfigValue(SceUInt32 key, const char *value);

/**
 * Apply a masked update to an integer tbl_config value.
 *
 * The resulting value is (oldValue & ~mask) | (value & mask).
 */
int sceLsdbSetConfigValueMasked(SceUInt32 key, SceInt64 value, SceUInt64 mask);

/**
 * Refresh the AppDB rows derived from an AppInfo object.
 *
 * First call ::sceLsdbOpen with ::SCE_LSDB_OPEN_MODE_NORMAL, call this
 * function while the process-global connection remains open, and then call
 * ::sceLsdbClose. This function does not open or close the AppDB itself.
 *
 * FW 3.60 upserts every key/value pair supplied in \a appInfo into
 * `tbl_appinfo`. A TITLE_ID entry is required. SUPPORT_URI values also cause
 * the corresponding derived `tbl_uri` rows to be rebuilt. Localized STITLE
 * keys are updated only when the caller includes them. The function invokes
 * process-local SceLsdb observers with
 * ::SCE_LSDB_APP_INFO_EVENT_UPDATE_APP_INFO and commits the operation.
 *
 * This function does not read `sce_sys/icon0.png` or `sce_sys/pic0.png`, copy
 * either image into `ur0:appmeta`, update `tbl_appinfo_icon`, update its
 * localized bubble title, generate the `reserved05` rendered-icon blob,
 * invalidate SceShell's in-memory icon cache, or send the AppDB notification
 * expected by SceShell after a broader database change. App promotion and
 * recovery prepare paths such as `ur0:appmeta/<TITLE_ID>/icon0.png` separately.
 * SceShell's registered FW 3.60 observer for this event re-queries the icon row
 * to reconcile a changed parental-lock level; it does not discard or rebuild
 * the rendered icon cache.
 *
 * SceShell's separate icon-cache builder reads the path already stored in the
 * icon row, which can be that appmeta path or an application `sce_sys/icon0.png`
 * path, then writes the rendered cache through ::sceLsdbUpdateIconInfo.
 *
 * A pic0-related AppInfo key/path is stored if it is present in \a appInfo,
 * but no image is opened, copied, decoded, or cached. There is no dedicated
 * pic0 refresh operation in this entry point. Language-wide localized AppInfo,
 * icon-title, and LiveArea updates use ::sceLsdbUpdateLocalizedData instead.
 *
 * The supplied reference and all nested values are borrowed for the duration
 * of the call. FW 3.60 performs no process-ID, application-lifecycle,
 * foreground-title, or current-title check, so the currently running or
 * foreground title can be refreshed. The call may run on a worker thread; it
 * has no main-thread restriction. It has the same private-module, AppDB-path,
 * and registry privilege requirements described by ::sceLsdbOpen. Do not call
 * it from an interrupt or exception context, and do not close or reopen the
 * process-global AppDB concurrently.
 *
 * @param[in] appInfo - Non-NULL AppInfo map containing at least TITLE_ID.
 *
 * @retval 0 The supplied rows and derived URI records were refreshed.
 * @retval SCE_LSDB_ERROR_INVALID_HANDLE ::sceLsdbOpen has not established a
 *         usable process-global database connection.
 * @retval SCE_LSDB_ERROR_REQUIRED_APPINFO_FIELD_MISSING TITLE_ID is absent.
 * @retval SCE_LSDB_ERROR_NO_MEMORY An internal allocation failed.
 * @return Other negative ActivityDb/SQLite errors can also be propagated.
 */
int sceLsdbRefreshAppInfo(const SceLsdbAppInfo *appInfo);

/** Return the FNV-1a key for STITLE or STITLE_00 through STITLE_19. */
SceUInt32 sceLsdbGetStitleKeyForLanguage(SceInt32 languageIndex);

/**
 * AppInfo field accessors.
 *
 * String results point into \a appInfo and remain valid only while that object
 * remains alive. Integer fields use their database value or the FW 3.60
 * default selected by the getter.
 */
const char *sceLsdbGetStitle(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetTitle(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetPspEmulatorTitleId(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetOriginalPath(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetAttribute(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetLiveAreaType(const SceLsdbAppInfo *appInfo);

/**
 * Return the SELF type-6 total memory budget in 1-KiB units.
 *
 * SceShell converts this value to bytes before passing it to AppMgr.
 */
SceInt64 sceLsdbGetTotalMemoryBudget(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetCategory(const SceLsdbAppInfo *appInfo);

/** Return nonzero when CATEGORY is `gda`. */
int sceLsdbIsCategoryGda(const SceLsdbAppInfo *appInfo);

SceInt64 sceLsdbGetPromoteAttribute(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetName(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetForegroundTitleId(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetBackgroundTitleId(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetTitleId(const SceLsdbAppInfo *appInfo);

/**
 * Return SceShell's application-type classification.
 *
 * SceShell stores 0 for `gda`, `gdb`, and `wda` applications; 1 for ordinary
 * applications; and 2 for `NPXS10998`. Consumers on FW 3.60 also recognize
 * value 3 for an application record without a SELF path.
 */
SceInt64 sceLsdbGetShellApplicationType(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetSelfPath(const SceLsdbAppInfo *appInfo);

/** Return the SELF type-6 physically contiguous memory budget in 1-KiB units. */
SceInt64 sceLsdbGetPhysicalContiguousMemoryBudget(const SceLsdbAppInfo *appInfo);

/** Return the attribute word from the SELF type-6 boot parameters. */
SceInt64 sceLsdbGetBootParamAttribute(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetAppVer(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetContentId(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetNpCommunicationId(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetSystemVersion(const SceLsdbAppInfo *appInfo);

/** Query game_plugin's application-type classification for \a appInfo. */
SceInt32 sceLsdbGetType(const SceLsdbAppInfo *appInfo);

/**
 * Query game_plugin's metadata-location classification for \a appInfo.
 *
 * Consumers inspected on FW 3.60 read `ur0:appmeta/<titleId>/param.sfo` for
 * value 1. Values 2 and 3 select a param.sfo obtained through AppData mount
 * ID 110. The distinction between values 2 and 3 is unknown.
 */
SceInt32 sceLsdbGetAppMetadataLocationType(const SceLsdbAppInfo *appInfo);

const char *sceLsdbGetDiscId(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetGameDataId(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetParentalLevel(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetBootable(const SceLsdbAppInfo *appInfo);

/** Return ATTRIBUTE bits 13 and 14. Their purpose is unknown. */
SceUInt32 sceLsdbGetAttributeBits13To14(const SceLsdbAppInfo *appInfo);

/** Return ATTRIBUTE bit 15. Its purpose is unknown. */
SceUInt32 sceLsdbGetAttributeBit15(const SceLsdbAppInfo *appInfo);

const char *sceLsdbGetBootInstallDir(const SceLsdbAppInfo *appInfo);

/** Return nonzero for a game category or legacy system-version value 1 or 2. */
int sceLsdbIsGameCategoryOrLegacySystemVersion(const SceLsdbAppInfo *appInfo);

/** Return nonzero for a game category or the `EG` category. */
int sceLsdbIsGameOrEgCategory(const SceLsdbAppInfo *appInfo);

/** Return nonzero when type is 1 and ATTRIBUTE bit 15 is clear. */
int sceLsdbIsType1AndAttributeBit15Clear(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetContentLocation(const SceLsdbAppInfo *appInfo);

/** Return the resolved path to the application's `sce_sys/pic0.png` image. */
const char *sceLsdbGetPic0Path(const SceLsdbAppInfo *appInfo);

/** Return ATTRIBUTE bit 16. Its purpose is unknown. */
int sceLsdbGetAttributeBit16(const SceLsdbAppInfo *appInfo);

const char *sceLsdbGetLastLaunchTime(const SceLsdbAppInfo *appInfo);
SceInt64 sceLsdbGetUpdateStatus(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetSupportUri(const SceLsdbAppInfo *appInfo);
const char *sceLsdbGetSavedataAddInstallDir(const SceLsdbAppInfo *appInfo);

/** Return ATTRIBUTE bits 34 through 36. Their purpose is unknown. */
SceInt64 sceLsdbGetAttributeBits34To36(const SceLsdbAppInfo *appInfo);

/**
 * Select the LiveArea compatibility mode for \a titleId.
 *
 * The default is ::SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_UNSPECIFIED. A
 * Japanese title accepted by SceCompatForVsh can produce
 * ::SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_ON or
 * ::SCE_LSDB_LIVEAREA_COMPATIBILITY_MODE_OFF depending on game_plugin state
 * on FW 3.60.
 *
 * @return One of ::SceLsdbLiveAreaCompatibilityMode.
 */
SceInt32 sceLsdbGetLiveAreaCompatibilityMode(const char *titleId);

/**
 * Open the process-global AppDB connection.
 *
 * Call ::sceLsdbOpen with ::SCE_LSDB_OPEN_MODE_NORMAL before using global
 * query or refresh entry points, keep the connection open until all such work
 * has finished, then call ::sceLsdbClose. Do not call Open twice without an
 * intervening Close. ::SCE_LSDB_OPEN_MODE_REBUILD is destructive: it removes
 * and reconstructs the AppDB before opening it and should be used only for
 * database recovery.
 *
 * FW 3.60 stores the disk database at `ur0:shell/db/app.db`. The absolute value
 * of \a mode is checked and stored as its primary version; the supported value
 * is 59. Other values are not additional access flags and produce an
 * incompatible database.
 *
 * SceLsdb is a private user-mode module at
 * `vs0:vsh/shell/livespace_db.suprx`. It performs no explicit caller-PID check,
 * but the process must be authorized to load that module and access the Shell
 * database and registry state. An ordinary sandboxed application does not
 * have that calling environment. Call from a normal user thread on which
 * blocking filesystem/database work is allowed, not from an interrupt or
 * exception context. Worker threads are supported; there is no main-thread
 * restriction. The connection is process-global, so callers must serialize
 * Open, Close, Reopen, transactions, queries, and refreshes.
 *
 * @param[in] mode - One of ::SceLsdbOpenMode.
 *
 * @retval 0 The AppDB was opened.
 * @retval SCE_LSDB_ERROR_NO_MEMORY An internal allocation failed.
 * @retval SCE_LSDB_ERROR_DATABASE_LANGUAGE_MISMATCH The stored system-language
 *         component differs from the current value.
 * @retval SCE_LSDB_ERROR_DATABASE_CONFIGURATION_MISMATCH The stored
 *         region/environment component differs from the current value.
 * @retval SCE_LSDB_ERROR_DATABASE_SCHEMA_TOO_NEW The on-disk schema is newer
 *         than the FW 3.60 implementation supports.
 * @retval SCE_LSDB_ERROR_DATABASE_VERSION_MISMATCH The primary AppDB version
 *         is not 59.
 * @retval SCE_LSDB_ERROR_INVALID_DATABASE_VERSION The primary or schema
 *         version is zero or otherwise uninitialized.
 * @return ActivityDb/Dbutil errors, including 0x801010F5, 0x801010FE, and
 *         0x801010FF, can also be propagated; their exact failure split is
 *         unknown.
 */
int sceLsdbOpen(SceLsdbOpenMode mode);

/**
 * Close the process-global AppDB connection and release its model helpers.
 *
 * Do not call this while another thread is using the connection. The FW 3.60
 * wrapper returns 0, including when the connection is already closed.
 */
int sceLsdbClose(void);

/**
 * Replace the process-global disk connection with an in-memory database.
 *
 * FW 3.60 closes the current handle and opens `:memory:` with version 0. This
 * is not an ordinary reopen of `ur0:shell/db/app.db` and must not be used to
 * implement the normal Open/Close lifecycle.
 */
int sceLsdbReopen(void);

/** Update localized AppInfo, icon-title, and LiveArea data. */
int sceLsdbUpdateLocalizedData(SceInt32 languageIndex);

/** Return the language index stored in the Activity database. */
SceInt32 sceLsdbGetDatabaseLanguageIndex(void);

/** Clear NewEvent state and refresh the LiveArea database. */
int sceLsdbResetNewEventsAndLiveArea(void);

/** Return the most recently recorded AppDB error. */
int sceLsdbGetLastAppDatabaseError(void);

/** Return the configured LiveArea path, or `#_livearea_path` when absent. */
SceLsdbString sceLsdbGetLiveAreaPath(void);

/** Set the configured LiveArea path. */
int sceLsdbSetLiveAreaPath(const SceLsdbString *value);

/** Return the shell's current system-language index, or 1 when unavailable. */
SceInt32 sceLsdbGetSystemLanguageIndex(void);

/** Return the configured system region. */
SceLsdbString sceLsdbGetRegion(void);

/** Map an explicit/current/NP language selection to a language index. */
SceInt32 sceLsdbMapLanguageIndex(SceInt32 mode, SceUInt32 language);

/** Return the current NP language string. */
SceLsdbString sceLsdbGetNpLanguage(void);

/** Return the configured country, with the mode-2 telephony override applied. */
SceLsdbString sceLsdbGetCountry(SceInt32 mode);

/** Normalize an integer to 0 or 1. */
SceBool sceLsdbNormalizeBool(SceInt32 value);

/** Install the callback used when obtaining content-rating information. */
SceLsdbContentRatingCallback sceLsdbSetContentRatingCallback(SceLsdbContentRatingCallback callback);

/** Return the current user content-rating or age value. */
SceInt32 sceLsdbGetUserContentRating(void);

/** Return the current NP environment string. */
SceLsdbString sceLsdbGetNpEnvironment(void);

/** Initialize a NewEvent database wrapper. */
SceLsdbNewEventDatabase *sceLsdbNewEventDatabaseInit(SceLsdbNewEventDatabase *database);
SceLsdbNewEventDatabase *sceLsdbNewEventDatabaseInit2(SceLsdbNewEventDatabase *database);

/** Open the NewEvent database. */
int sceLsdbNewEventDatabaseOpen(SceLsdbNewEventDatabase *database);

/** Close the NewEvent database. */
int sceLsdbNewEventDatabaseClose(SceLsdbNewEventDatabase *database);

/** Insert one event and apply the FW 3.60 pruning policy when requested. */
int sceLsdbInsertNewEvent(const SceLsdbNewEvent *event, SceBool prune);

/**
 * Update selected fields of existing, nondeleted NewEvent rows.
 *
 * Each object identifies a row by a nonzero rowId or by nonempty titleId and
 * itemId strings. Only fields selected by \a updateMask are compared and
 * written. Every changed row receives an incremented hash value.
 *
 * When the time field is selected, a nonempty updateTime must begin with a
 * `YYYY-MM-DD HH:MM:SS` timestamp. When icon data is selected, a nonzero
 * iconDataSize requires a non-NULL iconData pointer. If both the stored and
 * supplied popupFlag values are nonzero, FW 3.60 preserves a copy of the
 * previous popup row before assigning a new popup number to the updated row.
 *
 * @param[in] events - Array of reference-counted NewEvent objects.
 * @param[in] count - Number of elements in \a events.
 * @param[in] updateMask - Bitwise OR of ::SceLsdbNewEventUpdateFlag values;
 *                         must not be zero.
 *
 * @return 0 when at least one row changed, or a negative ::SceLsdbErrorCode
 *         value.
 */
int sceLsdbUpdateNewEvents(const SceLsdbNewEventRef *events, SceSize count, SceUInt32 updateMask);

/** Set del_flag for the selected events. */
int sceLsdbMarkNewEventsDeleted(const SceLsdbNewEventSelector *selectors, SceSize count);

/** Delete events according to the supplied byte threshold. */
int sceLsdbDeleteNewEvents(SceUInt8 threshold);

/** Read one selected NewEvent row. */
int sceLsdbGetNewEvent(SceLsdbNewEventDatabase *database, SceLsdbNewEvent *event);

/**
 * Enumerate nondeleted NewEvent rows.
 *
 * \a maxCount must not exceed 64. When \a clearNotification is nonzero, the
 * process-local pending-notification flag is cleared after the query.
 */
int sceLsdbGetNewEvents(SceLsdbNewEventDatabase *database, SceLsdbNewEventRefArray *events, SceSize maxCount, SceSize *totalCount, SceSize *newCount, SceBool clearNotification);

/** Read the icon blob for a NewEvent row. */
SceLsdbDbBlobRef sceLsdbGetNewEventIconData(SceLsdbNewEventDatabase *database, const SceInt64 *rowId, int *error);

/** Return whether the NewEvent database has a pending notification. */
SceBool sceLsdbHasPendingNewEventNotification(SceLsdbNewEventDatabase *database);

/** Clear the process-local pending NewEvent notification state. */
int sceLsdbClearPendingNewEventNotification(SceLsdbNewEventDatabase *database);

/** Return the number of references held by the NewEvent manager. */
int sceLsdbGetNewEventReferenceCount(SceLsdbNewEventDatabase *database);

/** Read the first row whose popup_no is greater than zero. */
int sceLsdbGetFirstPopupNewEvent(SceLsdbNewEventDatabase *database, SceLsdbNewEvent *event);

/** Set popup_no to zero for the selected row. */
int sceLsdbClearNewEventPopup(SceLsdbNewEventDatabase *database, const SceInt64 *rowId);

/** Set popup_no to zero for all rows. */
int sceLsdbClearNewEventPopups(SceLsdbNewEventDatabase *database);

/**
 * Update new_flag for an array of existing, nondeleted NewEvent rows.
 *
 * Each object identifies a row by a nonzero rowId or by nonempty titleId and
 * itemId strings. Every changed row receives an incremented hash value. This
 * specialized operation suppresses the popup-row preservation performed by
 * ::sceLsdbUpdateNewEvents.
 */
int sceLsdbUpdateNewEventFlags(SceLsdbNewEventDatabase *database, const SceLsdbNewEventRef *events, SceSize count);

/** Clear new_flag and increment hash for every new row. */
int sceLsdbClearNewEventFlags(SceLsdbNewEventDatabase *database);

/** Select the event operation identified by the LAUPDATE token. */
int sceLsdbSelectLiveAreaUpdateEvent(SceLsdbNewEventDatabase *database, const SceLsdbNewEventSelector *selector);

/**
 * Test a time interval against the current RTC or ad-network clock.
 *
 * The interval endpoints are inclusive.
 */
int sceLsdbIsCurrentTimeInRange(const SceLsdbTimeRange *range, SceBool mode, SceBool useAdNetworkClock);

/** Initialize one parsed LiveArea frame. */
SceLsdbLiveAreaFrame *sceLsdbLiveAreaFrameInit(SceLsdbLiveAreaFrame *frame);

/** Return one indexed liveitem from a parsed frame, or NULL. */
SceLsdbLiveAreaFrameItem *sceLsdbLiveAreaFrameGetItem(const SceLsdbLiveAreaFrame *frame, SceSize index);

/** Initialize an empty parsed LiveArea object list. */
SceLsdbLiveAreaFrameList *sceLsdbLiveAreaFrameListInit(SceLsdbLiveAreaFrameList *frameList);

/** Release a parsed LiveArea object list. */
SceLsdbLiveAreaFrameList *sceLsdbLiveAreaFrameListEnd(SceLsdbLiveAreaFrameList *frameList);

/** Parse a NN.NN version string into the integer representation used by SceLsdb. */
SceInt32 sceLsdbParseVersion(const SceLsdbString *version);
SceInt32 sceLsdbParseVersion2(const SceLsdbString *version);

/**
 * Initialize a LiveArea parser for \a contentsPath.
 *
 * When \a filterHardwareModel is nonzero, XML choices whose `model` attribute
 * does not match the current hardware are discarded. On FW 3.60, \a flag2
 * changes the initial template-acquisition path in
 * ::sceLsdbParseLiveAreaTemplate; its purpose is unknown.
 */
SceLsdbLiveAreaParser *sceLsdbLiveAreaParserInit(SceLsdbLiveAreaParser *parser, const char *contentsPath, SceBool filterHardwareModel, SceBool flag2);

/** Release a LiveArea parser. */
SceLsdbLiveAreaParser *sceLsdbLiveAreaParserEnd(SceLsdbLiveAreaParser *parser);

/** Parse the template selected by \a parser. */
int sceLsdbParseLiveAreaTemplate(SceLsdbLiveAreaParser *parser);

/**
 * Apply locale and content-rating selection to a parsed template.
 *
 * \a languageIndex points to the selected language, \a country is the raw
 * country code, and \a contentRating is the current user rating.
 */
int sceLsdbLiveAreaParserSetLocale(SceLsdbLiveAreaParser *parser, SceInt32 *languageIndex, const char *country, SceInt32 contentRating);

/** Return the default LiveArea template path. */
SceLsdbString sceLsdbGetDefaultLiveAreaTemplatePath(void);

/** Read the current RTC tick. */
int sceLsdbGetCurrentRtcTick(SceRtcTick *tick);

/** Read the ad-network RTC tick. */
int sceLsdbGetAdNetworkRtcTick(SceRtcTick *tick);

/**
 * Parse one LiveArea frame XML document.
 *
 * The final pointer is accepted by the export but ignored on FW 3.60.
 */
SceBool sceLsdbParseLiveAreaFrame(SceLsdbLiveAreaFrame **frame, const void *xml, SceSize xmlLength, SceInt32 *languageIndex, const char *country, SceInt32 contentRating, SceBool filterHardwareModel, const void *ignored);

/**
 * Read and parse the stored LiveArea template for one title and target type.
 *
 * \a unused is ignored on FW 3.60. \a targetType is one of
 * ::SceLiveAreaTargetType; the byte-sized declaration matches this export's
 * ABI.
 */
int sceLsdbGetLiveAreaFrames(void *unused, SceLsdbLiveAreaFrameListRef *frames, const SceLsdbString *titleId, SceUInt8 targetType);

/** Enumerate titles which have stored LiveArea data. */
int sceLsdbGetLiveAreaTitleIds(void *unused, const SceLsdbStringArray *existingTitleIds, SceLsdbString *titleId);

/**
 * Read tbl_livearea_frame.userdata.
 *
 * Both \a unused and \a activityDbHandle are ignored on FW 3.60.
 */
int sceLsdbGetLiveAreaFrameUserData(void *unused, const SceLsdbString *titleId, const SceLsdbString *frameId, SceLsdbString *userData, void *activityDbHandle);

/**
 * FW 3.60 no-op retained as an exported entry point.
 *
 * SceShell passes the title ID as a ::SceLsdbString object and the LiveArea
 * contents path as a NUL-terminated string, but FW 3.60 ignores both
 * arguments.
 */
void sceLsdb_29B275BA(const SceLsdbString *titleId, const char *contentsPath);

/**
 * Refresh LiveArea rows from their stored contents paths.
 *
 * An empty \a titleId refreshes every title. For a nonempty title,
 * \a originalPath receives its stored `org_path` when non-NULL. Per-title
 * refresh failures are normalized to success on FW 3.60; database setup
 * failures are returned.
 */
int sceLsdbRefreshLiveAreaRecords(const SceLsdbString *titleId, SceLsdbString *originalPath);

/** Replace all stored LiveArea contents for one title. */
int sceLsdbReplaceLiveAreaContents(void *handle, const SceLsdbString *titleId, const SceLsdbString *contentsPath, const SceRtcTick *lastModified, SceUInt32 changeFlags, SceLsdbString *resolvedTitleId, ScePID processId);

/** Update one or more parsed LiveArea frame records. */
int sceLsdbUpdateLiveAreaFrames(void *handle, const SceLsdbString *titleId, const SceLsdbLiveAreaFrameListRef *frames, const SceLsdbString *contentsPath, SceUInt8 targetType, SceUInt32 changeFlags, SceLsdbString *resolvedTitleId, ScePID processId);

/**
 * Return whether a system-function-zone entry has the requested type.
 *
 * \a unused is ignored on FW 3.60.
 */
int sceLsdbLiveAreaSfEntryExists(void *unused, const SceLsdbString *titleId, SceInt32 type);

/** Read pccUpdDate, pccResStatus, and pccSignOff for one title. \a unused is ignored on FW 3.60. */
int sceLsdbGetLiveAreaPccInfo(void *unused, const SceLsdbString *titleId, SceLsdbLiveAreaPccInfo *pccInfo);

/**
 * Update PCC information and the empty system-function-zone slot for one title.
 *
 * \a unused is ignored on FW 3.60. When \a addSystemFunctionZoneSlot is
 * nonzero, an empty slot is added to the system-function-zone entry if needed;
 * when it is zero, one empty slot is removed. \a updateFlags is a bitwise OR
 * of ::SceLsdbLiveAreaPccUpdateFlag values and selects the fields copied from
 * \a pccInfo.
 */
int sceLsdbUpdateLiveAreaPccInfo(void *unused, const SceLsdbString *titleId, SceBool addSystemFunctionZoneSlot, const SceLsdbLiveAreaPccInfo *pccInfo, SceUInt32 updateFlags);

/** Add or replace one sfType/sfTarg pair in a title's LiveArea record. */
int sceLsdbSetLiveAreaSfEntry(void *handle, const SceLsdbString *titleId, const SceLsdbLiveAreaSfEntry *entry);

/** Delete the sfType/sfTarg pair whose sfType equals \a type. */
int sceLsdbDeleteLiveAreaSfEntry(void *handle, const SceLsdbString *titleId, SceInt32 type);

/** Read tbl_livearea.modifiedDate for one title. */
int sceLsdbGetLiveAreaModifiedDate(void *handle, const SceLsdbString *titleId, SceRtcTick *lastModified);

/** Set tbl_livearea.modifiedDate for one title. */
int sceLsdbSetLiveAreaModifiedDate(void *unused, const SceLsdbString *titleId, const SceRtcTick *lastModified);

/** Rebuild one title's LiveArea rows from `template.xml` under \a contentsPath. */
int sceLsdbUpdateLiveAreaFromContents(const SceLsdbString *titleId, const char *contentsPath);

/** Delete the stored LiveArea record for one title. */
int sceLsdbDeleteLiveArea(const SceLsdbString *titleId);

/**
 * Read a stored LiveArea revision.
 *
 * When \a frameId is non-NULL, \a revision receives the matching frame
 * revision and \a originalPath is not written. Otherwise, \a revision receives
 * the content revision and \a originalPath receives tbl_livearea.org_path.
 */
int sceLsdbGetLiveAreaRevision(const SceLsdbString *titleId, const char *frameId, SceUInt64 *revision, SceLsdbString *originalPath);

/** Decrement the icon-layout open count and release the INI context at zero. */
int sceLsdbIconLayoutClose(void);

/** Open ux0:iconlayout.ini and increment the open count. */
int sceLsdbIconLayoutOpen(void);

/** Delete ux0:iconlayout.ini. A missing file is treated as success. */
int sceLsdbIconLayoutDeleteFile(void);

/** Return whether ux0:iconlayout.ini exists. */
int sceLsdbIconLayoutFileExists(void);

/** Enable or disable icon-layout writes. The supplied value is returned. */
SceBool sceLsdbIconLayoutSetWritesDisabled(SceBool disabled);

/** Read one icon-layout entry by title ID. */
SceBool sceLsdbIconLayoutGetEntry(const char *titleId, SceLsdbIconLayoutEntry *entry);

/** Write one icon-layout entry by title ID. */
int sceLsdbIconLayoutSetEntry(const char *titleId, const SceLsdbIconLayoutEntry *entry);

/** Delete one icon-layout entry. */
int sceLsdbIconLayoutDeleteEntry(const char *titleId);

/**
 * Update selected parent fields of an icon-layout entry.
 *
 * NULL page, position, or title pointers leave the corresponding field
 * unchanged.
 */
int sceLsdbIconLayoutUpdateParent(const char *titleId, const SceInt32 *parentPageNo, const SceInt32 *parentPosition, const char *parentTitleId);

/** Increment or decrement page numbers at or above \a pageNo. */
int sceLsdbIconLayoutShiftPages(SceBool increment, SceInt32 pageNo);

/**
 * Return whether an icon occupies \a pageNo and \a position.
 *
 * Normal page and position values are 0 through 9. FW 3.60 also recognizes
 * special page numbers from -20000000 through -10000001.
 */
int sceLsdbIconLayoutPositionExists(SceInt32 pageNo, SceInt32 position);

/** Count the distinct special negative page numbers in iconlayout.ini. */
int sceLsdbIconLayoutGetSpecialPageCount(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_LSDB_H_ */
