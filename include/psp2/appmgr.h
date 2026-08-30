/**
 * \usergroup{SceAppMgr}
 * \usage{psp2/appmgr.h,SceAppMgr_stub}
 */


#ifndef _PSP2_APPMGR_H_
#define _PSP2_APPMGR_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>
#include <psp2common/appmgr.h>
#include <psp2common/pfsmgr.h>
#include <psp2/apputil.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SceAppMgrErrorCode {
	SCE_APPMGR_ERROR_INVALID_PARAM_SFO  = 0x80800015,
	SCE_APPMGR_ERROR_BUSY               = 0x80802000, //!< Busy
	SCE_APPMGR_ERROR_INTERNAL           = 0x80802001, //!< Internal error
	SCE_APPMGR_ERROR_PARAM              = 0x80802002, //!< Invalid parameter
	SCE_APPMGR_ERROR_NOT_FOUND          = 0x80802012, //!< Application or process not found
	SCE_APPMGR_ERROR_STATE              = 0x80802013, //!< Invalid state
	SCE_APPMGR_ERROR_NULL_POINTER       = 0x80802016, //!< NULL pointer
	SCE_APPMGR_ERROR_INVALID            = 0x8080201A, //!< Invalid param
	SCE_APPMGR_ERROR_TOO_LONG_ARGV      = 0x8080201D, //!< argv is too long
	SCE_APPMGR_ERROR_INVALID_SELF_PATH  = 0x8080201E, //!< Invalid SELF path
	SCE_APPMGR_ERROR_NOEXEC             = 0x8080201F, //!< The process is not authorized to run this function
	SCE_APPMGR_ERROR_BGM_PORT_BUSY      = 0x80803000  //!< BGM port was occupied and could not be secured
} SceAppMgrErrorCode;

typedef enum SceAppMgrSystemEventType {
	SCE_APPMGR_SYSTEMEVENT_ON_ACTIVATE           = 0x10000001,
	SCE_APPMGR_SYSTEMEVENT_ON_DEACTIVATE         = 0x10000002,
	SCE_APPMGR_SYSTEMEVENT_ON_RESUME             = 0x10000003,
	SCE_APPMGR_SYSTEMEVENT_ON_STORE_PURCHASE     = 0x10000004,
	SCE_APPMGR_SYSTEMEVENT_ON_NP_MESSAGE_ARRIVED = 0x10000005,
	SCE_APPMGR_SYSTEMEVENT_ON_STORE_REDEMPTION   = 0x10000006,
	SCE_APPMGR_SYSTEMEVENT_REQUEST_QUIT          = 0x20000001
} SceAppMgrSystemEventType;

typedef enum SceAppMgrEventType {
	SCE_APP_EVENT_ON_ACTIVATE                    = 0x10000001,
	SCE_APP_EVENT_ON_DEACTIVATE                  = 0x10000002,
	SCE_APP_EVENT_ON_RESUME                      = 0x10000003,
	SCE_APP_EVENT_LIVEBOARD_MODE_0               = 0x10000101, //!< Queued by `sceAppMgrSendLiveBoardMode` with mode 0.
	SCE_APP_EVENT_LIVEBOARD_MODE_1               = 0x10000102, //!< Queued by `sceAppMgrSendLiveBoardMode` with mode 1.
	SCE_APP_EVENT_REQUEST_QUIT                   = 0x20000001,
	SCE_APP_EVENT_PARAM_NOTIFY                   = 0x20000004,
	SCE_APP_EVENT_RECOMMENDED_ORIENTATION_CHANGED = 0x20000010
} SceAppMgrEventType;

typedef enum SceAppMgrInfoBarVisibility {
	SCE_APPMGR_INFOBAR_VISIBILITY_INVISIBLE = 0,
	SCE_APPMGR_INFOBAR_VISIBILITY_VISIBLE   = 1
} SceAppMgrInfoBarVisibility;

typedef enum SceAppMgrInfoBarColor {
	SCE_APPMGR_INFOBAR_COLOR_BLACK  = 0,
	SCE_APPMGR_INFOBAR_COLOR_WHITE  = 1
} SceAppMgrInfoBarColor;

typedef enum SceAppMgrInfoBarTransparency {
	SCE_APPMGR_INFOBAR_TRANSPARENCY_OPAQUE      = 0,
	SCE_APPMGR_INFOBAR_TRANSPARENCY_TRANSLUCENT = 1
} SceAppMgrInfoBarTransparency;

typedef enum SceAppMgrApplicationMode {
	SCE_APPMGR_APPLICATION_MODE_A = 2, //!< Application without physically contiguous memory access
	SCE_APPMGR_APPLICATION_MODE_B = 3, //!< Application with physically contiguous memory access
	SCE_APPMGR_APPLICATION_MODE_C = 4  //!< Application with physically contiguous memory and extra memory access
} SceAppMgrApplicationMode;

typedef struct SceAppMgrSystemEvent {
	int     systemEvent;   //!< One of ::SceAppMgrSystemEventType
	uint8_t reserved[60];  //!< Reserved data
} SceAppMgrSystemEvent;
VITASDK_BUILD_ASSERT_EQ(0x40, SceAppMgrSystemEvent);

typedef struct SceAppMgrSaveDataData {
	int size;                                //!< Must be 0x4C
	unsigned int slotId;                     //!< Save slot to use
	SceAppUtilSaveDataSlotParam* slotParam;  //!< Save slot params
	uint8_t reserved[32];                    //!< Reserved data
	SceAppUtilSaveDataFile* files;           //!< Pointer to an array of files
	int fileNum;                             //!< Number of files to save
	SceAppUtilSaveDataMountPoint mountPoint; //!< Savedata mountpoint
	unsigned int* requiredSizeKB;            //!< Required size in KBs
	int unk_0x48;
} SceAppMgrSaveDataData;
VITASDK_BUILD_ASSERT_EQ(0x4C, SceAppMgrSaveDataData);

typedef struct SceAppMgrSaveDataDataDelete {
	int size;                                //!< Must be 0x44
	unsigned int slotId;                     //!< Save slot to use
	SceAppUtilSaveDataSlotParam* slotParam;  //!< Save slot params
	uint8_t reserved[32];                    //!< Reserved data
	SceAppUtilSaveDataFile* files;           //!< Pointer to an array of files
	int fileNum;                             //!< Number of files to delete
	SceAppUtilSaveDataMountPoint mountPoint; //!< Savedata mountpoint
} SceAppMgrSaveDataDataDelete;
VITASDK_BUILD_ASSERT_EQ(0x44, SceAppMgrSaveDataDataDelete);

typedef struct SceAppMgrSaveDataSlot {
	int size;                                //!< Must be 0x418
	unsigned int slotId;                     //!< Save slot to use
	SceAppUtilSaveDataSlotParam slotParam;   //!< Save slot params
	uint8_t reserved[116];                   //!< Reserved data
	SceAppUtilSaveDataMountPoint mountPoint; //!< Savedata mountpoint
	uint8_t reserved2[0x40];
} SceAppMgrSaveDataSlot;
VITASDK_BUILD_ASSERT_EQ(0x418, SceAppMgrSaveDataSlot);

