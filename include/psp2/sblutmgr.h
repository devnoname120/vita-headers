/**
 * \usergroup{SceSblUtMgr}
 * \usage{psp2/sblutmgr.h,SceSblUtMgr_stub}
 */

#ifndef _PSP2_SBLUTMGR_H_
#define _PSP2_SBLUTMGR_H_

#include <psp2/types.h>
#include <psp2common/sblpostssmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the remaining validity time for the initialized Utoken.
 *
 * This operation is restricted to system programs.
 *
 * @param[out] remaining_seconds - Receives -1 when no validity deadline is
 * configured or the deadline is 0xFFFFFFFF, 0 when the deadline has passed,
 * or the number of seconds until the deadline otherwise.
 *
 * @return SCE_OK on success, 0x800F1A26 if no Utoken has been initialized,
 *         or another negative error code.
 */
int sceSblUtMgrGetRemainingValidityTime(int *remaining_seconds);

/**
 * Copy the Utoken name.
 *
 * This operation is restricted to system programs and requires initialized
 * Utoken state. No terminator is added when fewer than 0x18 bytes are copied.
 *
 * @param[out] name - Destination buffer.
 * @param[in] size - Destination size. At most 0x18 bytes are copied.
 *
 * @return SCE_OK on success, < 0 on error.
 */
int sceSblUtMgrGetUtName(char *name, SceSize size);

/**
 * Read and verify the persisted encrypted Utoken.
 *
 * This operation is restricted to system programs.
 *
 * The returned contents remain encrypted; FW 3.60 uses secure-module command
 * 1 only to verify them before copying them to user memory.
 *
 * @param[out] utoken - Receives the encrypted Utoken.
 * @param[in] size - Must be at least 0x800; exactly 0x800 bytes are written.
 *
 * @return SCE_OK on success, < 0 on error.
 */
int sceSblUtMgrReadUtoken(SceUtoken *utoken, SceSize size);

/**
 * Overwrite and remove the persisted Utoken file.
 *
 * This operation is restricted to system programs.
 *
 * Already parsed Utoken flags remain cached on FW 3.60 until the module is
 * reinitialized.
 *
 * @return SCE_OK on success, < 0 on error.
 */
int sceSblUtMgrResetUtokenFile(void);

/**
 * Verify and persist an encrypted Utoken.
 *
 * This operation is restricted to system programs.
 *
 * @param[in] utoken - Encrypted Utoken to verify and persist.
 * @param[in] size - Must be at least 0x800; exactly 0x800 bytes are consumed.
 *
 * @return SCE_OK on success, < 0 on error.
 */
int sceSblUtMgrUpdateUtoken(const SceUtoken *utoken, SceSize size);

#define sceSblUtMgrGetCurrentSecureTick        sceSblUtMgrGetRemainingValidityTime
#define sceSblUtMgrIsTrilithiumFlagEnabled     sceSblUtMgrGetUtName

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_SBLUTMGR_H_ */
