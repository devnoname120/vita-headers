/**
 * \kernelgroup{SceFios2Kernel}
 * \usage{psp2kern/fios2.h,SceFios2KernelForDriver_stub}
 */

#ifndef _PSP2KERN_FIOS2_H_
#define _PSP2KERN_FIOS2_H_

#include <psp2kern/types.h>
#include <psp2common/fios2.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Overlay process file system overlay
 *
 * @param[in]  overlay - Overlay config pointer
 * @param[out] outID   - outID pointer
 *
 * @return     Error code or zero on success
 */
int ksceFiosKernelOverlayAdd(SceFiosOverlay *overlay, SceFiosOverlayID *outID);

/**
 * Overlay process file system overlay
 *
 * @param[in]  pid     - Process id
 * @param[in]  overlay - Overlay config pointer
 * @param[out] outID   - outID pointer
 *
 * @return     Error code or zero on success
 */
int ksceFiosKernelOverlayAddForProcess(SceUID pid, SceFiosOverlay *overlay, SceFiosOverlayID *outID);

/**
 * Remove process file system overlay
 *
 * @param[in] pid - Process id
 * @param[in] id  - Overlay id
 *
 * @return     Error code or zero on success
 */
int ksceFiosKernelOverlayRemoveForProcess(SceUID pid, SceFiosOverlayID id);

/**
 * Resolves a path synchronously through a process's overlays.
 *
 * @p maxPath must be between 1 and 0x400. The input must terminate within that
 * limit and @p outPath must provide at least @p maxPath bytes. When application
 * overlays are suppressed for the current thread, orders below 0x80 are skipped.
 *
 * @param[in]  pid             - Process whose overlays are used.
 * @param[in]  resolveForWrite - Must be 0 for read resolution or 1 for write resolution.
 * @param[in]  inPath          - Path to resolve.
 * @param[out] outPath         - Resolved path output buffer.
 * @param[in]  maxPath         - Input and output path limit.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayResolveSync(SceUID pid, int resolveForWrite, const char *inPath, char *outPath, SceSize maxPath);


/**
 * Gets an overlay from the calling process's table.
 *
 * @param[in]  id         - Overlay identifier.
 * @param[out] outOverlay - Receives a copy of the complete overlay.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayGetInfo(SceFiosOverlayID id, SceFiosOverlay *outOverlay);

/**
 * Gets an overlay from a process's table.
 *
 * The complete 0x258-byte overlay is copied to @p outOverlay. The pointer is
 * not retained and access to another process's table is privilege-checked.
 *
 * @param[in]  pid        - Process whose overlay table is queried.
 * @param[in]  id         - Overlay identifier.
 * @param[out] outOverlay - Receives the overlay information.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayGetInfoForProcess(SceUID pid, SceFiosOverlayID id, SceFiosOverlay *outOverlay);

/**
 * Gets the overlay IDs in an inclusive order range.
 *
 * @param[in]  pid           - Process whose overlays are enumerated.
 * @param[in]  minOrder      - Minimum value of an overlay's order field to include.
 * @param[in]  maxOrder      - Maximum value of an overlay's order field to include.
 * @param[out] outIDs        - Overlay ID output buffer, or NULL when @p maxIDs is zero.
 * @param[in]  maxIDs        - Maximum number of overlay IDs to write.
 * @param[out] actualIDs     - Optional pointer receiving the total match count, including entries beyond @p maxIDs.
 *
 * @return Error code or zero on success.
 */
int ksceFiosKernelOverlayGetList(SceUID pid, SceUInt8 minOrder, SceUInt8 maxOrder, SceFiosOverlayID *outIDs, SceSize maxIDs, SceSize *actualIDs);

/**
 * Gets the recommended scheduler index for a partially resolved path.
 *
 * @param[in] schedulerCount        - Number of available schedulers.
 * @param[in] partiallyResolvedPath - Optional partially resolved path.
 *
 * @return 1 for a host[0-9]: path when schedulerCount is greater than 1,
 *         otherwise 0.
 */
int ksceFiosKernelOverlayGetRecommendedScheduler(int schedulerCount, const char *partiallyResolvedPath);

/**
 * Replaces an overlay in the calling process's table.
 *
 * The provider validates and copies @p newValue synchronously. It preserves
 * the process and overlay IDs and recomputes both path lengths.
 *
 * @param[in] id       - Overlay identifier.
 * @param[in] newValue - Replacement overlay configuration.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayModify(SceFiosOverlayID id, const SceFiosOverlay *newValue);

/**
 * Replaces an overlay in a process's table.
 *
 * The input is copied and never retained. Access to another process's table is
 * privilege-checked.
 *
 * @param[in] pid      - Process whose overlay is replaced.
 * @param[in] id       - Overlay identifier.
 * @param[in] newValue - Replacement overlay configuration.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayModifyForProcess(SceUID pid, SceFiosOverlayID id, const SceFiosOverlay *newValue);

/**
 * Removes an overlay synchronously from the calling process's table.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayRemove(SceFiosOverlayID id);

/**
 * Resolves a path through overlays within an inclusive order range.
 *
 * @p maxPath must be between 1 and 0x400. The input must terminate within that
 * limit and @p outPath must provide at least @p maxPath bytes. If application
 * overlays are suppressed for the current thread, the supplied @p minOrder is
 * replaced with 0x80.
 *
 * @param[in]  pid             - Process whose overlays are used.
 * @param[in]  resolveForWrite - Must be 0 for read resolution or 1 for write resolution.
 * @param[in]  inPath          - Path to resolve.
 * @param[out] outPath         - Resolved path output buffer.
 * @param[in]  maxPath         - Output buffer size.
 * @param[in]  minOrder        - Minimum value of an overlay's order field to include.
 * @param[in]  maxOrder        - Maximum value of an overlay's order field to include.
 *
 * @return Error code or zero on success.
 */
int ksceFiosKernelOverlayResolveWithRangeSync(SceUID pid, int resolveForWrite, const char *inPath, char *outPath, SceSize maxPath, SceUInt8 minOrder, SceUInt8 maxOrder);

/**
 * Returns the current thread's application-overlay suppression state.
 *
 * A nonzero state skips orders 0x00 through 0x7F. Privileged overlays
 * with orders 0x80 through 0xFF remain active.
 *
 * @return 0 or 1.
 */
int ksceFiosKernelOverlayThreadIsDisabled(void);

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
int ksceFiosKernelOverlayThreadSetDisabled(int disabled);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_FIOS2_H_ */