typedef struct SceAppMgrSaveDataSlotDelete {
	int size;                                 //!< Must be 0x18
	unsigned int slotId;                      //!< Save slot to use
	SceAppUtilSaveDataMountPoint mountPoint;  //!< Savedata mountpoint
} SceAppMgrSaveDataSlotDelete;
VITASDK_BUILD_ASSERT_EQ(0x18, SceAppMgrSaveDataSlotDelete);

typedef struct SceAppMgrAppState {
	SceUInt32 systemEventNum;
	SceUInt32 appEventNum;
	SceBool isSystemUiOverlaid;
	SceUInt8 reserved[116];
} SceAppMgrAppState;
VITASDK_BUILD_ASSERT_EQ(0x80, SceAppMgrAppState);

typedef struct SceAppMgrBudgetInfo {
	int size;                           //!< Must be 0x88
	int app_mode;                       //!< One of ::SceAppMgrApplicationMode
	int unk0;                           //!< Unknown Data
	unsigned int total_user_rw_mem;     //!< Total amount of accessible USER_RW memory
	unsigned int free_user_rw;          //!< Free amount of accessible USER_RW memory
	SceBool extra_mem_allowed;          //!< Flag for extra memory accessibility
	int unk1;                           //!< Unknown Data
	unsigned int total_extra_mem;       //!< Total amount of accessible extra memory
	unsigned int free_extra_mem;        //!< Free amount of accessible extra memory
	int unk2[2];                        //!< Unknown Data
	unsigned int total_phycont_mem;     //!< Total amount of accessible physically contiguous memory
	unsigned int free_phycont_mem;      //!< Free amount of accessible physically contiguous memory
	int unk3[10];                       //!< Unknown Data
	unsigned int total_cdram_mem;       //!< Total amount of accessible CDRAM memory
	unsigned int free_cdram_mem;        //!< Free amount of accessible CDRAM memory
	int reserved[9];                    //!< Reserved data
} SceAppMgrBudgetInfo;
VITASDK_BUILD_ASSERT_EQ(0x88, SceAppMgrBudgetInfo);

typedef struct SceAppMgrExecOptParam SceAppMgrExecOptParam; // Missing struct
typedef struct SceAppMgrLaunchAppOptParam SceAppMgrLaunchAppOptParam; // Missing struct

typedef struct SceAppMgrLoadExecOptParam {
	int reserved[64];    //!< Reserved data
} SceAppMgrLoadExecOptParam;
VITASDK_BUILD_ASSERT_EQ(0x100, SceAppMgrLoadExecOptParam);

#define SCE_APPMGR_MAX_APP_NAME_LENGTH	(31)

/**
 * Save data on savedata0: partition
 *
 * @param[in] data - Data to save
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataDataSave(SceAppMgrSaveDataData *data);

/**
 * Remove data on savedata0: partition
 *
 * @param[in] data - Data to remove
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataDataRemove(SceAppMgrSaveDataDataDelete *data);

/**
 * Create a savedata slot
 *
 * @param[in] data - Slot data
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataSlotCreate(SceAppMgrSaveDataSlot *data);

/**
 * Get current param of a savedata slot
 *
 * @param[out] data - Slot data
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataSlotGetParam(SceAppMgrSaveDataSlot *data);

/**
 * Set current param of a savedata slot
 *
 * @param[in] data - Slot data
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataSlotSetParam(SceAppMgrSaveDataSlot *data);

/**
 * Delete a savedata slot
 *
 * @param[in] data - Slot data
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrSaveDataSlotDelete(SceAppMgrSaveDataSlotDelete *data);

/**
 * Get Process ID by Title ID
 *
 * @param[out] pid - Process ID
 * @param[in] name - Title ID
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrGetIdByName(SceUID *pid, const char *name);

/**
 * Get Title ID by Process ID
 *
 * @param[in] pid - Process ID
 * @param[out] name - Title ID
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrGetNameById(SceUID pid, char *name);

/**
 * Destroy other apps
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrDestroyOtherApp(void);

/**
 * Destroy an application by Title ID
 *
 * @param[in] name - Title ID of the application
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrDestroyAppByName(const char *name);

/**
 * Destroy an application by Application ID
 *
 * @param[in] appId - Application ID of the application
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrDestroyAppByAppId(SceInt32 appId);

/**
 * Get PID of an application for Shell
 *
 * @param[in] appId - Application ID of the application
 *
 * @return The PID on success, < 0 on error.
 */
SceUID sceAppMgrGetProcessIdByAppIdForShell(SceInt32 appId);

/**
 * Get a list of running applications
 *
 * @param[out] appIds - Array of running application IDs
 * @param[in] count - Max number of running applications to search
 *
 * @return Number of running applications.
 */
int sceAppMgrGetRunningAppIdListForShell(SceInt32 *appIds, int count);

/**
 * Get an application state
 *
 * @param[out] appState - State of the application
 * @param[in] len - sizeof(SceAppMgrState)
 * @param[in] version - Version (?)

 * @return 0 on success, < 0 on error.
 */
int _sceAppMgrGetAppState(SceAppMgrAppState *appState, SceSize len, uint32_t version);

/**
 * Receive system event
 *
 * @param[out] systemEvent - Received system event

 * @return 0 on success, < 0 on error.
 */
int sceAppMgrReceiveSystemEvent(SceAppMgrSystemEvent *systemEvent);

/**
 * Copies app param to an array
 *
 * @param[out] param - pointer to a 1024 byte location to store the app param
 *
 * @return 0 on success, < 0 on error.
 *
 * @note App param example: type=LAUNCH_APP_BY_URI&uri=psgm:play?titleid=NPXS10031
 */
int sceAppMgrGetAppParam(char *param);

/**
 * Obtains the BGM port, even when it is not in front
 *
 * @return 0 on success, < 0 on error.
 *
 */
int sceAppMgrAcquireBgmPort(void);

/**
 * Release acquired BGM port
 *
 * @return 0 on success, < 0 on error.
 *
 */
int sceAppMgrReleaseBgmPort(void);

/**
 * Set infobar state
 *
 * @param[in] visibility - Infobar visibility
 * @param[in] color - Infobar color
 * @param[in] transparency - Infobar transparency
 *
 * @return 0 on success, < 0 on error.
 *
 */
int sceAppMgrSetInfobarState(SceAppMgrInfoBarVisibility visibility, SceAppMgrInfoBarColor color, SceAppMgrInfoBarTransparency transparency);

/**
 * Load and start a SELF executable
 *
 * @param[in] appPath - Path of the SELF file
 * @param[in] argv - Args to pass to SELF module_start
 * @param[in] optParam - Optional params
 *
 * @return 0 on success, < 0 on error.
 *
 * @note SELF file must be located in app0: partition.
 */
