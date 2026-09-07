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
 * @p maxPath must be from 1 through 0x400, inclusive. The input's NUL terminator
 * must fall within that many bytes, and @p outPath must provide at least
 * @p maxPath bytes. When application overlays are disabled for the current
 * thread, orders below 0x80 are skipped.
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
 * Copies the complete 0x258-byte overlay to @p outOverlay. The output buffer
 * must remain valid until this function returns; it is not used afterwards.
 * Access to another process's table requires the appropriate privileges.
 *
 * @param[in]  pid        - Process whose overlay table is queried.
 * @param[in]  id         - Overlay identifier.
 * @param[out] outOverlay - Receives the overlay information.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceFiosKernelOverlayGetInfoForProcess(SceUID pid, SceFiosOverlayID id, SceFiosOverlay *outOverlay);

/**
 * Gets the IDs of overlays whose order is from @p minOrder through @p maxOrder,
 * inclusive.
 *
 * @param[in]  pid           - Process whose overlays are enumerated.
 * @param[in]  minOrder      - Minimum value of an overlay's order field to include.
 * @param[in]  maxOrder      - Maximum value of an overlay's order field to include.
 * @param[out] outIDs        - Overlay ID output buffer, or NULL when @p maxIDs is zero.
 * @param[in]  maxIDs        - Maximum number of overlay IDs to write.
 * @param[out] actualIDs     - Optional pointer receiving the total number of matching overlays, including those beyond @p maxIDs.
 *
 * @return 0 on success, or an error code.
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
 * Validates and copies @p newValue before returning. Keeps the process and
 * overlay IDs and recalculates both path lengths.
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
 * Copies @p newValue before returning. Keep it valid until this function
 * returns; it is not used afterwards. Access to another process's table
 * requires the appropriate privileges.
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
 * Resolves a path through overlays whose order is from @p minOrder through
 * @p maxOrder, inclusive.
 *
 * @p maxPath must be from 1 through 0x400, inclusive. The input's NUL terminator
 * must fall within that many bytes, and @p outPath must provide at least
 * @p maxPath bytes. If application overlays are disabled for the current
 * thread, the supplied @p minOrder is replaced with 0x80.
 *
 * @param[in]  pid             - Process whose overlays are used.
 * @param[in]  resolveForWrite - Must be 0 for read resolution or 1 for write resolution.
 * @param[in]  inPath          - Path to resolve.
 * @param[out] outPath         - Resolved path output buffer.
 * @param[in]  maxPath         - Output buffer size.
 * @param[in]  minOrder        - Minimum value of an overlay's order field to include.
 * @param[in]  maxOrder        - Maximum value of an overlay's order field to include.
 *
 * @return 0 on success, or an error code.
 */
int ksceFiosKernelOverlayResolveWithRangeSync(SceUID pid, int resolveForWrite, const char *inPath, char *outPath, SceSize maxPath, SceUInt8 minOrder, SceUInt8 maxOrder);

/**
 * Returns whether application overlays are disabled for the current thread.
 *
 * A nonzero state skips orders 0x00 through 0x7F. Privileged overlays
 * with orders 0x80 through 0xFF remain active.
 *
 * @return 0 or 1.
 */
int ksceFiosKernelOverlayThreadIsDisabled(void);

/**
 * Enables or disables application overlays for the current thread.
 *
 * Any nonzero value skips orders 0x00 through 0x7F until this function is
 * called with zero or the thread exits. Privileged orders 0x80 through 0xFF
 * remain active.
 *
 * @param[in] disabled - Zero to enable application overlays, or any nonzero value to disable them.
 *
 * @return 0 on success, or -1 if the thread-local state is unavailable.
 */
int ksceFiosKernelOverlayThreadSetDisabled(int disabled);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_FIOS2_H_ */
