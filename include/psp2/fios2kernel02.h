/**
 * \usergroup{SceFios2Kernel02}
 * \usage{psp2/fios2kernel02.h,SceFios2Kernel_stub}
 */

#ifndef _PSP2_FIOS2KERNEL02_H_
#define _PSP2_FIOS2KERNEL02_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>
#include <psp2common/fios2.h>

#ifdef __cplusplus
extern "C" {
#endif


int sceFiosKernelOverlayAddForProcess02(SceUID pid, SceFiosOverlay *overlay, SceFiosOverlayID *outID);


typedef struct sceFiosKernelOverlayGetList02_opt {
	SceFiosOverlayID *outIDs; //!< Output buffer for overlay IDs; may be NULL only when maxIDs is zero.
	SceSize maxIDs; //!< Maximum number of overlay IDs to write; must not exceed 128 on FW 3.60.
	SceSize *actualIDs; //!< Optional pointer receiving the total match count, including entries beyond maxIDs.
	SceSize outIDsBufferSize; //!< Number of bytes copied to outIDs; must not exceed 0x200 on FW 3.60.
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} sceFiosKernelOverlayGetList02_opt;
VITASDK_BUILD_ASSERT_EQ(0x18, sceFiosKernelOverlayGetList02_opt); // size is from FW 3.60

typedef struct sceFiosKernelOverlayGetRecommendedScheduler02_opt {
	SceUInt32 reserved[2]; //!< Copied from user memory but actually unused on FW 3.60.
} sceFiosKernelOverlayGetRecommendedScheduler02_opt;
VITASDK_BUILD_ASSERT_EQ(0x8, sceFiosKernelOverlayGetRecommendedScheduler02_opt); // size is from FW 3.60

typedef struct sceFiosKernelOverlayResolveSync02_opt {
	char *outPath; //!< Resolved path output buffer.
	SceSize maxPath; //!< Maximum path length used by the resolver; must be between 1 and 0x400.
	SceUInt32 reserved0; //!< Ignored on FW 3.60.
	SceSize outPathBufferSize; //!< Number of bytes copied to outPath; must not exceed 0x400 on FW 3.60.
	SceUInt32 reserved1[2]; //!< Ignored on FW 3.60.
} sceFiosKernelOverlayResolveSync02_opt;
VITASDK_BUILD_ASSERT_EQ(0x18, sceFiosKernelOverlayResolveSync02_opt); // size is from FW 3.60

typedef struct sceFiosKernelOverlayResolveWithRangeSync02_opt {
	char *outPath; //!< Resolved path output buffer.
	SceSize maxPath; //!< Maximum path length used by the resolver; must be between 1 and 0x400.
	SceUInt8 minOrder; //!< Minimum overlay order to include.
	SceUInt8 maxOrder; //!< Maximum overlay order to include.
	SceUInt8 reserved0[2]; //!< Ignored on FW 3.60.
	SceUInt32 reserved1; //!< Ignored on FW 3.60.
	SceSize outPathBufferSize; //!< Number of bytes copied to outPath; must not exceed 0x400 on FW 3.60.
	SceUInt32 reserved2[2]; //!< Ignored on FW 3.60.
} sceFiosKernelOverlayResolveWithRangeSync02_opt;
VITASDK_BUILD_ASSERT_EQ(0x1C, sceFiosKernelOverlayResolveWithRangeSync02_opt); // size is from FW 3.60

/**
 * Gets information about an overlay in a process's table.
 *
 * The provider copies the complete ::SceFiosOverlay to @p outOverlay and does
 * not retain the pointer.
 *
 * @param[in]  pid        - Process whose overlay table is queried.
 * @param[in]  id         - Overlay identifier.
 * @param[out] outOverlay - Receives the overlay information.
 *
 * @return 0 on success, or a negative error code.
 */
int sceFiosKernelOverlayGetInfoForProcess02(SceUID pid, SceFiosOverlayID id, SceFiosOverlay *outOverlay);

/**
 * Gets the overlay IDs in an inclusive order range.
 *
 * @p opt is required. When \c opt->outIDs is non-NULL, \c opt->maxIDs must not
 * exceed 128 on FW 3.60. \c opt->actualIDs may be NULL; when supplied it
 * receives the total number of matches, including entries beyond
 * \c opt->maxIDs.
 * The reserved fields are ignored on FW 3.60.
 *
 * @param[in] pid      - Process whose overlays are enumerated.
 * @param[in] minOrder - Minimum overlay order to include.
 * @param[in] maxOrder - Maximum overlay order to include.
 * @param[in] opt      - Enumeration options and output pointers.
 *
 * @return Error code or zero on success.
 */
int sceFiosKernelOverlayGetList02(SceUID pid, SceUInt8 minOrder, SceUInt8 maxOrder, const sceFiosKernelOverlayGetList02_opt *opt);

/**
 * Gets the recommended scheduler index for a partially resolved path.
 *
 * @p opt is required and FW 3.60 copies all eight bytes from user memory, but
 * does not interpret their contents. @p partiallyResolvedPath may be NULL; a
 * non-NULL path must terminate within 0x400 bytes.
 *
 * @param[in] schedulerCount        - Number of available schedulers.
 * @param[in] partiallyResolvedPath - Optional partially resolved path.
 * @param[in] opt                   - Required 8-byte option block.
 *
 * @return 1 for a host[0-9]: path when schedulerCount is greater than 1,
 *         otherwise 0, or a negative error code when a user-memory copy fails.
 */
int sceFiosKernelOverlayGetRecommendedScheduler02(int schedulerCount, const char *partiallyResolvedPath, const sceFiosKernelOverlayGetRecommendedScheduler02_opt *opt);

/**
 * Replaces an overlay in a process's table.
 *
 * The provider validates and copies @p newValue synchronously. It preserves
 * the process and overlay IDs, recomputes both path lengths, and reorders the
 * entry if its order changes. The pointer is not retained.
 *
 * @param[in] pid      - Process whose overlay is replaced.
 * @param[in] id       - Overlay identifier.
 * @param[in] newValue - Replacement overlay configuration.
 *
 * @return 0 on success, or a negative error code.
 */
int sceFiosKernelOverlayModifyForProcess02(SceUID pid, SceFiosOverlayID id, const SceFiosOverlay *newValue);

/**
 * Removes an overlay synchronously from a process's table.
 *
 * @return 0 on success, or a negative error code.
 */
int sceFiosKernelOverlayRemoveForProcess02(SceUID pid, SceFiosOverlayID id);

/**
 * Resolves a path synchronously through a process's overlays.
 *
 * @p resolveForWrite must be 0 for a read or 1 for a write. @p opt is required.
 * Its maxPath field controls path validation and resolution, while
 * outPathBufferSize independently controls the final copy to user memory.
 * The reserved fields are ignored on FW 3.60.
 *
 * @param[in] pid             - Process whose overlays are used.
 * @param[in] resolveForWrite - Must be 0 for read resolution or 1 for write resolution.
 * @param[in] inPath          - Path to resolve.
 * @param[in] opt             - Resolution options and output buffer.
 *
 * @return 0 on success, or a negative error code.
 */
int sceFiosKernelOverlayResolveSync02(SceUID pid, int resolveForWrite, const char *inPath, const sceFiosKernelOverlayResolveSync02_opt *opt);

/**
 * Resolves a path through overlays whose order is between \c minOrder and
 * \c maxOrder, inclusive.
 *
 * @p opt is required. Its order-filter fields are unsigned and minOrder must
 * not exceed maxOrder. The maxPath and outPathBufferSize fields are independent;
 * the reserved fields are ignored on FW 3.60. If application overlays are
 * suppressed for the current thread, the supplied minOrder is replaced with
 * 0x80.
 *
 * @param[in]  pid             - Process whose overlays are used.
 * @param[in]  resolveForWrite - Must be 0 for read resolution or 1 for write resolution.
 * @param[in]  inPath          - Path to resolve.
 * @param[in]  opt             - Resolution options and output buffer.
 *
 * @return Error code or zero on success.
 */
int sceFiosKernelOverlayResolveWithRangeSync02(SceUID pid, int resolveForWrite, const char *inPath, const sceFiosKernelOverlayResolveWithRangeSync02_opt *opt);

/**
 * Returns the current thread's application-overlay suppression state.
 *
 * A nonzero state skips orders 0x00 through 0x7F. Privileged overlays
 * with orders 0x80 through 0xFF remain active.
 *
 * @return 0 or 1.
 */
int sceFiosKernelOverlayThreadIsDisabled02(void);

/**
 * Sets the current thread's application-overlay suppression state.
 *
 * Any nonzero value suppresses orders 0x00 through 0x7F until this function is
 * called with zero or the thread exits. Privileged orders 0x80 through 0xFF
 * remain active.
 *
 * @param[in] disabled - Zero to enable application overlays, or any nonzero value to suppress them.
 *
 * @return 0 on success, or -1 if the thread-local state is unavailable.
 */
int sceFiosKernelOverlayThreadSetDisabled02(int disabled);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_FIOS2KERNEL02_H_ */
