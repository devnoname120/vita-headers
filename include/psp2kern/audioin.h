/**
 * \kernelgroup{SceAudioIn}
 * \usage{psp2kern/audioin.h,SceAudioInForDriver_stub}
 */

#ifndef _PSP2KERN_AUDIOIN_H_
#define _PSP2KERN_AUDIOIN_H_

#include <psp2kern/types.h>
#include <psp2common/audioin.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Callback invoked after an audio-input port is opened or released.
 *
 * @param[in] processId - Owner PID for an opened port, or former owner PID for
 *                        a released port.
 */
typedef void (*SceAudioInPortCallback)(ScePID processId);

/**
 * Open an audio-input port for the calling process.
 *
 * @param[in] portType - Capture profile. See ::sceAudioInOpenPort.
 * @param[in] grain - Number of mono samples returned by each input call.
 * @param[in] freq - Sample frequency in Hz.
 * @param[in] param - Must be ::SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO on FW 3.60.
 *
 * @return A port handle on success, or a negative error code. Most validation
 *         failures use ::SceAudioInErrorCode.
 */
int ksceAudioInOpenPort(SceAudioInPortType portType, int grain, int freq, SceAudioInParam param);

/**
 * Release an audio-input port owned by the calling process.
 *
 * @param[in] port - Port handle returned by ::ksceAudioInOpenPort.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceAudioInReleasePort(int port);

/**
 * Capture one grain of mono signed 16-bit PCM.
 *
 * @param[in] port - Port handle returned by ::ksceAudioInOpenPort.
 * @param[out] destPtr - Kernel buffer for the captured samples. Its size must
 *                       be at least <code>grain * sizeof(SceInt16)</code> bytes.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceAudioInInput(int port, void *destPtr);

/**
 * Check whether any process owns an audio-input port.
 *
 * @return SCE_TRUE when at least one port is open, otherwise SCE_FALSE.
 */
SceBool ksceAudioInIsAnyPortOpen(void);

/**
 * Check whether a process owns an audio-input port.
 *
 * @param[in] processId - Valid nonzero process ID. Passing 0 also matches an
 *                        unused FW 3.60 port slot.
 *
 * @return SCE_TRUE when the process owns a port, otherwise SCE_FALSE.
 */
SceBool ksceAudioInIsProcessPortOwner(ScePID processId);

/**
 * Select the process whose audio-input port is adopted.
 *
 * On FW 3.60 \a unused1 and \a unused2 are ignored. A nonzero
 * \a transitionDelay temporarily suppresses the selected process's input. The
 * input worker subtracts 16 from this value per processing cycle. If the value
 * reaches exactly zero while route-change reports remain, the worker holds it
 * at 16 until both reports have been consumed. Values that are not positive
 * multiples of 16 can cross below zero and end suppression without that hold.
 * AppMgr always passes zero on FW 3.60.
 *
 * @param[in] processId - Process to adopt.
 * @param[in] unused1 - Ignored on FW 3.60.
 * @param[in] unused2 - Ignored on FW 3.60.
 * @param[in] transitionDelay - Input-suppression countdown, or 0.
 *
 * @return 0.
 */
int ksceAudioInSetAdoptForPid(ScePID processId, int unused1, int unused2, int transitionDelay);

/**
 * Register or clear the port-open callback.
 *
 * The callback runs synchronously after a port opens successfully and receives
 * its owner PID. AudioIn releases its internal mutex before invoking it and
 * returns the new handle after the callback completes.
 *
 * @param[in] callback - Callback, or NULL to clear it.
 *
 * @return 0.
 */
int ksceAudioInSetPortOpenCallback(SceAudioInPortCallback callback);

/**
 * Register or clear the port-release callback.
 *
 * The callback runs synchronously when a port is released, including during
 * process exit or kill cleanup, and receives its former owner PID. It runs
 * while AudioIn holds its internal mutex. The callback must not call an AudioIn
 * operation that locks the same mutex, such as opening, releasing, or reading
 * a port.
 *
 * @param[in] callback - Callback, or NULL to clear it.
 *
 * @return 0.
 */
int ksceAudioInSetPortReleaseCallback(SceAudioInPortCallback callback);

/**
 * Select the input route.
 *
 * The request is applied asynchronously by the input worker. FW 3.60 does not
 * reject other integer values: the worker records the requested value as the
 * current route but performs no explicit backend selection for it.
 *
 * @param[in] inputMode - Input route.
 *
 * @return 0 when the route is already selected, otherwise the result of the
 *         FW 3.60 input-worker notification.
 */
int ksceAudioInSelectInput(SceAudioInInputMode inputMode);

/**
 * Submit Bluetooth microphone PCM to AudioIn.
 *
 * AVConfig uses this function after receiving Bluetooth audio. The samples are
 * consumed only while the Bluetooth input backend is active. FW 3.60 stages
 * them in a 360-sample ring buffer and advances the expected producer position
 * by 120 samples per submission. It inserts silence when the producer is more
 * than 120 samples behind and trims input when the producer is more than 120
 * samples ahead. The worker upsamples the submitted stream by two using
 * midpoint interpolation.
 *
 * @param[in] samples - Kernel-resident mono signed 16-bit PCM samples. This may
 *                      be NULL only when \a sampleCount is zero.
 * @param[in] sampleCount - Number of 16-bit samples, not bytes.
 *
 * @return The input-worker notification result on success, -1 when the
 *         Bluetooth backend is not active, or a negative mutex error.
 */
int ksceAudioInSubmitBluetoothPcm(const SceInt16 *samples, SceSize sampleCount);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_AUDIOIN_H_ */
