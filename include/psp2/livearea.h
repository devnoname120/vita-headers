/**
 * \usergroup{SceLiveAreaUtil}
 * \usage{psp2/livearea.h,SceLiveArea_stub,SCE_SYSMODULE_LIVEAREA}
 */

#ifndef _PSP2_LIVEAREA_H_
#define _PSP2_LIVEAREA_H_

#include <psp2/types.h>
#include <psp2common/kernel/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LiveArea frame target.
 *
 * Retail frames take precedence over normal frames when both variants of the
 * same frame are present.
 */
typedef enum SceLiveAreaTargetType {
	SCE_LIVEAREA_TARGET_TYPE_NORMAL = 0, //!< Normal LiveArea data under `sce_sys/livearea/contents`.
	SCE_LIVEAREA_TARGET_TYPE_RETAIL = 1  //!< Retail override data under `sce_sys/retail/livearea/contents`.
} SceLiveAreaTargetType;

/**
 * Poll an asynchronous LiveArea operation.
 *
 * Only one asynchronous operation can be pending in a process at a time.
 * The SceShell worker progresses independently; this function only retrieves
 * its current or terminal result. Retrieving a terminal result clears the
 * process-local pending request, so a subsequent call reports no request.
 *
 * @return 1 while the operation is pending, 0 when it completed successfully,
 * or a negative error code. Calling this without a pending operation returns
 * an error.
 */
int sceLiveAreaGetStatus(void);