int sceAppMgrLoadExec(const char *appPath, char * const argv[],
	const SceAppMgrExecOptParam *optParam);

/**
 * Start an application by URI
 *
 * @param[in] flags - Must be 0x20000
 * @param[in] uri - Uri to launch
 *
 * @return 0 on success, < 0 on error.
 *
 * @note If flags != 0x20000, Livearea is opened.
 */
int sceAppMgrLaunchAppByUri(int flags, const char *uri);

/**
 * Start an application by Title ID
 *
 * @param[in] name - Title ID of the application
 * @param[in] param - The params passed to the application which can be retrieved with ::sceAppMgrGetAppParam
 * @param[in] optParam - Optional params
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrLaunchAppByName2(const char *name, const char *param, SceAppMgrLaunchAppOptParam *optParam);

/**
* Start an Application by Title ID
*
* @param[in] flags - Usually 0x60000
* @param[in] name - Title ID of the application
* @param[in] param - The params passed to the application which can be retrieved with ::sceAppMgrGetAppParam
*
* @return 0 on success < 0 on error.
*/
int sceAppMgrLaunchAppByName(int flags, const char* name, const char* param);
	
/**
 * Start an application by Title ID for Shell
 *
 * @param[in] name - Title ID of the application
 * @param[in] param - The params passed to the application which can be retrieved with ::sceAppMgrGetAppParam
 * @param[in] optParam - Optional params
 *
 * @return Application ID (?)
 */
SceUID sceAppMgrLaunchAppByName2ForShell(const char *name, const char *param, SceAppMgrLaunchAppOptParam *optParam);

/**
 * Mount game data
 *
 * @param[in] app_path    - example : "ux0:/app/${TITLEID}"
 * @param[in] patch_path  - example : "ux0:/patch/${TITLEID}", "invalid:"
 * @param[in] rif_path    - If NULL the system will automatically search the rif path. example : "ux0:/license/app/${TITLEID}/${HEX}.rif"
 * @param[in] mount_point - Mountpoint output
 *
 * @return 0 on success, < 0 on error.
 *
 */
int sceAppMgrGameDataMount(const char *app_path, const char *patch_path, const char *rif_path, char *mount_point);

/**
 * Mount application data
 *
 * @param[in] id - App data ID
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note id: 100 (photo0), 101 (friends), 102 (messages), 103 (near), 105 (music), 108 (calendar)
 */
int sceAppMgrAppDataMount(int id, char mount_point[16]);

/**
 * Mount application data by Title ID
 *
 * @param[in] id - App data ID
 * @param[in] titleid - Application title ID
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note id: 106 (ad), 107 (ad)
 */
int sceAppMgrAppDataMountById(int id, const char *titleid, char mount_point[16]);

/**
 * Get application params from SFO descriptor
 *
 * @param[in] pid - Process ID
 * @param[in] param - Param ID in the SFO descriptor
 * @param[out] string - Param data
 * @param[in] length - Length of the param data
 *
 * @return 0 on success, < 0 on error.
 *
 * @note param: 6 (contentid) 8 (category), 9 (stitle/title?), 10 (title/stitle?), 12 (titleid)
 */
int sceAppMgrAppParamGetString(int pid, int param, char *string, SceSize length);

/**
 * Get device info
 *
 * @param[in] dev - Device to get info about
 * @param[out] max_size - Capacity of the device
 * @param[out] free_size - Free space of the device
 *
 * @return 0 on success, < 0 on error.
 *
 * @note dev: ux0:
 */
int sceAppMgrGetDevInfo(const char *dev, uint64_t *max_size, uint64_t *free_size);

/**
 * Mount application data (PSPEmu)
 *
 * @param[in] id - App data ID
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note id: 400 (ad), 401 (ad), 402 (ad)
 */
int sceAppMgrMmsMount(int id, char mount_point[16]);

/**
 * Mount PSPEmu virtual memory stick
 *
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note mount_point: ms
 */
int sceAppMgrPspSaveDataRootMount(char mount_point[16]);

/**
 * Mount working directory
 *
 * @param[in] id - Working directory ID
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note id: 200 (td), 201 (td), 203 (td), 204 (td), 206 (td)
 */
int sceAppMgrWorkDirMount(int id, char mount_point[16]);

/**
 * Mount working directory by Title ID
 *
 * @param[in] id - Working directory ID
 * @param[in] titleid - Application Title ID
 * @param[out] mount_point - Receives the 16-byte mountpoint string.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note id: 205 (cache0), 207 (td)
 */
int sceAppMgrWorkDirMountById(int id, const char *titleid, char mount_point[16]);

/**
 * Unmount a mountpoint
 *
 * @param[in] mount_point - Mountpoint to unmount
 *
 * @return 0 on success, < 0 on error.
 *
 * @note Unmount app0: for example to enable write access to ux0:app/TITLEID
 */
int sceAppMgrUmount(const char *mount_point);

/**
 * Convert vs0 path string to a new one usable by applications
 *
 * @param[in] path - Path to convert
 * @param[in] mount_point - Mountpoint to use
 * @param[in] unk - Unknown
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrConvertVs0UserDrivePath(char *path, char *mount_point, int unk);

/**
 * Get raw path for a given path
 *
 * @param[out] path               - Path to convert
 * @param[in]  resolved_path      - The input process path
 * @param[in]  resolved_path_size - The input process path length
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrGetRawPath(char *path, char *resolved_path, int resolved_path_size);

/**
 * Get the real/resolved path of app0: (where it's actually mounted)
 *
 * @param[in] appId - Use -2 for the current application
 * @param[out] resolved_path - Buffer that will hold the resolved path. It should have enough room to hold 292 characters or it will buffer overflow (noname120).
 *
 * @return 0 on success.
 */
int sceAppMgrGetRawPathOfApp0ByAppIdForShell(int appId, char resolved_path[292]);

/**
 * Get memory budget info for a running system application
 *
 * @param[out] info - Info related to the memory budget of the running application.
 *
 * @return 0 on success, < 0 on error.
 *
 * @note This function will always return an error if used in a normal application.
 */
int sceAppMgrGetBudgetInfo(SceAppMgrBudgetInfo *info);

typedef struct SceAppMgrCoredumpState {
	SceUID pid;
	int process_state;
	int progress; // 0-100
	int is_coredump_completed;
	int data_0x10;
	SceSize path_len;
	char path[0x400];
	int data_0x418;
	int data_0x41C;
	int data_0x420;
	int data_0x424;
} SceAppMgrCoredumpState;
VITASDK_BUILD_ASSERT_EQ(0x428, SceAppMgrCoredumpState);

/**
 * Get current coredump state for shell
 *
 * @param[out] state - state info output.
 *
 * @return 0 on success, < 0 on error.
 */
int sceAppMgrGetCoredumpStateForShell(SceAppMgrCoredumpState *state);

