/**
 * \usergroup{SceDrmBridge}
 * \usage{psp2/drmbridge.h,SceDrmBridge_stub}
 */

#ifndef _PSP2_DRMBRIDGE_H_
#define _PSP2_DRMBRIDGE_H_

#include <vitasdk/build_utils.h>
#include <psp2/types.h>
#include <psp2common/kernel/gcauthmgr/mlnpsnl.h>
#include <psp2common/kernel/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceDrmBridgeMlnpsnlAuth1Opt {
	SceSblGcAuthMgrMlnpsnlSessionData *sessionData; //!< Destination for the phase-one session data.
	SceSize requestSize; //!< Must be 0x20; smaller copies leave provider input bytes uninitialized.
	SceSize secureTickSize; //!< Must be 8; smaller copies leave provider input bytes uninitialized.
	SceSize responseSize; //!< Number of response bytes to copy, at most 0x80.
	SceSize sessionDataSize; //!< Number of session-data bytes to copy, at most 0x40.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceDrmBridgeMlnpsnlAuth1Opt;
VITASDK_BUILD_ASSERT_EQ(0x18, SceDrmBridgeMlnpsnlAuth1Opt); // size is from FW 3.60

/**
 * Reports whether remote-play debugging is allowed.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 1 when allowed, 0 when disabled, or a negative authorization error.
 */
SceBool _sceDrmBridgeIsAllowRemotePlayDebug(void);

/**
 * Performs the first MLNPSNL authentication phase.
 *
 * The SceDriverUser wrapper supplies the maximum copy sizes and exposes this as
 * a four-buffer operation. The returned session data must be retained unchanged
 * for the second authentication phase. All pointers are required. Both output
 * structures are generated in zero-initialized kernel buffers and the requested
 * byte counts are copied back even when the authentication provider reports an
 * error; no caller pointer is retained after the function returns.
 *
 * @param[in] request - Required phase-one request. requestSize must be exactly
 *                      0x20.
 * @param[in] secureTick - Required secure tick. secureTickSize must be exactly
 *                         8.
 * @param[out] response - Required phase-one response. responseSize must be at most
 *                        0x80; the system wrapper requests the complete structure.
 * @param[in] opt - Required copy sizes and required session-data destination.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success; 0x808A0200 when an authentication buffer is NULL; or
 *         another negative copy, authorization, size, or authentication error.
 */
int _sceDrmBridgeMlnpsnlAuth1(const SceSblGcAuthMgrMlnpsnlAuth1Request *request, const SceRtcTick *secureTick, SceSblGcAuthMgrMlnpsnlAuth1Response *response, const SceDrmBridgeMlnpsnlAuth1Opt *opt);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_DRMBRIDGE_H_ */
