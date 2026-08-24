/**
 * \usergroup{SceLibKernel}
 * \usage{psp2/kernel/libkernel.h,SceLibKernel_stub}
 */

#ifndef _PSP2_KERNEL_LIBKERNEL_H_
#define _PSP2_KERNEL_LIBKERNEL_H_

#include <psp2/types.h>
#include <psp2/kernel/ssp.h>
#include <psp2/sblgcauthmgr.h>
#include <psp2common/kernel/backtrace.h>
#include <psp2common/kernel/threadmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Terminates execution after a stack-protector check fails. */
__attribute__((__noreturn__))
void __stack_chk_fail(void);

/**
 * Gets a thread backtrace.
 *
 * @param[in] threadId Thread ID, or ::SCE_KERNEL_BACKTRACE_CONTEXT_CURRENT.
 * @param[out] pCallFrameBuffer Buffer that receives the call frames, or NULL
 *                              when \a numBytesBuffer is 0.
 * @param[in] numBytesBuffer Size of the call-frame buffer in bytes. A non-NULL
 *                           buffer must hold at least one frame.
 * @param[out] pNumReturn Optional pointer that receives the number of frames
 *                        written to the output buffer.
 * @param[in] mode Bitwise OR of ::SceKernelBacktraceMode values.
 *
 * @return With ::SCE_KERNEL_BACKTRACE_MODE_DONT_EXCEED, 0 on success.
 * Otherwise, the complete call-stack depth on success, even when the output
 * buffer is too small to contain every frame. Returns < 0 on error.
 */
SceInt32 sceKernelBacktrace(SceUID threadId, SceKernelCallFrame *pCallFrameBuffer, SceSize numBytesBuffer, SceUInt32 *pNumReturn, SceInt32 mode);

/**
 * Gets a backtrace of the calling thread.
 *
 * @param[out] pCallFrameBuffer Buffer that receives the call frames, or NULL
 *                              when \a numBytesBuffer is 0.
 * @param[in] numBytesBuffer Size of the call-frame buffer in bytes. A non-NULL
 *                           buffer must hold at least one frame.
 * @param[out] pNumReturn Optional pointer that receives the number of frames
 *                        written to the output buffer.
 * @param[in] mode Bitwise OR of ::SceKernelBacktraceMode values.
 *
 * @return With ::SCE_KERNEL_BACKTRACE_MODE_DONT_EXCEED, 0 on success.
 * Otherwise, the complete call-stack depth on success, even when the output
 * buffer is too small to contain every frame. Returns < 0 on error.
 */
SceInt32 sceKernelBacktraceSelf(SceKernelCallFrame *pCallFrameBuffer, SceSize numBytesBuffer, SceUInt32 *pNumReturn, SceInt32 mode);

/**
 * Disarms a timer event and cancels all threads waiting on the timer.
 *
 * @param[in] timerId Timer identifier.
 * @param[out] numWaitThreads Optional pointer that receives the number of
 * canceled waiters.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelCancelTimer(SceUID timerId, SceUInt32 *numWaitThreads);

/**
 * Gets a timer's reference process time.
 *
 * @param[in] timerId Timer identifier.
 * @param[out] baseTime Required pointer that receives 0 while the timer is
 * stopped, or its reference process time with ::SceKernelSysClock semantics
 * while it is active.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelGetTimerBase(SceUID timerId, SceKernelSysClock *baseTime);

/**
 * Gets the remaining duration before an armed timer event.
 *
 * @param[in] timerId Timer identifier.
 * @param[out] remainingTime Required pointer that receives a value with
 * ::SceKernelSysClock semantics.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelGetTimerEventRemainingTime(SceUID timerId, SceKernelSysClock *remainingTime);

/**
 * Gets size-prefixed timer information.
 *
 * @param[in] timerId Timer identifier.
 * @param[in,out] pInfo Required pointer to a ::SceKernelTimerInfo structure.
 * Its leading size field selects the exact number of bytes copied to and from
 * the structure, up to the complete 0x60-byte FW 3.60 layout.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelGetTimerInfo(SceUID timerId, SceKernelTimerInfo *pInfo);

/**
 * Gets the current timer time.
 *
 * @param[in] timerId Timer identifier.
 * @param[out] timerTime Required pointer that receives a value with
 * ::SceKernelSysClock semantics.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelGetTimerTime(SceUID timerId, SceKernelSysClock *timerTime);

/**
 * Prints call frames for the current process.
 *
 * @param[in] pCallFrame Required call frames to print, even when
 *                       \a numFrames is 0.
 * @param[in] numFrames Number of entries in pCallFrame.
 *
 * @return 0 on success, or < 0 on error.
 */