typedef struct SceAppMgrDrmOpenParam SceAppMgrDrmOpenParam;
typedef struct SceAppMgrAppInfo {
	SceBool isRunning; //!< Non-zero while the selected process is in AppMgr's started lifecycle state.
	SceUInt8 reserved[0x7C]; //!< Set to 0 on FW 3.60.
} SceAppMgrAppInfo;
VITASDK_BUILD_ASSERT_EQ(0x80, SceAppMgrAppInfo); // size is from FW 3.60

SceInt32 sceAppMgrDrmOpen(const SceAppMgrDrmOpenParam *param);

/**
 * Get the started lifecycle state of an application.
 *
 * The calling process must be a non-game program. FW 3.60 clears the complete
 * 0x80-byte output and writes only ::SceAppMgrAppInfo::isRunning.
 *
 * @param[in] appName Application name of at most 31 characters; NULL selects
 *                    the calling process.
 * @param[out] appInfo Output buffer whose FW 3.60 layout is
 *                     ::SceAppMgrAppInfo. Still declared as
 *                     ::SceAppMgrAppState * for backwards compatibility.
 *
 * @return 0 on success, ::SCE_APPMGR_ERROR_NOT_FOUND when no matching process
 *         context exists, or another negative error code.
 */
SceInt32 sceAppMgrGetAppInfo(const char *appName, SceAppMgrAppState *appInfo);

typedef struct SceAppMgrAppMgrState {
	SceUInt32 prioritizeSystemChat;
	SceUInt32 systemImposeState;
	SceInt32 reserved08; //!< Initialized to -1 on FW 3.60.
	SceUInt32 audioRoutingState;
	SceUInt32 recommendedScreenOrientation; //!< A value from 1 through 4.
	SceUInt32 systemImposeState2;
	SceUInt8 reserved[0x68]; //!< Reserved in the FW 3.60 state block.
} SceAppMgrAppMgrState;
VITASDK_BUILD_ASSERT_EQ(0x80, SceAppMgrAppMgrState); // size is from FW 3.60

typedef struct SceAppMgrAppParam {
	char data[0x400];
} SceAppMgrAppParam;
VITASDK_BUILD_ASSERT_EQ(0x400, SceAppMgrAppParam); // size is from FW 0.990

typedef enum SceAppMgrApplicationType {
	SCE_APPMGR_APPLICATION_TYPE_MINI_APPLICATION   = 0,
	SCE_APPMGR_APPLICATION_TYPE_GAME               = 1,
	SCE_APPMGR_APPLICATION_TYPE_SYSTEM_APPLICATION = 2
} SceAppMgrApplicationType;

/** String fields accepted by ::_sceAppMgrAppParamGetString. */
typedef enum SceAppMgrAppParamId {
	SCE_APPMGR_APP_PARAM_CONTENT_ID               = 6,   //!< Content ID; requires at least 0x30 output bytes.
	SCE_APPMGR_APP_PARAM_NP_COMMUNICATION_ID      = 7,   //!< NP communication ID; requires at least 0xD output bytes.
	SCE_APPMGR_APP_PARAM_CATEGORY                 = 8,   //!< Application category; requires at least 4 output bytes.
	SCE_APPMGR_APP_PARAM_TITLE                    = 9,   //!< Localized title; requires at least 0x80 output bytes.
	SCE_APPMGR_APP_PARAM_SHORT_TITLE              = 10,  //!< Localized short title; requires at least 0x34 output bytes.
	SCE_APPMGR_APP_PARAM_TITLE_ID                 = 12,  //!< Title ID; requires at least 10 output bytes.
	SCE_APPMGR_APP_PARAM_APP_PATH                 = 14,  //!< Executable path; requires system permission and at least 0x124 output bytes.
	SCE_APPMGR_APP_PARAM_CONTENT_ID_OVERRIDE      = 100, //!< Process-local override, falling back to ::SCE_APPMGR_APP_PARAM_CONTENT_ID.
	SCE_APPMGR_APP_PARAM_TITLE_OVERRIDE           = 101  //!< Process-local override, falling back to ::SCE_APPMGR_APP_PARAM_TITLE.
} SceAppMgrAppParamId;

/**
 * Launch-request flags observed on FW 3.60.
 */
typedef enum SceAppMgrLaunchFlag {
	SCE_APPMGR_LAUNCH_FLAG_LIVEAREA_OFF       = 0x00010000, //!< Launch without activating LiveArea.
	SCE_APPMGR_LAUNCH_FLAG_DEBUG_CALLBACK     = 0x00020000, //!< Set up AppMgr's debug semaphore and callback.
	SCE_APPMGR_LAUNCH_FLAG_EXIT_TO            = 0x00040000, //!< Enable exit-to behavior.
	SCE_APPMGR_LAUNCH_FLAG_ALLOW_EMPTY_TARGET = 0x00100000  //!< Permit an empty launch target.
} SceAppMgrLaunchFlag;

/** Values written to ::SceAppMgrAppStatus::processLaunchType on FW 3.60. */
typedef enum SceAppMgrProcessLaunchType {
	SCE_APPMGR_PROCESS_LAUNCH_TYPE_NORMAL      = 0, //!< Normal title or URI launch.
	SCE_APPMGR_PROCESS_LAUNCH_TYPE_PATH4       = 1, //!< Launch through `sceAppMgrLaunchAppByPath4`; represented as `NPXS10998` by SceShell.
	SCE_APPMGR_PROCESS_LAUNCH_TYPE_KERNEL_PATH = 2  //!< Launch through ::ksceAppMgrLaunchAppByPath; represented as `NPXS10999` by SceShell.
} SceAppMgrProcessLaunchType;

/**
 * Application status returned by the raw status exports.
 *
 * FW 3.60 always writes the complete 0x80-byte structure; the output is not
 * size-prefixed.
 */
typedef struct SceAppMgrAppStatus {
	ScePID parentProcessId; //!< Process that submitted the launch request, or -1.
	SceUInt32 launchFlags; //!< Bitwise OR of ::SceAppMgrLaunchFlag values.
	SceUInt32 appProtectionModeOnMemoryShortage;
	char appName[32];
	SceUInt32 hasLaunchParam; //!< Non-zero when launch parameters are available.
	SceUID appId;
	ScePID processId;
	SceBool isActive; //!< Non-zero while AppMgr considers the application activated.
	SceBool isRunning; //!< Non-zero after AppMgr starts the process and before it enters shutdown.
	SceBool networkDisconnectionWarningDialogEnabled;
	SceUID parentAppId;
	SceUInt8 infoBarVisibility; //!< One of ::SceAppMgrInfoBarVisibility.
	SceUInt8 infoBarColor; //!< One of ::SceAppMgrInfoBarColor.
	SceUInt8 infoBarTransparency; //!< One of ::SceAppMgrInfoBarTransparency.
	SceUInt8 reserved4B; //!< High byte of the packed info-bar state.
	SceUInt32 reserved4C; //!< Internal launch-request value; its purpose is unknown.
	char bgmProxyAppName[32];
	SceUInt8 processLaunchType; //!< One of ::SceAppMgrProcessLaunchType.
	SceUInt8 applicationType; //!< One of ::SceAppMgrApplicationType.
	SceUInt8 reserved72[2]; //!< Set to 0 on FW 3.60.
	SceUInt32 appFlags; //!< Internal process flags copied from AppMgr's process context.
	SceUInt32 recommendedScreenOrientation; //!< A value from 1 through 4.
	SceBool recommendedScreenOrientationActivated;
} SceAppMgrAppStatus;
VITASDK_BUILD_ASSERT_EQ(0x80, SceAppMgrAppStatus); // size is from FW 3.60