/**
 * Replace all LiveArea data for the calling title synchronously.
 *
 * Normally the target is the title associated with the calling process. After
 * ::sceLiveAreaEnableBgAppMode is called, an implicit target is instead read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 *
 * @note On retail FW 3.60, this implicit-title variant requires the original
 * source path to begin with `app0:` or `savedata0:`. An `app0:` path under
 * `/sce_` is limited to `sce_sys/livearea/contents` or
 * `sce_sys/retail/livearea/contents`. Use
 * ::sceLiveAreaReplaceAllForTitleSync with a non-empty title ID for a source
 * staged under `ux0:data`.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaReplaceAllSync(const char *contentsPath);

/**
 * Replace all LiveArea data for the calling title asynchronously.
 *
 * Normally the target is the title associated with the calling process. After
 * ::sceLiveAreaEnableBgAppMode is called, an implicit target is instead read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * Poll ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 *
 * @note The source-path restrictions documented for
 * ::sceLiveAreaReplaceAllSync also apply to this function.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaReplaceAllAsync(const char *contentsPath);

/**
 * Replace all LiveArea data for a title synchronously.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 * @param[in] titleId - Target title ID. If NULL or empty, SceShell resolves an
 * implicit target from the calling process, or from its `_fg_title_id` LSDB
 * property after ::sceLiveAreaEnableBgAppMode is called.
 * @param[in] lastModified - Modification time to store with the LiveArea data,
 * or NULL to use a zero tick.
 *
 * @note On FW 3.60, this ForTitle variant accepts externally staged sources
 * such as `ux0:data`. Supply a non-empty \a titleId to avoid implicit title
 * resolution.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaReplaceAllForTitleSync(const char *contentsPath, const char *titleId, const SceRtcTick *lastModified);

/**
 * Replace all LiveArea data for a title asynchronously.
 *
 * Poll ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 * @param[in] titleId - Target title ID. If NULL or empty, SceShell resolves an
 * implicit target from the calling process, or from its `_fg_title_id` LSDB
 * property after ::sceLiveAreaEnableBgAppMode is called.
 * @param[in] lastModified - Modification time to store with the LiveArea data,
 * or NULL to use a zero tick.
 *
 * @note The source-path and title-resolution behavior documented for
 * ::sceLiveAreaReplaceAllForTitleSync also applies to this function.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaReplaceAllForTitleAsync(const char *contentsPath, const char *titleId, const SceRtcTick *lastModified);

/**
 * Update one LiveArea frame for the calling title synchronously.
 *
 * The frame ID and frame contents are read from the XML document.
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXml - Frame XML document.
 * @param[in] frameXmlLength - XML size in bytes. A negative value uses the
 * NUL-terminated string length. The resulting size must not exceed 10239 bytes.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaUpdateFrameSync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType);

/**
 * Update one LiveArea frame for the calling title asynchronously.
 *
 * The frame ID and frame contents are read from the XML document. Poll
 * ::sceLiveAreaGetStatus to obtain the final result.
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXml - Frame XML document.
 * @param[in] frameXmlLength - XML size in bytes. A negative value uses the
 * NUL-terminated string length. The resulting size must not exceed 10239 bytes.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaUpdateFrameAsync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType);

/**
 * Update one LiveArea frame for a title synchronously.
 *
 * The frame ID and frame contents are read from the XML document.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXml - Frame XML document.
 * @param[in] frameXmlLength - XML size in bytes. A negative value uses the
 * NUL-terminated string length. The resulting size must not exceed 10239 bytes.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. If NULL or empty, SceShell resolves an
 * implicit target from the calling process, or from its `_fg_title_id` LSDB
 * property after ::sceLiveAreaEnableBgAppMode is called.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaUpdateFrameForTitleSync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Update one LiveArea frame for a title asynchronously.
 *
 * The frame ID and frame contents are read from the XML document. Poll
 * ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXml - Frame XML document.
 * @param[in] frameXmlLength - XML size in bytes. A negative value uses the
 * NUL-terminated string length. The resulting size must not exceed 10239 bytes.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. If NULL or empty, SceShell resolves an
 * implicit target from the calling process, or from its `_fg_title_id` LSDB
 * property after ::sceLiveAreaEnableBgAppMode is called.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaUpdateFrameForTitleAsync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Update multiple LiveArea frames for a title synchronously.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXmlArray - Array of NUL-terminated frame XML documents.
 * @param[in] frameCount - Number of entries in \a frameXmlArray. Must be nonzero.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process. Background-application mode is
 * not transmitted for this operation on FW 3.60.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaUpdateFramesForTitleSync(const char *formatVersion, const char *const *frameXmlArray, SceUInt32 frameCount, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Update multiple LiveArea frames for a title asynchronously.
 *
 * Poll ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXmlArray - Array of NUL-terminated frame XML documents.
 * @param[in] frameCount - Number of entries in \a frameXmlArray. Must be nonzero.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process. Background-application mode is
 * not transmitted for this operation on FW 3.60.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaUpdateFramesForTitleAsync(const char *formatVersion, const char *const *frameXmlArray, SceUInt32 frameCount, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Get the overall LiveArea content revision for the calling title.
 *
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[out] revision - Receives the revision.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaGetRevision(SceUInt64 *revision);

/**
 * Get a LiveArea frame revision for the calling title.
 *
 * If normal and retail variants exist, the retail frame revision is returned.
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] frameId - Non-empty frame ID.
 * @param[out] revision - Receives the revision.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaGetFrameRevision(const char *frameId, SceUInt64 *revision);

/**
 * Get the user data associated with a LiveArea frame for the calling title.
 *
 * If normal and retail variants exist, the retail frame user data is returned.
 * The result is NUL-terminated. FW 3.60 caps the transfer to 1024 bytes when a
 * larger buffer size is supplied.
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] frameId - Frame ID.
 * @param[out] buffer - Destination buffer.
 * @param[in] bufferSize - Destination buffer size. Must be nonzero.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaGetFrameUserData(const char *frameId, char *buffer, SceSize bufferSize);

/**
 * Enable foreground-title resolution for a background companion application.
 *
 * By default, an operation without an explicit title ID targets the title
 * associated with the calling process. After this function is called, FW 3.60
 * treats the caller as a background companion and reads the foreground target
 * title from the caller's `_fg_title_id` LSDB property. If that lookup does not
 * produce a title ID, an affected operation can fail with `0x801040FF`.
 *
 * This sets a process-global flag to 1. There is no corresponding function to
 * disable it. The flag is sent with replace-all, single-frame, revision, and
 * frame user-data requests; multi-frame updates do not use it on FW 3.60.
 *
 * @warning This does not put the calling application into the background,
 * register an `_fg_title_id` relationship, grant additional privileges, or
 * bypass the replace-all source-path policy. A normal foreground application
 * should generally use an explicit-title function instead.
 *
 * @note Exported by the `SceLiveAreaUtilBgApp` library.
 *
 * @return 0 on FW 3.60.
 */
int sceLiveAreaEnableBgAppMode(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_LIVEAREA_H_ */
