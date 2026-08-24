/**
 * \usergroup{SceLibc}
 * \usage{psp2/libc.h,SceLibc_stub}
 */

#ifndef _PSP2_LIBC_H_
#define _PSP2_LIBC_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Append at most \p count characters from one string to another.
 *
 * The destination is always terminated with a NUL after the copied suffix.
 * The function does not know the destination capacity and performs no overlap
 * or overflow check.
 *
 * @param[in,out] dest NUL-terminated destination string.
 * @param[in] src NUL-terminated source string.
 * @param[in] count Maximum number of source characters to append.
 *
 * @return \p dest.
 */
char *strncat(char *dest, const char *src, size_t count);

#ifndef _PSP2KERN_KERNEL_SYSCLIB_H_
/**
 * Copy at most \p count characters with runtime-constraint checks.
 *
 * On success, FW 3.60 copies `strnlen_s(src, count)` characters and appends a
 * NUL. The destination capacity must be greater than that bounded length, and
 * the source characters must not overlap the complete destination range.
 *
 * A NULL destination, zero destination capacity, or capacity greater than
 * `INT_MAX` invokes the runtime constraint handler without writing the
 * destination. A NULL source, count greater than `INT_MAX`, insufficient
 * capacity, or overlap first sets `dest[0]` to NUL and then invokes the
 * handler.
 *
 * This four-argument SceLibc export is ABI-incompatible with the distinct
 * three-argument SceSysclib `strncpy_s` export declared in
 * `psp2kern/kernel/sysclib.h`.
 *
 * @param[out] dest Destination buffer.
 * @param[in] destSize Total destination capacity.
 * @param[in] src Source string.
 * @param[in] count Maximum number of source characters to examine and copy.
 *
 * @return 0 on success, or 34 (`ERANGE`) after a runtime-constraint violation.
 */
int strncpy_s(char *dest, size_t destSize, const char *src, size_t count);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_LIBC_H_ */
