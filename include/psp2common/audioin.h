/**
 * \usergroup{SceAudioIn}
 * \usage{psp2common/audioin.h}
 */

#ifndef _PSP2COMMON_AUDIOIN_H_
#define _PSP2COMMON_AUDIOIN_H_

#include <psp2common/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SceAudioInErrorCode {
	SCE_AUDIO_IN_ERROR_FATAL                = 0x80260100, //!< AudioIn is not initialized.
	SCE_AUDIO_IN_ERROR_INVALID_PORT         = 0x80260101, //!< Invalid port handle.
	SCE_AUDIO_IN_ERROR_INVALID_SIZE         = 0x80260102, //!< Invalid grain size.
	SCE_AUDIO_IN_ERROR_INVALID_SAMPLE_FREQ  = 0x80260103, //!< Invalid sample frequency.
	SCE_AUDIO_IN_ERROR_INVALID_PORT_TYPE    = 0x80260104, //!< Invalid port type.
	SCE_AUDIO_IN_ERROR_INVALID_POINTER      = 0x80260105, //!< Invalid pointer.
	SCE_AUDIO_IN_ERROR_INVALID_PORT_PARAM   = 0x80260106, //!< Invalid port parameter.
	SCE_AUDIO_IN_ERROR_PORT_FULL            = 0x80260107, //!< The process already owns a port on FW 3.60.
	SCE_AUDIO_IN_ERROR_OUT_OF_MEMORY        = 0x80260108, //!< All four port slots are in use on FW 3.60.
	SCE_AUDIO_IN_ERROR_NOT_OPENED           = 0x80260109, //!< The port is not open.
	SCE_AUDIO_IN_ERROR_BUSY                 = 0x8026010A, //!< An input operation is already in progress.
	SCE_AUDIO_IN_ERROR_INVALID_PARAMETER    = 0x8026010B  //!< Invalid parameter.
} SceAudioInErrorCode;

typedef enum SceAudioInPortType {
	SCE_AUDIO_IN_PORT_TYPE_VOICE  = 0,    //!< Voice capture profile.
	SCE_AUDIO_IN_PORT_TYPE_RAW    = 2,    //!< Raw capture profile.
	SCE_AUDIO_IN_PORT_TYPE_DIAG   = 14,   //!< Diagnostic profile accepted by ::sceAudioInOpenPortForDiag.
	SCE_AUDIO_IN_PORT_TYPE_CAMERA = 0x20  //!< Camera capture profile used by ScePhotoCam on FW 3.60.
} SceAudioInPortType;

typedef enum SceAudioInParam {
	SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO = 0,
	SCE_AUDIO_IN_GETSTATUS_MUTE        = 1
} SceAudioInParam;

/** Commands accepted by ::sceAudioInSetMute. */
typedef enum SceAudioInMuteCommand {
	SCE_AUDIO_IN_MUTE   = 1,
	SCE_AUDIO_IN_UNMUTE = 2
} SceAudioInMuteCommand;

/** Audio-input routes used by ::sceAudioInSelectInput and ::ksceAudioInSelectInput. */
typedef enum SceAudioInInputMode {
	SCE_AUDIO_IN_INPUT_MODE_NONE      = -1,   //!< No active input route.
	SCE_AUDIO_IN_INPUT_MODE_CODEC_0   = 0,    //!< External-codec input profile 0.
	SCE_AUDIO_IN_INPUT_MODE_CODEC_1   = 1,    //!< External-codec input profile 1.
	SCE_AUDIO_IN_INPUT_MODE_CODEC_2   = 2,    //!< External-codec input profile 2.
	SCE_AUDIO_IN_INPUT_MODE_BLUETOOTH = 0x10, //!< Bluetooth microphone PCM supplied by ::ksceAudioInSubmitBluetoothPcm.
	SCE_AUDIO_IN_INPUT_MODE_VITA_TV   = 0x20  //!< Vita TV input profile.
} SceAudioInInputMode;

#ifdef __cplusplus
}
#endif

#endif /* _PSP2COMMON_AUDIOIN_H_ */
