/**
 * \usergroup{SceSblRng}
 * \usage{psp2/sblrng.h,SceSblSsMgr_stub}
 */

#ifndef _PSP2_SBLRNG_H_
#define _PSP2_SBLRNG_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceKernelGetRandomNumberParam {
	SceUInt32 dstSize;  //!< Number of bytes copied to the destination; must be at most 0x40.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceKernelGetRandomNumberParam;
VITASDK_BUILD_ASSERT_EQ(8, SceKernelGetRandomNumberParam); // size is from FW 3.60

/**
 * Generate up to 0x40 bytes using SceSblRng's pseudorandom generator.
 *
 * The provider first generates \a length bytes, then copies the number in
 * ::SceKernelGetRandomNumberParam::dstSize to \a pDst. Bytes between those
 * two sizes are zero if the destination size is larger than \a length. The
 * public ::sceKernelGetRandomNumber wrapper sets both sizes to the same value.
 *
 * @param[out] pDst - Required destination buffer.
 * @param[in] length - Number of random bytes to generate; must be at most 0x40.
 * @param[in] pParam - Required eight-byte input parameter structure.
 *
 * @return 0 on success, < 0 on error.
 */
int _sceKernelGetRandomNumber(void *pDst, SceSize length, const SceKernelGetRandomNumberParam *pParam);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_SBLRNG_H_ */
