/**
 * \usergroup{SceCoredumpNounlink}
 * \usage{psp2/coredump_nounlink.h,SceCoredumpNounlink_stub}
 */

#ifndef _PSP2_COREDUMP_NOUNLINK_H_
#define _PSP2_COREDUMP_NOUNLINK_H_

#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Appends data to the current coredump's user-data section.
 *
 * This function is intended to be called from a ::SceCoredumpHandler. All
 * calls made by one handler share a 0x4000-byte destination on FW 3.60. A call
 * that would exceed the remaining capacity succeeds with a short byte count.
 *
 * @param[in] data Data to append.
 * @param[in] size Number of bytes requested.
 *
 * @return The number of bytes appended, which can be less than \p size when
 * the destination is full, or a negative error code. The return value has
 * ::SceSSize semantics despite the published \c int type.
 *
 * @retval 0x800A0000 The user buffer is inaccessible.
 * @retval 0x800A0003 No user-data capture is active.
 */
int sceCoredumpWriteUserData(const void *data, SceSize size);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_COREDUMP_NOUNLINK_H_ */
