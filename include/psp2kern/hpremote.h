/**
 * \kernelgroup{SceHpremote}
 * \usage{psp2kern/hpremote.h,SceHpremoteForDriver_stub}
 */

#ifndef _PSP2KERN_HPREMOTE_H_
#define _PSP2KERN_HPREMOTE_H_

#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Connection state reported for devices that require asynchronous detection. */
typedef enum SceHprmDeviceState {
	SCE_HPRM_DEVICE_STATE_PENDING       = -1, //!< Detection is still pending.
	SCE_HPRM_DEVICE_STATE_NOT_CONNECTED = 0,  //!< The device is not connected.
	SCE_HPRM_DEVICE_STATE_CONNECTED     = 1   //!< The device is connected.
} SceHprmDeviceState;

/** Connected-device bits delivered to callbacks registered with ::ksceHprmRegisterCallback. */
typedef enum SceHprmConnectedDevice {
	SCE_HPRM_CONNECTED_DEVICE_HEADPHONE  = 0x1,
	SCE_HPRM_CONNECTED_DEVICE_REMOTE     = 0x2,
	SCE_HPRM_CONNECTED_DEVICE_MICROPHONE = 0x4
} SceHprmConnectedDevice;

/**
 * Direct connection callback installed by ::ksceHprmSetConnectCallback.
 *
 * On FW 3.60 this callback runs from the vblank subinterrupt handler. Its
 * return value is ignored. The callback is invoked synchronously and must use
 * only operations that are safe from interrupt context.
 *
 * @param[in] headphone_detect - Debounced Syscon headphone-detect signal.
 * @param[in] audio_out_dock_exists - Whether the multiconnector is in its
 *                                    audio-output mode.
 * @param[in] headphone_exists - Published headphone connection state.
 * @param[in] remote_state - Remote connection state.
 * @param[in] microphone_state - Headset-microphone connection state.
 * @param[in] argp - Argument supplied to ::ksceHprmSetConnectCallback.
 */
typedef int (*SceHprmConnectCallback)(SceBool headphone_detect,
	SceBool audio_out_dock_exists, SceBool headphone_exists,
	SceHprmDeviceState remote_state, SceHprmDeviceState microphone_state,
	void *argp);

/**
 * SceHpremote module state on FW 3.60.
 *
 * ::ksceHprmGetInternalState returns the address of this live module storage.
 * It is updated by the detection thread, Syscon callbacks, the vblank
 * subinterrupt handler, and the system-event handler. The pointer remains
 * valid for the lifetime of the module and must not be freed. Reads and writes
 * directly access the module's state and can race with those execution
 * contexts.
 *
 * FW 3.60 initializes all 32 callback slots and examines all 32 when sending
 * notifications, although ::ksceHprmRegisterCallback and
 * ::ksceHprmUnregisterCallback manage only the first 16 slots.
 *
 * The detection-behavior flags are derived from bits 16 through 23 of the
 * Syscon hardware information. FW 3.60 selects 3 below 0x80, 0 from 0x80
 * through 0x81, and 5 from 0x82 onward. Mask 0x2 enables explicit Syscon
 * HP-remote power control, mask 0x4 selects alternate disconnect debounce and
 * settle timing, and mask 0x1 retains an additional no-microphone detection
 * step.
 *
 * The detection state machine uses the following states on FW 3.60:
 *
 * - 0 performs the shutdown cleanup requested by ::ksceHprmShutdown.
 * - 1 is a cleanup transition for which the module has no internal entry path.
 * - 2 performs disconnect cleanup before waiting for another headset.
 * - 3 waits in the inactive state entered after shutdown cleanup.
 * - 4 waits for the Syscon headphone-connect signal.
 * - 5 disables Syscon headset-remote power, when required by the hardware,
 *   before restarting a probe.
 * - 6 waits before enabling, or retries enabling, Syscon headset-remote power.
 * - 7 waits for the Syscon microphone-connect signal.
 * - 8 enables the codec microphone-detection circuit.
 * - 9 samples the codec microphone-detection result for the first time.
 * - 10 performs the optional delayed retry after an undetected microphone.
 * - 11 samples the codec microphone-detection result a second time.
 * - 12 is the steady state that waits for disconnect or reset events.
 */
typedef struct SceHprmInternalState {
	SceUID eventFlagId; //!< Event flag used by the detection state machine.
	SceUID detectionThreadId; //!< Headset-detection worker thread.
	SceInt32 detectionState; //!< Detection state-machine index described above.
	SceUID callbackIds[32]; //!< Kernel callback UIDs, or a negative value for an unused slot.
	SceUInt8 hardwareFlags; //!< Model-specific headset-detection behavior flags.
	SceUInt8 headphoneDetect; //!< Debounced Syscon headphone-detect signal, 0 or 1.
	SceUInt8 sysconMicrophoneConnect; //!< Latest Syscon microphone-connect signal, 0 or 1.
	SceUInt8 pendingHeadphoneDetect; //!< Latest raw headphone-detect signal awaiting debounce.
	SceUInt32 headphoneDebounceHistory; //!< Shift register of recent headphone-detect samples.
	SceUInt8 interfaceConnect; //!< Debounced Syscon interface-connect signal, 0 or 1.
	SceUInt8 pendingInterfaceConnect; //!< Latest raw interface-connect signal awaiting debounce.
	SceUInt8 audioOutDockExists; //!< Whether the multiconnector is in audio-output mode.
	SceUInt8 reserved0; //!< Set to 0 during initialization and otherwise unused on FW 3.60.
	SceUInt32 interfaceDebounceHistory; //!< Shift register of recent interface-connect samples.
	SceUInt8 reserved1[4]; //!< Set to 0 during initialization and otherwise unused on FW 3.60.
	SceUInt8 notificationPending; //!< Whether connection callbacks need to be notified.
	SceInt8 headphoneExists; //!< Published headphone connection state, 0 or 1.
	SceInt8 remoteState; //!< One of ::SceHprmDeviceState; the FW 3.60 module only sets -1 or 0.
	SceInt8 microphoneState; //!< One of ::SceHprmDeviceState.
	SceHprmConnectCallback connectCallback; //!< Direct connection callback, or NULL.
	void *connectCallbackArg; //!< Caller-owned argument passed to the direct connection callback.
} SceHprmInternalState;
VITASDK_BUILD_ASSERT_EQ(0xAC, SceHprmInternalState); // size is from FW 3.60

