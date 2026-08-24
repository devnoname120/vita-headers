/**
 * \kernelgroup{SceCodec}
 * \usage{psp2kern/codec.h,SceCodecForDriver_stub}
 *
 * On handheld FW 3.60 systems, this library controls the external WM audio
 * codec. On Vita TV, no backend is installed: these exports return 0 and
 * output pointers are left untouched.
 */

#ifndef _PSP2KERN_CODEC_H_
#define _PSP2KERN_CODEC_H_

#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Complete output-route configurations used by SceAVConfig on FW 3.60.
 *
 * These entries identify complete endpoint configurations, not individual
 * endpoint flags. The WM backend independently decodes mask bits 0x0100,
 * 0x0400, and 0x1000; all other bits are ignored. Other combinations of the
 * three decoded bits are accepted but are not used by FW 3.60 SceAVConfig.
 */
typedef enum SceCodecOutputRoute {
	SCE_CODEC_OUTPUT_ROUTE_NONE      = 0x0000, //!< Disable all local codec outputs.
	SCE_CODEC_OUTPUT_ROUTE_HEADPHONE = 0x0400, //!< Route output to connected headphones.
	SCE_CODEC_OUTPUT_ROUTE_DOCK      = 0x0500, //!< Route output to the multiconnector audio-output dock.
	SCE_CODEC_OUTPUT_ROUTE_SPEAKER   = 0x1100  //!< Route output to the built-in speaker.
} SceCodecOutputRoute;

/**
 * External-codec audio-output profiles used by SceAudio.
 *
 * The FW 3.60 WM backend examines only bits 8 through 15. Any value whose
 * high byte is neither 0x01 nor 0x02 leaves the current profile unchanged.
 */
typedef enum SceCodecAudioOutMode {
	SCE_CODEC_AUDIO_OUT_MODE_COMPRESSED = 0x0100, //!< Compressed SceAudio output profile.
	SCE_CODEC_AUDIO_OUT_MODE_NORMAL     = 0x0200  //!< Normal SceAudio output profile.
} SceCodecAudioOutMode;

/**
 * External-codec audio-input profiles used by SceAudioIn.
 */
typedef enum SceCodecAudioInputMode {
	SCE_CODEC_AUDIO_INPUT_MODE_INACTIVE    = -1, //!< Inactive WM register profile.
	SCE_CODEC_AUDIO_INPUT_MODE_BUILTIN_MIC = 0,  //!< Handheld built-in microphone.
	SCE_CODEC_AUDIO_INPUT_MODE_HEADSET_MIC = 1,  //!< Wired headset microphone.
	SCE_CODEC_AUDIO_INPUT_MODE_UNK_2       = 2   //!< WM profile not used by a first-party FW 3.60 caller.
} SceCodecAudioInputMode;

/**
 * Read a raw register from the external audio codec.
 *
 * The FW 3.60 WM backend sends the low 16 bits of the address as a big-endian
 * word to I2C bus 0, device address 0x34. This raw entry point does not acquire
 * the codec mutex and therefore must not race another codec operation.
 *
 * @param[in] reg - Register address; only the low 16 bits are used.
 *
 * @return The 16-bit register value on success, or a negative error code.
 */
int ksceCodecReadRegister(SceUInt32 reg);

/**
 * Write a raw register in the external audio codec.
 *
 * The FW 3.60 WM backend sends the address and value as big-endian 16-bit
 * words to I2C bus 0, device address 0x34. It waits for a preceding special
 * register write to complete and retries a failed transfer up to three times.
 * This raw entry point does not acquire the codec mutex and therefore must not
 * race another codec operation.
 *
 * @param[in] reg - Register address; only the low 16 bits are used.
 * @param[in] value - Register value; only the low 16 bits are used.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceCodecWriteRegister(SceUInt32 reg, SceUInt32 value);

/**
 * Get the current external-codec output state.
 *
 * @param[out] speaker_gain_db - Optional pointer that receives the cached
 *                               speaker gain in dB. Receives -121 while the
 *                               speaker route is inactive on FW 3.60.
 * @param[out] headphone_gain_db - Optional pointer that receives the cached
 *                                 headphone gain in dB.
 *
 * @return The current absolute output-route mask composed of bits 0x0100,
 *         0x0400, and 0x1000; known endpoint configurations are listed in
 *         ::SceCodecOutputRoute. Returns a negative mutex error on failure.
 */
int ksceCodecGetOutputState(SceInt32 *speaker_gain_db, SceInt32 *headphone_gain_db);

/**
 * Set the external-codec output route.
 *
 * The route is an absolute mask. The FW 3.60 WM backend independently enables
 * or disables the paths selected by bits 0x0100, 0x0400, and 0x1000 and
 * ignores all other bits. Known complete endpoint configurations are listed
 * in ::SceCodecOutputRoute.
 *
 * @param[in] output_mask - New output-route mask.
 *
 * @return 0 on success, or a nonzero error code.
 */
int ksceCodecSetOutputRoute(SceUInt32 output_mask);

/**
 * Set the external-codec playback volume.
 *
 * @param[in] volume_level - Volume level from 0 through 30. Level 0 is
 *                           silence on FW 3.60.
 *
 * @return 0 on success, 0x80261000 when \a volume_level is greater than 30,
 *         a negative mutex or I2C error for a headphone-path failure, or 1
 *         for a speaker-path lock or programming failure on the FW 3.60 WM
 *         backend.
 */
int ksceCodecSetVolume(SceUInt32 volume_level);

/**
 * Select the external-codec audio-input configuration used by SceAudioIn.
 *
 * FW 3.60 SceAVConfig selects the built-in-microphone profile at handheld
 * initialization and changes to the headset-microphone profile when a wired
 * headset microphone is selected. The WM backend also implements mode 2, but
 * no first-party FW 3.60 caller was found that selects it. Other signed values
 * return 0x80261000.
 *
 * @param[in] mode - Audio-input profile.
 *
 * @return 1 after successfully selecting mode 1, 0 after successfully
 *         selecting mode -1, 0, or 2, or a negative error code.
 */
int ksceCodecSetAudioInputMode(SceCodecAudioInputMode mode);

/**
 * Select the external-codec audio-output mode used by SceAudio.
 *
 * If the speaker path is active, changing profiles temporarily disables that
 * path, reconfigures the codec, and restores it. On the FW 3.60 WM backend a
 * compressed-profile request made while the speaker path is inactive is not
 * retained.
 *
 * @param[in] mode - One of ::SceCodecAudioOutMode.
 *
 * @return 0 on success, or a nonzero error code.
 */
int ksceCodecSetAudioOutMode(SceCodecAudioOutMode mode);

/**
 * Get the headset-microphone detection state.
 *
 * The FW 3.60 WM backend returns codec register 0x30 bit 7. After microphone
 * detection power is enabled, SceHpremote interprets 0 as microphone present
 * and every nonzero result as microphone absent.
 *
 * @return 0 or 0x80 on success, or a negative error code.
 */
int ksceCodecGetMicrophoneDetectStatus(void);

/**
 * Enable or disable the codec circuitry used for headset-microphone detection.
 *
 * @param[in] enable - A nonzero value enables detection power; zero disables
 *                     it.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceCodecSetMicrophoneDetectPower(SceBool enable);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_CODEC_H_ */
