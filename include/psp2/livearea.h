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
 *
 * @return 1 while the operation is pending, 0 when it completed successfully,
 * or a negative error code. Calling this without a pending operation returns
 * an error.
 */
int sceLiveAreaGetStatus(void);

/**
 * Replace all LiveArea data for the calling title synchronously.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaReplaceAllSync(const char *contentsPath);

/**
 * Replace all LiveArea data for the calling title asynchronously.
 *
 * Poll ::sceLiveAreaGetStatus to obtain the final result.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaReplaceAllAsync(const char *contentsPath);

/**
 * Replace all LiveArea data for a title synchronously.
 *
 * @param[in] contentsPath - Directory containing the replacement LiveArea data.
 * The path must contain at most 255 characters.
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process.
 * @param[in] lastModified - Modification time to store with the LiveArea data,
 * or NULL to use a zero tick.
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
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process.
 * @param[in] lastModified - Modification time to store with the LiveArea data,
 * or NULL to use a zero tick.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaReplaceAllForTitleAsync(const char *contentsPath, const char *titleId, const SceRtcTick *lastModified);

/**
 * Update one LiveArea frame for the calling title synchronously.
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
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaUpdateFrameSync(const char *formatVersion, const char *frameXml, SceInt32 frameXmlLength, const char *contentsPath, SceLiveAreaTargetType targetType);

/**
 * Update one LiveArea frame for the calling title asynchronously.
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
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process.
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
 * @param[in] titleId - Target title ID. NULL or an empty string selects the
 * title associated with the calling process.
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
 * title associated with the calling process.
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
 * title associated with the calling process.
 *
 * @return 0 when the operation was submitted, or a negative error code.
 */
int sceLiveAreaUpdateFramesForTitleAsync(const char *formatVersion, const char *const *frameXmlArray, SceUInt32 frameCount, const char *contentsPath, SceLiveAreaTargetType targetType, const char *titleId);

/**
 * Get the overall LiveArea content revision for the calling title.
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
 *
 * @param[in] frameId - Frame ID.
 * @param[out] buffer - Destination buffer.
 * @param[in] bufferSize - Destination buffer size. Must be nonzero.
 *
 * @return 0 on success, or a negative error code.
 */
int sceLiveAreaGetFrameUserData(const char *frameId, char *buffer, SceSize bufferSize);

/**
 * Enable background-application mode for subsequent LiveArea operations.
 *
 * Background mode changes title resolution and permits paths that are rejected
 * for foreground applications. Once enabled, it remains active for the process.
 * On FW 3.60 the flag applies to replace-all, single-frame, revision, and frame
 * user-data operations. Multi-frame updates do not use it.
 *
 * @return 0.
 */
int sceLiveAreaEnableBgAppMode(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_LIVEAREA_H_ */
