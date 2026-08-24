/**
 * \usergroup{SceLiveAreaUtil}
 * \usage{psp2/livearea.h,SceLiveArea_stub SceLiveAreaUtilBgApp_stub,SCE_SYSMODULE_LIVEAREA}
 */

#ifndef _PSP2_LIVEAREA_H_
#define _PSP2_LIVEAREA_H_

#include <psp2/types.h>
#include <psp2common/kernel/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LiveArea frame database layer.
 *
 * The source directory is provided independently to the update function. The
 * paths below are the conventional source locations. Retail frames take
 * precedence over normal frames when both variants of the same frame exist.
 */
typedef enum SceLiveAreaTargetType {
	SCE_LIVEAREA_TARGET_TYPE_NORMAL = 0, //!< Normal layer, conventionally from `sce_sys/livearea/contents`.
	SCE_LIVEAREA_TARGET_TYPE_RETAIL = 1  //!< Retail override layer, usually from `sce_sys/retail/livearea/contents`.
} SceLiveAreaTargetType;

/**
 * Poll an asynchronous LiveArea operation.
 *
 * Only one LiveArea operation can be pending in a process at a time.
 * The SceShell worker progresses independently; this function only retrieves
 * its current or terminal result. Retrieving a terminal result clears the
 * process-local pending request, so a subsequent call reports no request.
 *
 * @retval 1 The operation is still pending.
 * @retval 0 The operation completed successfully. This consumes the terminal
 * result and clears the pending request.
 * @retval 0x80104001 No operation is pending. This is also returned after a
 * terminal result has already been consumed.
 * @return Another negative value is the terminal result returned by SceShell;
 * retrieving it also clears the pending request.
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
 * @retval 0 The replacement completed successfully.
 * @retval 0x80104002 \a contentsPath is NULL or longer than 255 characters.
 * @retval 0x80104003 SceShell could not resolve the source path through the
 * caller's FIOS overlays.
 * @retval 0x8010400B The source path is not permitted by the implicit-title
 * path policy.
 * @return Another negative error code returned by SceShell or LSDB.
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
 * SceLiveAreaUtil serializes \a contentsPath before returning; the caller does
 * not need to retain it while the asynchronous operation is pending.
 *
 * @retval 0 The operation was submitted.
 * @retval 0x80104001 Another asynchronous operation is still pending.
 * @retval 0x80104002 \a contentsPath is NULL or longer than 255 characters.
 * @return Another negative submission error.
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
 * or NULL to store a zero tick. Exactly eight bytes are serialized during the
 * call.
 *
 * @note On FW 3.60, selecting the ForTitle export bypasses the implicit
 * `app0:`/`savedata0:` source allowlist, independently of whether \a titleId is
 * empty. This permits externally staged sources such as `ux0:data`. Supply a
 * non-empty \a titleId to avoid implicit target resolution.
 *
 * @retval 0 The replacement completed successfully.
 * @retval 0x80104002 \a contentsPath is NULL or longer than 255 characters.
 * @retval 0x80104003 SceShell could not resolve the source path through the
 * caller's FIOS overlays.
 * @return Another negative error code returned by SceShell or LSDB.
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
 * or NULL to store a zero tick. Exactly eight bytes are serialized during the
 * call.
 *
 * @note The source-path and title-resolution behavior documented for
 * ::sceLiveAreaReplaceAllForTitleSync also applies to this function.
 *
 * All pointed-to input data is serialized before this function returns.
 *
 * @retval 0 The operation was submitted.
 * @retval 0x80104001 Another asynchronous operation is still pending.
 * @retval 0x80104002 \a contentsPath is NULL or longer than 255 characters.
 * @return Another negative submission error.
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
 * @note The implicit-title source-path policy documented for
 * ::sceLiveAreaReplaceAllSync also applies to frame updates.
 *
 * @retval 0 The frame was updated.
 * @retval 0x80104002 An argument, path length, format version, or target type
 * is invalid.
 * @retval 0x80104004 SceShell could not parse the frame XML.
 * @retval 0x80104009 The XML is longer than 10239 bytes.
 * @retval 0x8010400B The source path is not permitted by the implicit-title
 * path policy.
 * @return Another negative error code returned by SceShell or LSDB.
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
 * @note The implicit-title source-path policy documented for
 * ::sceLiveAreaReplaceAllSync also applies to frame updates.
 *
 * SceLiveAreaUtil serializes the XML, path, and other pointed-to input before
 * returning; the caller does not need to retain them while the operation is
 * pending.
 *
 * @retval 0 The operation was submitted.
 * @retval 0x80104001 Another asynchronous operation is still pending.
 * @retval 0x80104002 An argument, path length, format version, or target type
 * is invalid.
 * @retval 0x80104009 The XML is longer than 10239 bytes.
 * @return Another negative submission error.
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
 * @note Selecting the ForTitle export bypasses the implicit
 * `app0:`/`savedata0:` source allowlist, even when \a titleId is NULL or empty.
 *
 * @retval 0 The frame was updated.
 * @retval 0x80104002 An argument, path length, format version, or target type
 * is invalid.
 * @retval 0x80104004 SceShell could not parse the frame XML.
 * @retval 0x80104009 The XML is longer than 10239 bytes.
 * @return Another negative error code returned by SceShell or LSDB.
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
 * @note Selecting the ForTitle export bypasses the implicit
 * `app0:`/`savedata0:` source allowlist, even when \a titleId is NULL or empty.
 *
 * All pointed-to input data is serialized before this function returns.
 *
 * @retval 0 The operation was submitted.
 * @retval 0x80104001 Another asynchronous operation is still pending.
 * @retval 0x80104002 An argument, path length, format version, or target type
 * is invalid.
 * @retval 0x80104009 The XML is longer than 10239 bytes.
 * @return Another negative submission error.
 */