/**
 * Size-prefixed application boot parameters.
 *
 * FW 3.60 accepts and returns any prefix up to 0x2C bytes. The former
 * 0xC-byte layout therefore remains supported when \c size is 0xC.
 */
typedef struct SceAppMgrBootParam {
	SceSize size; //!< Number of bytes requested from the public wrapper; maximum value is 0x2C.
	SceUInt reserved04[2]; //!< Set to 0 on FW 3.60.
	SceUInt reserved0C[8]; //!< Preserved from the caller on FW 3.60.
} SceAppMgrBootParam;
VITASDK_BUILD_ASSERT_EQ(0x2C, SceAppMgrBootParam); // size is from FW 3.60

#define SCE_APP_MGR_MAX_EVENT_PARM_LENGTH (56)

typedef struct SceAppMgrEvent {
	int event; //!< One of ::SceAppMgrEventType or ::SceAppMgrSystemEventType.
	SceUID appId; //!< Application ID; FW 3.60 writes -1 for received events.
	char param[SCE_APP_MGR_MAX_EVENT_PARM_LENGTH]; //!< Event parameters; set to 0 on FW 3.60.
} SceAppMgrEvent;
VITASDK_BUILD_ASSERT_EQ(0x40, SceAppMgrEvent); // size is from FW 3.60

typedef struct SceAppMgrLaunchAppByUri2Param {
	SceSize size; //!< Ignored on FW 3.60.
	SceUInt32 flags; //!< Bitwise OR of ::SceAppMgrLaunchFlag values; FW 3.60 forces
	                 //!< ::SCE_APPMGR_LAUNCH_FLAG_ALLOW_EMPTY_TARGET.
	SceUInt32 reserved08; //!< Ignored on FW 3.60.
	SceUID appLaunchCallbackId; //!< Callback notified with the launched process ID, or -1.
	SceUID getParamCallbackId; //!< Callback notified when the target consumes its launch parameters, or -1.
	SceInt32 parentAppId; //!< Parent application ID, or -1;
	                       //!< used when \a flags includes ::SCE_APPMGR_LAUNCH_FLAG_EXIT_TO.
	SceUInt32 launchMode; //!< Application mode from 0 through 4; values 2 through 4 are ::SceAppMgrApplicationMode values.
	SceUInt32 reserved1C; //!< Stored in the target process context on FW 3.60; its purpose is unknown.
	SceUInt8 reserved[0x20]; //!< Must be zero on FW 3.60.
} SceAppMgrLaunchAppByUri2Param;
VITASDK_BUILD_ASSERT_EQ(0x40, SceAppMgrLaunchAppByUri2Param); // size is from FW 3.60

typedef struct SceAppMgrNetworkDisconnectionWarningDialogStateOptParam {
	SceUInt8 reserved[0x80]; //!< Not accepted by the FW 3.60 provider; pass NULL.
} SceAppMgrNetworkDisconnectionWarningDialogStateOptParam;
VITASDK_BUILD_ASSERT_EQ(0x80, SceAppMgrNetworkDisconnectionWarningDialogStateOptParam); // size is from FW 3.60

typedef struct SceAppMgrCheckRifGDOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrCheckRifGDOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrCheckRifGDOpt); // size is from FW 3.60

/** Game-data RIF information returned by ::_sceAppMgrCheckRifGD. */
typedef struct SceAppMgrCheckRifGDResult {
	char contentId[0x30]; //!< RIF content ID.
	SceUInt32 licenseVersion; //!< RIF license version.
	SceUInt32 drmType; //!< RIF DRM type.
	SceUInt32 flags; //!< Derived license flags returned by ::ksceNpDrmGetRifInfo.
	SceUInt32 skuFlags; //!< RIF SKU flags.
	SceUInt64 accountId; //!< License account ID.
	SceUInt64 rifData98; //!< Raw 8-byte RIF field.
	SceInt64 licenseStartTime; //!< Effective license start time.
	SceInt64 licenseExpirationTime; //!< Effective license expiration time.
	SceUInt8 key[0x10]; //!< Decrypted Vita license key.
} SceAppMgrCheckRifGDResult;
VITASDK_BUILD_ASSERT_EQ(0x70, SceAppMgrCheckRifGDResult); // size is from FW 3.60

typedef struct SceAppMgrGameDataMountOpt {
	char *mountPoint; //!< Receives a 16-byte mount point of the form "gpXXXXXXXXXXXd:".
	SceUInt32 reserved[5]; //!< Ignored on FW 3.60.
} SceAppMgrGameDataMountOpt;
VITASDK_BUILD_ASSERT_EQ(0x18, SceAppMgrGameDataMountOpt); // size is from FW 3.60

typedef struct SceAppMgrGetAppInfoOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrGetAppInfoOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetAppInfoOpt); // size is from FW 3.60

typedef struct SceAppMgrGetAppParamOpt {
	SceSize appParamSize; //!< FW 0.990 application-parameter size; ignored on FW 3.60.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceAppMgrGetAppParamOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetAppParamOpt); // size is from FW 0.990

typedef struct SceAppMgrGetBootParamOpt {
	SceSize bootParamSize; //!< Number of leading bytes to copy to and from the boot-parameter buffer.
	SceUInt reserved; //!< Ignored on FW 3.60.
} SceAppMgrGetBootParamOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetBootParamOpt); // size is from FW 0.990