/**
 * Get the debounced Syscon headphone-detect signal.
 *
 * @return SCE_TRUE when the signal is asserted, otherwise SCE_FALSE.
 */
SceBool ksceHprmGetHpDetect(void);

/**
 * Get the module's internal state.
 *
 * The returned pointer addresses live module storage. It remains valid for the
 * lifetime of the module and must not be freed. This function does not
 * synchronize access to it.
 *
 * @param[out] state - Optional pointer that receives the module's internal
 *                     state address.
 *
 * @return The current FW 3.60 detection-state-machine index, from 0 through 12.
 */
int ksceHprmGetInternalState(SceHprmInternalState **state);

/**
 * Check whether an audio-output dock is connected to the multiconnector.
 *
 * On FW 3.60 this is true when bits 8 through 15 of the Syscon multiconnector
 * information equal 0x03.
 *
 * @return SCE_TRUE when an audio-output dock is connected, otherwise SCE_FALSE.
 */
SceBool ksceHprmIsAudioOutDockExist(void);

/**
 * Check whether headphones are connected.
 *
 * @return SCE_TRUE when headphones are connected, otherwise SCE_FALSE.
 */
SceBool ksceHprmIsHeadphoneExist(void);

/**
 * Check whether a headset microphone is connected.
 *
 * The FW 3.60 implementation reports SCE_FALSE while detection is pending.
 *
 * @return SCE_TRUE when a headset microphone is connected, otherwise
 *         SCE_FALSE.
 */
SceBool ksceHprmIsMicrophoneExist(void);

/**
 * Check whether a headset remote is connected.
 *
 * The FW 3.60 implementation reports SCE_FALSE while detection is pending.
 * Without external modification of the live internal state, FW 3.60 never
 * assigns ::SCE_HPRM_DEVICE_STATE_CONNECTED to the remote state and this
 * function therefore returns SCE_FALSE.
 *
 * @return SCE_TRUE when a headset remote is connected, otherwise SCE_FALSE.
 */
SceBool ksceHprmIsRemoteExist(void);

/**
 * Register a kernel callback UID for connection-state notifications.
 *
 * The callback's notification argument is the current connected-device mask,
 * formed by bitwise OR of ::SceHprmConnectedDevice values. The FW 3.60
 * implementation has 16 registration slots. It does not validate the UID or
 * reject duplicate registrations. The caller retains ownership of the callback
 * object and must unregister it before deleting it.
 *
 * The vblank subinterrupt handler calls ::ksceKernelNotifyCallback; execution
 * of the registered callback follows the kernel callback object's normal
 * scheduling rules and does not occur synchronously in the subinterrupt.
 *
 * @param[in] callback_id - Kernel callback UID.
 *
 * @return 0 on success, or -1 when every registration slot is occupied.
 */
int ksceHprmRegisterCallback(SceUID callback_id);

/**
 * Request a new headset connection probe.
 *
 * This function posts an asynchronous reset event. It also resumes probing
 * after ::ksceHprmShutdown.
 *
 * @return 0.
 */
int ksceHprmReset(void);

/**
 * Install the direct connection callback.
 *
 * The module supports one direct callback. Installing another callback replaces
 * the previous callback and argument. Pass NULL as \a callback to disable it.
 * The callback and argument are retained by address rather than copied, and no
 * immediate notification is generated when they are installed.
 *
 * @param[in] callback - Callback, or NULL.
 * @param[in] argp - Opaque argument passed to \a callback.
 *
 * @return 0.
 */
int ksceHprmSetConnectCallback(SceHprmConnectCallback callback, void *argp);

/**
 * Suspend headset probing.
 *
 * This function posts an asynchronous shutdown event. The detection thread
 * disables codec microphone detection and, on hardware that requires it,
 * Syscon headset-remote power. The thread then remains alive in an inactive
 * wait state. ::ksceHprmReset resumes probing.
 *
 * @return 0.
 */
int ksceHprmShutdown(void);

/**
 * Unregister a kernel callback UID.
 *
 * If the same UID was registered more than once, this function removes only
 * the first matching registration.
 *
 * @param[in] callback_id - Kernel callback UID previously passed to
 *                          ::ksceHprmRegisterCallback.
 *
 * @return 0 on success, or -1 when \a callback_id is not registered.
 */
int ksceHprmUnregisterCallback(SceUID callback_id);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_HPREMOTE_H_ */