int sceLiveAreaUpdateFrameForTitleAsync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Update multiple LiveArea frames for a title synchronously.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXmlArray - Array of NUL-terminated frame XML documents.
 * Every entry must be non-NULL; FW 3.60 computes each string length without a
 * bound.
 * @param[in] frameCount - Number of entries in \a frameXmlArray. Must be nonzero.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process. Background-application mode is
 * not transmitted for this operation on FW 3.60.
 *
 * @note This function always uses the ForTitle source-path policy, including
 * when \a titleId is NULL or empty.
 *
 * The complete IPMI request, including the descriptor-size table and each
 * payload rounded up to 16 bytes, must fit in 0x3F00 bytes on FW 3.60.
 *
 * @retval 0 The frames were updated.
 * @retval 0x80104002 An argument, path length, format version, frame count, or
 * target type is invalid.
 * @retval 0x80104004 SceShell could not parse one of the frame documents.
 * @retval 0x80020588 The serialized IPMI request is larger than 0x3F00 bytes.
 * @return Another negative error code returned by SceShell or LSDB.
 */
int sceLiveAreaUpdateFramesForTitleSync(const char *formatVersion, const char *const *frameXmlArray, SceUInt32 frameCount, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Update multiple LiveArea frames for a title asynchronously.
 *
 * Poll ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] formatVersion - LiveArea XML format version. Must be `"01.00"`.
 * @param[in] frameXmlArray - Array of NUL-terminated frame XML documents.
 * Every entry must be non-NULL; FW 3.60 computes each string length without a
 * bound.
 * @param[in] frameCount - Number of entries in \a frameXmlArray. Must be nonzero.
 * @param[in] contentsPath - Directory containing assets referenced by the XML.
 * The path must contain at most 255 characters.
 * @param[in] targetType - One of ::SceLiveAreaTargetType.
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process. Background-application mode is
 * not transmitted for this operation on FW 3.60.
 *
 * @note This function always uses the ForTitle source-path policy, including
 * when \a titleId is NULL or empty. The 0x3F00-byte serialized-request limit
 * documented for ::sceLiveAreaUpdateFramesForTitleSync also applies.
 *
 * The pointer array and every XML string are serialized before this function
 * returns; the caller does not need to retain them while the operation is
 * pending.
 *
 * @retval 0 The operation was submitted.
 * @retval 0x80104001 Another asynchronous operation is still pending.
 * @retval 0x80104002 An argument, path length, format version, frame count, or
 * target type is invalid.
 * @retval 0x80020588 The serialized IPMI request is larger than 0x3F00 bytes.
 * @return Another negative submission error.
 */
int sceLiveAreaUpdateFramesForTitleAsync(const char *formatVersion, const char *const *frameXmlArray, SceUInt32 frameCount, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Get the overall LiveArea content revision for the calling title.
 *
 * FW 3.60 queries the LiveArea object currently loaded by SceShell rather than
 * reading the database directly. Background-application mode does not retarget
 * this operation; SceShell always uses the calling application's title. A
 * temporarily unavailable object is retried every 16 milliseconds for up to
 * approximately 10 seconds.
 *
 * @param[out] revision - Receives the revision.
 *
 * @retval 0 The revision was returned.
 * @retval 0x80104002 \a revision is NULL.
 * @retval 0x8010400A The target's loaded LiveArea object is unavailable.
 * @return Another negative error code returned by SceShell.
 */
int sceLiveAreaGetRevision(SceUInt64 *revision);

/**
 * Get a LiveArea frame revision for the calling title.
 *
 * If normal and retail variants exist, the retail frame revision is returned.
 * FW 3.60 queries the LiveArea object currently loaded by SceShell rather than
 * reading the database directly. Background-application mode does not retarget
 * this operation; SceShell always uses the calling application's title. A
 * temporarily unavailable object is retried every 16 milliseconds for up to
 * approximately 10 seconds.
 *
 * @param[in] frameId - Non-empty frame ID.
 * @param[out] revision - Receives the revision.
 *
 * @retval 0 The revision was returned.
 * @retval 0x80104002 \a frameId is NULL or empty, or \a revision is NULL.
 * @retval 0x8010400A The target's loaded LiveArea object or frame is
 * unavailable.
 * @return Another negative error code returned by SceShell.
 */
int sceLiveAreaGetFrameRevision(const char *frameId, SceUInt64 *revision);

/**
 * Get the user data associated with a LiveArea frame for the calling title.
 *
 * This function queries LSDB directly; it does not require the title's
 * LiveArea object to be loaded. If normal and retail variants exist, the
 * retail frame user data is returned.
 * The result is NUL-terminated. At most
 * `min(bufferSize, 1024) - 1` data bytes are returned; FW 3.60 caps a larger
 * requested transfer to 1024 bytes including the terminator.
 * After ::sceLiveAreaEnableBgAppMode is called, the implicit target is read
 * from the caller's `_fg_title_id` LSDB property.
 *
 * @param[in] frameId - Frame ID. A NULL pointer is invalid; an empty string
 * does not match a frame.
 * @param[out] buffer - Destination buffer.
 * @param[in] bufferSize - Destination buffer size. Must be nonzero.
 *
 * @retval 0 The user-data string was returned.
 * @retval 0x80104002 \a frameId or \a buffer is NULL, or \a bufferSize is zero.
 * @retval 0x8010400A No matching frame exists.
 * @return Another negative error code returned by SceShell or LSDB.
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
 * disable it. On FW 3.60 it retargets replace-all, single-frame, and frame
 * user-data operations. Revision requests transmit the byte but SceShell
 * ignores it, and multi-frame updates do not transmit it.
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
