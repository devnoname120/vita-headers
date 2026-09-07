/**
 * \usergroup{SceSblGcAuthMgr}
 * \usage{psp2/sblgcauthmgr.h,SceSblGcAuthMgr_stub}
 */

#ifndef _PSP2_SBLGCAUTHMGR_H_
#define _PSP2_SBLGCAUTHMGR_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceMediaIdType01 {
	SceUInt8 data[0x20]; //!< Opaque output produced by secure command 0x23.
} SceMediaIdType01;
VITASDK_BUILD_ASSERT_EQ(0x20, SceMediaIdType01); // size is from FW 3.60

typedef struct SceSblGcAuthMgrGetMediaIdType01Opt {
	SceSize mediaIdSize; //!< Number of result bytes to copy; maximum 0x20.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceSblGcAuthMgrGetMediaIdType01Opt;
VITASDK_BUILD_ASSERT_EQ(0x8, SceSblGcAuthMgrGetMediaIdType01Opt); // size is from FW 3.60

/** PC activation challenge modes. */
typedef enum ScePcactMode {
	SCE_PCACT_MODE_0 = 0, //!< Requires the previous challenge to have expired.
	SCE_PCACT_MODE_1 = 1, //!< Requires activation data to be absent and records a new expiration.
	SCE_PCACT_MODE_2 = 2  //!< Bypasses the previous challenge expiration check.
} ScePcactMode;
VITASDK_BUILD_ASSERT_EQ(1, ScePcactMode);

typedef struct ScePcactActivationKeyData {
	SceUInt16 magic; //!< Must be set to 0x0211.
	ScePcactMode mode; //!< Must match the mode of the saved challenge state.
	SceUInt8 reserved[0xD]; //!< Must be set to zero.
	SceUInt8 protectedData[0x30]; //!< Protected data bound to the saved challenge state.
	SceUInt8 verificationTag[0x10]; //!< Tag verified before the protected data is accepted.
} ScePcactActivationKeyData;
VITASDK_BUILD_ASSERT_EQ(0x50, ScePcactActivationKeyData); // size is from FW 3.60

typedef struct ScePcactActivationData {
	ScePcactActivationKeyData keyData; //!< Envelope verified against the saved challenge state.
	SceUInt8 activationData[0x1040]; //!< Activation payload passed to NPDRM after verification.
} ScePcactActivationData;
VITASDK_BUILD_ASSERT_EQ(0x1090, ScePcactActivationData); // size is from FW 3.60

/** PC activation challenge packet. */
typedef struct ScePcactChallenge {
	SceUInt8 packetType; //!< Set to 0x11.
	SceUInt8 phase; //!< Set to 1.
	ScePcactMode mode; //!< Requested challenge mode.
	SceUInt8 reserved[0xD]; //!< Set to zero.
	SceUInt8 protectedData[0x60]; //!< Protected challenge data.
	SceUInt8 authenticationTag[0x10]; //!< Packet authentication tag.
} ScePcactChallenge;
VITASDK_BUILD_ASSERT_EQ(0x80, ScePcactChallenge); // size is from FW 3.60

typedef struct ScePcactGetChallengeOpt {
	SceSize epasswordSize; //!< Number of password bytes copied; use 0x20.
	SceSize challengeSize; //!< Number of result bytes copied; maximum 0x80.
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} ScePcactGetChallengeOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, ScePcactGetChallengeOpt); // size is from FW 3.60

typedef struct SceSblGcAuthMgrPkgVryInfo {
	SceSize hashSize; //!< Number of hash bytes copied; use 0x14.
	SceSize sigSize; //!< Number of signature bytes copied; use 0x28.
	SceUInt64 reserved; //!< Ignored on FW 3.60.
} SceSblGcAuthMgrPkgVryInfo;
VITASDK_BUILD_ASSERT_EQ(0x10, SceSblGcAuthMgrPkgVryInfo); // size is from FW 3.60

/**
 * Get the type-01 game-card media ID.
 *
 * Both pointers must be non-NULL. FW 3.60 always obtains the complete 0x20-byte
 * value internally and copies its first \c pOpt->mediaIdSize bytes.
 *
 * @param[out] pMediaId - Media-ID output.
 * @param[in] pOpt - Required output size.
 *
 * @return 0 on success; 0x808A0700 for an invalid output or secure-command
 *         failure; 0x808A0701 for a semaphore failure; 0x808A0703 when no
 *         validated game-card context exists; or another negative copy error.
 */
int _sceSblGcAuthMgrGetMediaIdType01(SceMediaIdType01 *pMediaId, const SceSblGcAuthMgrGetMediaIdType01Opt *pOpt);

/**
 * Validate and install PC activation data.
 *
 * The envelope must match the currently saved challenge state. On success the
 * 0x1040-byte payload is passed to NPDRM before this function returns. The input
 * buffer is not used after this function returns. After any activation attempt
 * with the correct size, FW 3.60 clears its saved challenge state even when
 * envelope verification or NPDRM installation fails.
 *
 * @param[in] activationData - Required complete activation object.
 * @param[in] activationDataSize - Must equal 0x1090.
 *
 * @return 0 on success, or a negative validation, cryptographic, copy, or
 *         NPDRM error.
 */
int _sceSblGcAuthMgrPcactActivation(const ScePcactActivationData *activationData, SceSize activationDataSize);

/**
 * Create a PC activation challenge.
 *
 * All pointers must be non-NULL. The function uses a complete 0x20-byte password
 * block and generates a complete challenge internally; \a pOpt only controls
 * how many bytes are copied from or to the caller's buffers.
 *
 * @param[in] mode - One of ::ScePcactMode.
 * @param[in] ePassword - Required 0x20-byte password block.
 * @param[out] challenge - Challenge output.
 * @param[in] pOpt - Required copy sizes.
 *
 * @return 0 on success; 0x808A0380 for an invalid pointer or mode;
 *         0x808A0383 when mode 1 finds existing activation data;
 *         0x808A0384 when mode 0 is used before challenge expiration; or
 *         another negative RTC, cryptographic, or copy error.
 */
int _sceSblGcAuthMgrPcactGetChallenge(SceUInt32 mode, const SceUInt8 *ePassword, ScePcactChallenge *challenge, const ScePcactGetChallengeOpt *pOpt);

/**
 * Verify a package ECDSA-160 signature.
 *
 * The hash and signature pointers must be non-NULL. FW 3.60 uses a complete
 * 0x14-byte hash and 0x28-byte signature. Do not use smaller copy sizes for
 * verification: they leave the remaining bytes unspecified.
 *
 * @param[in] pHash - SHA-1 digest.
 * @param[in] pSig - Raw ECDSA-160 signature.
 * @param[in] pInfo - Required copy sizes.
 *
 * @return 0 when valid; 0x808A0008 for a semaphore failure; 0x808A000C for
 *         invalid input or a rejected signature; or another negative copy
 *         error.
 */
int _sceSblGcAuthMgrPkgVry(const SceUInt8 *pHash, const SceUInt8 *pSig, const SceSblGcAuthMgrPkgVryInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_SBLGCAUTHMGR_H_ */