typedef struct SceAppMgrGetIdByNameOpt {
	SceSize appNameSize; //!< FW 0.990 application-name size; ignored on FW 3.60.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceAppMgrGetIdByNameOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetIdByNameOpt); // size is from FW 0.990

typedef struct SceAppMgrGetNameByIdOpt {
	SceSize appNameSize; //!< FW 0.990 application-name size; ignored on FW 3.60.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceAppMgrGetNameByIdOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetNameByIdOpt); // size is from FW 0.990

typedef struct SceAppMgrGetSaveDataInfoForSpecialExportOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrGetSaveDataInfoForSpecialExportOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetSaveDataInfoForSpecialExportOpt); // size is from FW 3.60

typedef enum SceAppMgrSaveDataStorageType {
	SCE_APPMGR_SAVE_DATA_STORAGE_OTHER       = 1,
	SCE_APPMGR_SAVE_DATA_STORAGE_SYSTEM      = 2,
	SCE_APPMGR_SAVE_DATA_STORAGE_GAME_CARD   = 3,
	SCE_APPMGR_SAVE_DATA_STORAGE_USER        = 4
} SceAppMgrSaveDataStorageType;

typedef enum SceAppMgrUserDirPartition {
	SCE_APPMGR_USER_DIR_PARTITION_UR0 = 1,
	SCE_APPMGR_USER_DIR_PARTITION_UX0 = 2
} SceAppMgrUserDirPartition;

typedef struct SceAppMgrGetSaveDataInfoResult {
	SceUInt32 storageType; //!< One of ::SceAppMgrSaveDataStorageType.
	SceBool isMounted; //!< Non-zero when the save data is mounted.
	SceBool hasBrokenData; //!< Set for specific PFS validation failures or when sdslot.dat is absent.
	SceUInt32 accountId[2]; //!< Low and high words of the param.sfo ACCOUNT_ID value.
	SceUInt32 saveDataParam44; //!< Value copied from the `param.sfo` `PARAMS` entry.
	SceUInt32 allocatedSizeKB;
} SceAppMgrGetSaveDataInfoResult;
VITASDK_BUILD_ASSERT_EQ(0x1C, SceAppMgrGetSaveDataInfoResult); // size is from FW 3.60

typedef struct SceAppMgrGetSaveDataInfoOpt {
	SceAppMgrGetSaveDataInfoResult *result; //!< Receives a 0x1C-byte result.
	SceUInt32 reserved[3]; //!< Ignored on FW 3.60.
} SceAppMgrGetSaveDataInfoOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceAppMgrGetSaveDataInfoOpt); // size is from FW 3.60

//! FW 0.990-only, this option structure is not accepted on FW 3.60.
typedef struct SceAppMgrGetStatusByIdOpt {
	SceSize appStatusSize;
	SceUInt32 reserved1;
} SceAppMgrGetStatusByIdOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetStatusByIdOpt); // size is from FW 0.990

typedef struct SceAppMgrGetUserDirPathOpt {
	SceSize pathMaxLength; //!< FW 0.990: must be at most 1024. Ignored on FW 3.60.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceAppMgrGetUserDirPathOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrGetUserDirPathOpt); // size is from FW 0.990

typedef struct SceAppMgrGenericMountContext {
	SceTitleId titleId; //!< Title or additional-content identifier used to construct the source path.
	char passCode[32]; //!< Passcode used to verify the mounted PFS keystone.
	char mountDrive[16]; //!< Optional explicit source drive or randomized mount-drive identifier.
} SceAppMgrGenericMountContext;
VITASDK_BUILD_ASSERT_EQ(0x40, SceAppMgrGenericMountContext); // size is from FW 3.60

typedef struct SceAppMgrAppDataMountByIdOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrAppDataMountByIdOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrAppDataMountByIdOpt); // size is from FW 3.60

typedef struct SceAppMgrAppParamGetStringOpt {
	SceSize stringSize; //!< Output-buffer capacity; the maximum value is 0x400.
	SceUInt32 reserved[3]; //!< Ignored on FW 3.60.
} SceAppMgrAppParamGetStringOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceAppMgrAppParamGetStringOpt); // size is from FW 3.60

typedef struct SceAppMgrGetStatusByNameOpt {
	SceSize appNameSize; //!< FW 0.990 application-name size; ignored on FW 3.60.
	SceSize appStatusSize; //!< FW 0.990 application-status size; ignored on FW 3.60.
	SceUInt32 reserved[2]; //!< FW 0.990 reserved words; not read on FW 3.60.
} SceAppMgrGetStatusByNameOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceAppMgrGetStatusByNameOpt); // size is from FW 0.990

typedef struct SceAppMgrLoadSaveDataSystemFileOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrLoadSaveDataSystemFileOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrLoadSaveDataSystemFileOpt); // size is from FW 3.60

typedef struct SceAppMgrLoopBackMountOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrLoopBackMountOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrLoopBackMountOpt); // size is from FW 3.60

typedef struct SceAppMgrPhotoMountOpt {
	SceSize paramSize; //!< Number of bytes to copy from ::SceAppMgrPhotoMountParam; maximum value is 0x84.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceAppMgrPhotoMountOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrPhotoMountOpt); // size is from FW 3.60

typedef struct SceAppMgrThemeDataMountOpt {
	char *mountPoint; //!< Receives a 16-byte mount point of the form "tmXXXXXXXXXXXX:".
	SceUInt32 reserved[5]; //!< Ignored on FW 3.60.
} SceAppMgrThemeDataMountOpt;
VITASDK_BUILD_ASSERT_EQ(0x18, SceAppMgrThemeDataMountOpt); // size is from FW 3.60

typedef struct SceAppMgrTrophyMountByIdOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrTrophyMountByIdOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrTrophyMountByIdOpt); // size is from FW 3.60

typedef struct SceAppMgrUpdateSaveDataParamOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrUpdateSaveDataParamOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrUpdateSaveDataParamOpt); // size is from FW 3.60

typedef struct SceAppMgrWorkDirMountByIdOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrWorkDirMountByIdOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrWorkDirMountByIdOpt); // size is from FW 3.60

typedef struct SceAppMgrConvertVs0UserDrivePathOpt {
	SceUInt32 reserved[4]; //!< Ignored on FW 3.60.
} SceAppMgrConvertVs0UserDrivePathOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceAppMgrConvertVs0UserDrivePathOpt); // size is from FW 3.60

typedef struct SceAppMgrDeclareShellProcess2Opt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrDeclareShellProcess2Opt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrDeclareShellProcess2Opt); // size is from FW 3.60

typedef struct SceAppMgrGetRawPathOpt {
	SceUInt32 reserved[4]; //!< Ignored on FW 3.60.
} SceAppMgrGetRawPathOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceAppMgrGetRawPathOpt); // size is from FW 3.60

typedef struct SceAppMgrLaunchAppByUri2Opt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrLaunchAppByUri2Opt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrLaunchAppByUri2Opt); // size is from FW 3.60

typedef SceAppMgrLaunchAppByUri2Param SceAppMgrLaunchVideoStreamingAppOpt;
VITASDK_BUILD_ASSERT_EQ(0x40, SceAppMgrLaunchVideoStreamingAppOpt); // size is from FW 3.60

typedef struct SceAppMgrLoadExecOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceAppMgrLoadExecOpt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceAppMgrLoadExecOpt); // size is from FW 3.60

/**
 * Get the calling application's pending-event and overlay state.
 *
 * @param[out] appState Receives the 0x80-byte application state.
 * @param[in] sizeofSceAppMgrAppState Must be the size of ::SceAppMgrAppState.
 * @param[in] buildVersion Maximum supported build version; FW 3.60 accepts
 *                         values through 0x03600011.
 *
 * @return 0 on success, or a negative error code.
 */
int __sceAppMgrGetAppState(SceAppMgrAppState *appState, SceUInt32 sizeofSceAppMgrAppState, SceUInt32 buildVersion);