SceInt32 sceKernelPrintBacktrace(const SceKernelCallFrame *pCallFrame, SceUInt32 numFrames);

/**
 * Arms a timer event after a relative interval.
 *
 * @param[in] timerId Timer identifier.
 * @param[in] type Notification behavior selected from ::SceKernelTimerType.
 * @param[in] interval Required relative interval with ::SceKernelSysClock
 * semantics.
 * @param[in] repeat Nonzero to rearm the event periodically.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelSetTimerEvent(SceUID timerId, SceKernelTimerType type, const SceKernelSysClock *interval, SceBool repeat);

/**
 * Sets the current timer time.
 *
 * @param[in] timerId Timer identifier.
 * @param[in,out] timerTime Required pointer containing the new timer time with
 * ::SceKernelSysClock semantics. For callers targeting SDK version 2.00 or
 * newer, it receives the previous timer time on return; older target SDK
 * versions do not receive that value.
 *
 * @return 0 on success, or < 0 on error.
 */
int sceKernelSetTimerTime(SceUID timerId, SceKernelSysClock *timerTime);

/**
 * Get the 0x20-byte type 01 media ID produced by the game-card authentication state.
 *
 * @param[out] pMediaId - Output media ID.
 *
 * @return 0 on success, < 0 on error.
 */
int sceSblGcAuthMgrGetMediaIdType01(SceMediaIdType01 *pMediaId);

/**
 * Install PC activation data.
 *
 * The key envelope is verified against the saved challenge state before the
 * trailing 0x1040-byte activation payload is written.
 *
 * @param[in] act_data - Pointer to a 0x1090-byte activation object.
 * @param[in] act_data_size - Must be 0x1090.
 *
 * @return 0 on success, < 0 on error.
 */
int sceSblGcAuthMgrPcactActivation(const ScePcactActivationData *act_data, SceSize act_data_size);

/**
 * Create a PC activation challenge.
 *
 * @param[in] mode - Challenge mode. Mode 0 is rejected until the previously
 *                   recorded expiration tick has elapsed. Mode 1 requires
 *                   that no readable activation data exists and records a
 *                   new expiration. Mode 2 bypasses the expiration check.
 * @param[in] epassword - Pointer to a 0x20-byte input.
 * @param[out] challenge - Pointer to a 0x80-byte output.
 *
 * @return 0 on success, < 0 on error.
 */
int sceSblGcAuthMgrPcactGetChallenge(SceUInt32 mode, const SceUInt8 *epassword, ScePcactChallenge *challenge);

/**
 * Verify a package ECDSA-160 signature.
 *
 * @param[in] pHash - Pointer to a 0x14-byte SHA-1 digest.
 * @param[in] pSig - Pointer to a 0x28-byte ECDSA signature.
 *
 * @return 0 if the signature is valid, < 0 on error.
 */
int sceSblGcAuthMgrPkgVry(const SceUInt8 *pHash, const SceUInt8 *pSig);

/**
 * Waits until any requested event bit is present.
 *
 * When the event has the auto-clear attribute, the matched bits are removed
 * after a successful wait. This function does not dispatch callbacks while
 * blocked.
 *
 * @param[in] eventId Event identifier.
 * @param[in] waitPattern Pattern of bits to wait for.
 * @param[out] pResultPattern Optional pointer that receives the matched bits.
 * @param[out] pUserData Optional pointer that receives the event's user data.
 * @param[in,out] pTimeout Optional timeout in microseconds. If the call
 * blocks, it receives the remaining duration; it receives 0 when the timeout
 * expires. It is left unchanged when the wait is satisfied immediately.
 *
 * @return 0 on success, or < 0 on error.
 */
SceInt32 sceKernelWaitEvent(SceUID eventId, SceUInt32 waitPattern, SceUInt32 *pResultPattern, SceUInt64 *pUserData, SceUInt32 *pTimeout);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_KERNEL_LIBKERNEL_H_ */
