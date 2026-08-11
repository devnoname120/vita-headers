/**
 * \kernelgroup{SceCodec}
 * \usage{psp2kern/codec.h,SceCodecForDriver_stub}
 */

#ifndef _PSP2KERN_CODEC_H_
#define _PSP2KERN_CODEC_H_

#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Output-route configurations observed on FW 3.60.
 *
 * These values are complete route masks, not independent flags.
 */
typedef enum SceCodecOutputRoute {
	SCE_CODEC_OUTPUT_ROUTE_NONE      = 0x0000,
	SCE_CODEC_OUTPUT_ROUTE_HEADPHONE = 0x0400,
	SCE_CODEC_OUTPUT_ROUTE_DOCK      = 0x0500,
	SCE_CODEC_OUTPUT_ROUTE_SPEAKER   = 0x1100
} SceCodecOutputRoute;

/**
 * External-codec audio-output modes used by SceAudio.
 */
typedef enum SceCodecAudioOutMode {
	SCE_CODEC_AUDIO_OUT_MODE_COMPRESSED = 0x0100,
	SCE_CODEC_AUDIO_OUT_MODE_NORMAL     = 0x0200
} SceCodecAudioOutMode;

/**
 * Read a raw register from the external audio codec.
 *
 * @param[in] reg - Register address. The FW 3.60 WM backend uses the low
 *                  16 bits.
 *
 * @return The 16-bit register value on success, or a negative error code.
 */
int ksceCodecReadRegister(SceUInt32 reg);

/**
 * Write a raw register in the external audio codec.
 *
 * @param[in] reg - Register address. The FW 3.60 WM backend uses the low
 *                  16 bits.
 * @param[in] value - Register value. The FW 3.60 WM backend uses the low
 *                    16 bits.
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
 * @return The current output-route mask; values for known endpoint
 *         configurations are listed in ::SceCodecOutputRoute. Returns a
 *         negative error code on failure.
 */
int ksceCodecGetOutputState(SceInt32 *speaker_gain_db, SceInt32 *headphone_gain_db);

/**
 * Set the external-codec output route.
 *
 * The route is an absolute mask. Known endpoint configurations are listed in
 * ::SceCodecOutputRoute.
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
 * @return 0 on success. The FW 3.60 WM backend returns 1 or a negative error
 *         code on failure.
 */
int ksceCodecSetVolume(SceUInt32 volume_level);

/**
 * Select the external-codec audio-input configuration used by SceAudioIn.
 *
 * Handheld FW 3.60 uses modes 0, 1, and 2. The WM backend also accepts -1 for
 * its inactive register profile. The purposes of modes 0 through 2 are
 * unknown.
 *
 * @param[in] mode - Audio-input mode.
 *
 * @return A non-negative mode-specific result on success, or a negative error
 *         code.
 */
int ksceCodecSetAudioInputMode(SceInt32 mode);

/**
 * Select the external-codec audio-output mode used by SceAudio.
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
 * detection power is enabled, SceHpremote interprets 0 as microphone present.
 *
 * @return 0 or 0x80 on success, or a negative error code.
 */
int ksceCodecGetMicrophoneDetectStatus(void);

/**
 * Enable or disable the codec circuitry used for headset-microphone detection.
 *
 * @param[in] enable - SCE_TRUE to enable detection power, SCE_FALSE to disable
 *                     it.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceCodecSetMicrophoneDetectPower(SceBool enable);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_CODEC_H_ */