/** Mount additional content after validating its PFS keystone and passcode. */
int _sceAppMgrAddContAddMount(const SceAppMgrGenericMountContext *mountContext, int forceMountUx);

/** Mount additional content for \a processId. */
int _sceAppMgrAddContMount(ScePID processId, const SceAppMgrGenericMountContext *mountContext);

/**
 * Mount application data and return a randomized mount point.
 *
 * FW 3.60 accepts mount IDs 100, 101, 102, 103, 105, 108, 109, 111, and 112.
 */
int _sceAppMgrAppDataMount(int mountId, char mountPoint[16]);

/**
 * Mount application data identified by a title or content ID.
 *
 * FW 3.60 accepts mount IDs 104, 106, 107, and 110. Mount ID 104 requires an
 * identifier with the exact form `ABCD12345_67`; IDs 106 and 107 select the
 * `RO` and `RW` directories below `ux0:psm/<titleId>`.
 */
int _sceAppMgrAppDataMountById(int mountId, const char *titleId, char mountPoint[16], const SceAppMgrAppDataMountByIdOpt *opt);

/**
 * Read a string parameter from a process's `param.sfo` metadata.
 *
 * The public wrapper stores its output-buffer length in \a opt. FW 3.60 caps
 * that length at 0x400 and validates the minimum required by \a paramId.
 * A zero \a processId selects the calling process. Game and non-game callers
 * cannot use a nonzero ID to inspect another process. Reading
 * ::SCE_APPMGR_APP_PARAM_APP_PATH additionally requires system permission.
 */
int _sceAppMgrAppParamGetString(ScePID processId, SceAppMgrAppParamId paramId, char *string, const SceAppMgrAppParamGetStringOpt *opt);

/** Validate a game-data RIF and return its parsed license information. */
int _sceAppMgrCheckRifGD(const char *path, SceAppMgrCheckRifGDResult *result, const SceAppMgrCheckRifGDOpt *opt);

/** Unmount the calling process's content mounts for an installation period. */
int _sceAppMgrContentInstallPeriodStart(void);

/** End an installation period and remount the calling process's content. */
int _sceAppMgrContentInstallPeriodStop(void);

/**
 * @param[out] convertedPath Output buffer for a path of at most 0x124 bytes,
 *                           including the terminating NUL.
 * @param[in] convertedPathSize Output-buffer capacity. FW 3.60 requires a
 *                              value greater than 0x3F and caps it at 0x124.
 */
int _sceAppMgrConvertVs0UserDrivePath(const char *path, char *convertedPath, SceSize convertedPathSize, const SceAppMgrConvertVs0UserDrivePathOpt *opt);

/**
 * @param[in] opt Required option block; ignored on FW 3.60.
 */
int _sceAppMgrDeclareShellProcess2(const char *appName, const SceAppMgrDeclareShellProcess2Opt *opt);

/**
 * Force-unmount active USB mass-storage image mounts.
 *
 * On FW 3.60 \a mountId must be 600 and the caller must have UDCD permission.
 */
int _sceAppMgrForceUmount(int mountId);
int _sceAppMgrGameDataMount(const char *appPath, const char *patchPath, const char *rifPath, const SceAppMgrGameDataMountOpt *opt);

/**
 * @param[in] appName Application name of at most 31 characters; NULL selects
 *                    the calling process.
 */
int _sceAppMgrGetAppInfo(const char *appName, SceAppMgrAppInfo *appInfo, const SceAppMgrGetAppInfoOpt *opt);

/** Get the 0x80-byte global AppMgr state. */
int _sceAppMgrGetAppMgrState(SceAppMgrAppMgrState *appMgrState);

/** Get the calling application's queued launch parameter. */
int _sceAppMgrGetAppParam(SceAppMgrAppParam *appParam, const SceAppMgrGetAppParamOpt *opt);

/**
 * Get a size-prefixed boot-parameter record.
 *
 * FW 3.60 ignores \a selector. The number of bytes copied is
 * ::SceAppMgrGetBootParamOpt::bootParamSize; values above 0x2C are rejected.
 */
int _sceAppMgrGetBootParam(SceUInt32 selector, SceAppMgrBootParam *bootParam, const SceAppMgrGetBootParamOpt *opt);

/**
 * @param[out] processId A pointer to a value of type ::ScePID.
 * @param[out] bgmState A pointer to a value of type ::SceUInt32.
 */
int _sceAppMgrGetCurrentBgmState(ScePID *processId, SceUInt32 *bgmState);

/** Resolve an application name to a process ID. */
int _sceAppMgrGetIdByName(ScePID *processId, const char *appName, const SceAppMgrGetIdByNameOpt *opt);

/** Resolve a process ID to its application name. */
int _sceAppMgrGetNameById(ScePID processId, char appName[32], const SceAppMgrGetNameByIdOpt *opt);

/**
 * @param[out] resolvedPath Output buffer for a path of at most 0x124 bytes,
 *                          including the terminating NUL.
 * @param[in] resolvedPathSize Output-buffer capacity passed to the FIOS
 *                              overlay resolver, capped at 0x124 on FW 3.60.
 * @param[in] opt Required option block; ignored on FW 3.60.
 */
int _sceAppMgrGetRawPath(const char *path, char *resolvedPath, SceSize resolvedPathSize, const SceAppMgrGetRawPathOpt *opt);
int _sceAppMgrGetRawPathOfApp0ByAppIdForShell(int appId, char resolvedPath[292]);

/**
 * Get savedata information for a title.
 *
 * @param[in] path Savedata path to inspect.
 * @param[in] titleId Exact nine-character title ID.
 * @param[in] forceMountUx Nonzero to force the `ux0:` savedata lookup path.
 * @param[in] opt Required options containing the result buffer.
 */
int _sceAppMgrGetSaveDataInfo(const char *path, const char *titleId, int forceMountUx, const SceAppMgrGetSaveDataInfoOpt *opt);

/**
 * @param[out] accountId Receives the 64-bit `ACCOUNT_ID` value.
 */
int _sceAppMgrGetSaveDataInfoForSpecialExport(const char *path, SceUInt64 *accountId, const SceAppMgrGetSaveDataInfoForSpecialExportOpt *opt);

/** Get the 0x80-byte application status for an application ID. */
int _sceAppMgrGetStatusByAppId(int appId, SceAppMgrAppStatus *appStatus);

/** Get the 0x80-byte application status for a process ID. */
int _sceAppMgrGetStatusById(ScePID processId, SceAppMgrAppStatus *appStatus);

/** Get the 0x80-byte application status for an application name. */
int _sceAppMgrGetStatusByName(const char *appName, SceAppMgrAppStatus *appStatus, const SceAppMgrGetStatusByNameOpt *opt);

/**
 * @param[in] location Must be 1 on FW 3.60.
 * @param[out] sdpr Receives the 64-bit SDPR value.
 */
int _sceAppMgrGetSystemDataFilePlayReady(SceUInt32 location, SceUInt64 *sdpr);

