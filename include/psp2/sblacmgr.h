/**
 * \usergroup{SceSblACMgr}
 * \usage{psp2/sblacmgr.h,SceSblACMgr_stub}
 */


#ifndef _PSP2_SBLACMGR_H_
#define _PSP2_SBLACMGR_H_

#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check whether the calling process is a game program.
 *
 * This SceLibKernel wrapper forwards to ::_sceSblACMgrIsGameProgram.
 *
 * @param[out] result - Required pointer that receives 1 for a game program,
 *                      or 0 otherwise.
 *
 * @return SCE_OK on success, 0x800F0916 if \a result is NULL, or a
 *         copy-to-user error.
 */
SceInt32 sceSblACMgrIsGameProgram(SceBool *result);

/**
 * Check whether the calling process is a game program.
 *
 * @param[out] pResult - Required pointer to a four-byte value that receives 1
 *                      if the calling process is a game program, otherwise 0.
 *
 * @return SCE_OK on success, 0x800F0916 if \a pResult is NULL, or a
 *         copy-to-user error.
 */
int _sceSblACMgrIsGameProgram(int *pResult);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_SBLACMGR_H_ */