/**
 * Get a per-user directory path for a storage partition.
 *
 * Partition ::SCE_APPMGR_USER_DIR_PARTITION_UR0 returns `ur0:user/<NN>`.
 * ::SCE_APPMGR_USER_DIR_PARTITION_UX0 first requires the corresponding
 * `ur0:` directory to exist, then returns `ux0:user/<NN>`.
 *
 * FW 3.60 requires \a userDirPathSize to be at least 32 and writes at most 32
 * bytes. The separate option block is ignored.
 */
int _sceAppMgrGetUserDirPath(SceAppMgrUserDirPartition partition, char *userDirPath, SceSize userDirPathSize, const SceAppMgrGetUserDirPathOpt *opt);

/** Get the 16-byte randomized drive ID for the vs0 user-data mount. */
int _sceAppMgrGetVs0UserDataDrive(ScePfsRndDriveId *rnd_drive_id);

/** Get the 16-byte randomized drive ID for the vs0 user-module mount. */
int _sceAppMgrGetVs0UserModuleDrive(ScePfsRndDriveId *rnd_drive_id);

/**
 * Launch an application URI.
 *
 * FW 3.60 accepts a URI of at most 0x822 characters. \a param is optional;
 * when present, it supplies a 0x40-byte launch descriptor. \a opt is required
 * and ignored on FW 3.60.
 */
int _sceAppMgrLaunchAppByUri2(const char *uri, const SceAppMgrLaunchAppByUri2Param *param, const SceAppMgrLaunchAppByUri2Opt *opt);

/**
 * @param[in] extraData Optional launch data of at most 0x1800 bytes.
 * @param[in] extraDataSize Size of \a extraData; the maximum value is 0x1800.
 * @param[in] param Optional launch descriptor for `NPXS10085`.
 */
int _sceAppMgrLaunchVideoStreamingApp(const void *extraData, SceSize extraDataSize, const SceAppMgrLaunchVideoStreamingAppOpt *param);

/**
 * Replace the current game process with another executable.
 *
 * On FW 3.60 \a optParam must be NULL. \a opt is a required raw option block
 * whose contents are ignored.
 */
int _sceAppMgrLoadExec(const char *appPath, char *const argv[], const SceAppMgrLoadExecOptParam *optParam, const SceAppMgrLoadExecOpt *opt);

/**
 * @param[out] paramSfoBuffer Output buffer for the save-data `param.sfo` file.
 * @param[in] bufferSize Maximum number of bytes to write.
 */
int _sceAppMgrLoadSaveDataSystemFile(const char *path, void *paramSfoBuffer, SceSize bufferSize, const SceAppMgrLoadSaveDataSystemFileOpt *opt);

/**
 * Mount a USB mass-storage image and return its randomized mount point.
 *
 * FW 3.60 requires \a mountId to be 600 and \a partitionName to contain
 * exactly 32 uppercase hexadecimal characters. The backing path is
 * `ux0:umass/<partitionName>.img`.
 */
int _sceAppMgrLoopBackMount(int mountId, const char *partitionName, char mountPoint[16], const SceAppMgrLoopBackMountOpt *opt);

/** Mount one of the PSP emulator MMS locations selected by IDs 400 through 402. */
int _sceAppMgrMmsMount(int mountId, char mountPoint[16]);

/** Mount photo application data. */
int _sceAppMgrPhotoMount(const SceAppMgrPhotoMountParam *param, const SceAppMgrPhotoMountOpt *opt);

/** Mount the PSP emulator savedata root. */
int _sceAppMgrPspSaveDataRootMount(char mountPoint[16]);

/** Receive and consume one pending application event. */
int _sceAppMgrReceiveEvent(SceAppMgrEvent *appEvent);

/** Get the number of pending application events. */
int _sceAppMgrReceiveEventNum(int *eventNum);

/**
 * Receive and consume one pending system event for the calling application.
 *
 * FW 3.60 may return activate, deactivate, resume, quit, store-purchase,
 * NP-message-arrival, or store-redemption events from
 * ::SceAppMgrSystemEventType.
 */
int _sceAppMgrReceiveSystemEvent(SceAppMgrSystemEvent *systemEvent);

/** Mount savedata after validating its PFS keystone and passcode. */
int _sceAppMgrSaveDataAddMount(const SceAppMgrGenericMountContext *mountContext);

/**
 * Enable or disable network-disconnection warning dialogs.
 *
 * FW 3.60 requires \a optParam to be NULL.
 */
int _sceAppMgrSetNetworkDisconnectionWarningDialogState(SceBool enable, const SceAppMgrNetworkDisconnectionWarningDialogStateOptParam *optParam);

/**
 * @param[in] sdpr Required 64-bit SDPR value when \a location is 1.
 * @param[in] data Required data buffer when \a location is 2.
 * @param[in] dataSize Size of \a data; the maximum value is 0x4000.
 */
int _sceAppMgrSetSystemDataFilePlayReady(SceUInt32 location, const SceUInt64 *sdpr, const void *data, SceSize dataSize);

/**
 * @param[in] contentId A NUL-terminated content ID of at most 26 characters.
 */
int _sceAppMgrThemeDataMount(const char *contentId, const char *themePath, const char *rifPath, const SceAppMgrThemeDataMountOpt *opt);

/** Mount trophy data for \a processId using mount ID 300 or 302. */
int _sceAppMgrTrophyMount(int mountId, ScePID processId, char mountPoint[16]);

/**
 * Mount trophy data for another title.
 *
 * FW 3.60 accepts mount ID 303 for the normal trophy path and 304 for its
 * `_BK` backup. \a titleId must have the exact form `ABCD12345_67`.
 */
int _sceAppMgrTrophyMountById(int mountId, const char *titleId, char mountPoint[16], const SceAppMgrTrophyMountByIdOpt *opt);

/** Unmount a 16-byte randomized mount point belonging to the calling process. */
int _sceAppMgrUmount(const char *mountPoint);

/** Update savedata `param.sfo` metadata at \a path. */
int _sceAppMgrUpdateSaveDataParam(const char *path, const SceAppMgrUpdateSaveDataParamOpt *opt);

/**
 * Mount a working-directory location selected by \a mountId.
 *
 * FW 3.60 accepts mount IDs 200, 201, 202, 203, 204, and 206.
 */
int _sceAppMgrWorkDirMount(int mountId, char mountPoint[16]);

/**
 * Mount another title's working directory.
 *
 * Mount ID 205 selects `ux0:cache/<titleId>` and requires an exact
 * nine-character title ID. Mount ID 207 selects
 * `ux0:temp/app_work/<titleId>/rec`.
 */
int _sceAppMgrWorkDirMountById(int mountId, const char *titleId, char mountPoint[16], const SceAppMgrWorkDirMountByIdOpt *opt);

/** Activate an application. This export is restricted to SceShell. */
int sceAppMgrActivateApp(int appId);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_APPMGR_H_ */
