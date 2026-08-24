/**
 * \usergroup{SceNgs}
 * \usage{psp2/ngs.h,SceNgsUser_stub}
 */

#ifndef _PSP2_NGS_H_
#define _PSP2_NGS_H_

#include <psp2/ngs_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_NGS_MAX_SYSTEM_CHANNELS 2

/**
 * Opaque immutable voice definition.
 *
 * FW 3.60 represents a definition as a pointer-typed encoded token. Do not
 * dereference it; store it in ::SceNgsRackDescription::voiceDefinition.
 */
typedef struct SceNgsVoiceDefinition SceNgsVoiceDefinition;

/** Buffer owned by the caller and used by NGS in place. */
typedef struct SceNgsBufferInfo {
	void *data;   //!< Buffer base address.
	SceSize size; //!< Buffer size in bytes.
} SceNgsBufferInfo;
VITASDK_BUILD_ASSERT_EQ(0x8, SceNgsBufferInfo); // size is from FW 3.60

/** System sizing and processing configuration. */
typedef struct SceNgsSystemInitParams {
	SceInt32 maxRacks;    //!< Maximum number of racks; must be from 1 through 64.
	SceInt32 maxVoices;   //!< Maximum number of voices; must be from 1 through 256.
	SceInt32 granularity; //!< Number of samples processed per update; a multiple of 64 from 64 through 512.
	SceInt32 sampleRate;  //!< System sample rate; must be 48000 on FW 3.60.
	SceInt32 reserved;    //!< Must be at least zero; otherwise ignored on FW 3.60.
} SceNgsSystemInitParams;
VITASDK_BUILD_ASSERT_EQ(0x14, SceNgsSystemInitParams); // size is from FW 3.60

/** Description used to size and create a rack. */
typedef struct SceNgsRackDescription {
	const SceNgsVoiceDefinition *voiceDefinition; //!< Immutable definition returned by a sceNgsVoiceDefGet* function.
	SceInt32 voiceCount;                          //!< Number of voices to allocate; must be greater than zero.
	SceInt32 channelsPerVoice;                    //!< Audio channels per voice; must be 1 or 2.
	SceInt32 maxPatchesPerInput;                  //!< Patch slots for each dynamic input. Input/mixer racks require a nonzero value.
	SceInt32 patchesPerOutput;                    //!< Patch slots allocated for each voice output.
	void *userData;                               //!< Value delivered to the callback passed to ::sceNgsRackRelease.
} SceNgsRackDescription;
VITASDK_BUILD_ASSERT_EQ(0x18, SceNgsRackDescription); // size is from FW 3.60

/** Endpoints used to create one voice-to-voice patch. */
typedef struct SceNgsPatchSetupInfo {
	SceNgsHVoice sourceVoice;       //!< Source voice.
	SceInt32 sourceOutputIndex;     //!< Source voice output index.
	SceInt32 sourceOutputSubIndex;  //!< Source output patch slot, or -1 to use the first free slot.
	SceNgsHVoice destinationVoice;  //!< Destination voice; it must belong to the same system as the source.
	SceInt32 destinationInputIndex; //!< Destination voice input index.
} SceNgsPatchSetupInfo;
VITASDK_BUILD_ASSERT_EQ(0x14, SceNgsPatchSetupInfo); // size is from FW 3.60

/** Voice preset header. Offsets are relative to the start of this structure. */
typedef struct SceNgsVoicePreset {
	SceInt32 nameOffset;        //!< Offset of the preset name. Ignored when applying the preset on FW 3.60.
	SceUInt32 nameLength;       //!< Preset-name length. Ignored when applying the preset on FW 3.60.
	SceInt32 presetDataOffset;  //!< Offset of the variable-length module parameter records, or zero.
	SceUInt32 presetDataSize;   //!< Size of the module parameter records in bytes.
	SceInt32 bypassFlagsOffset; //!< Offset of an array of module indices to bypass, or zero.
	SceUInt32 bypassFlagsCount; //!< Number of module indices in the bypass array.
} SceNgsVoicePreset;
VITASDK_BUILD_ASSERT_EQ(0x18, SceNgsVoicePreset); // size is from FW 3.60

/**
 * Information delivered to an NGS callback.
 *
 * A voice-finished callback receives the voice, rack, and user-data fields. A
 * rack-release callback receives the rack and user-data fields. A module
 * callback additionally receives the module ID, two module-defined reason
 * values, and an optional module-defined pointer. Parameter-range callbacks
 * receive the voice, rack, and module ID; the remaining fields are zero on
 * FW 3.60.
 */
typedef struct SceNgsCallbackInfo {
	SceNgsHVoice voiceHandle;  //!< Voice associated with the event, or zero for a rack-release event.
	SceNgsHRack rackHandle;    //!< Rack associated with the event.
	SceNgsModuleID moduleId;   //!< Module type associated with a module or parameter event.
	SceUInt32 callbackReason;  //!< First module-defined reason value.
	SceUInt32 callbackReason2; //!< Second module-defined reason value.
	void *callbackPtr;         //!< Optional module-defined pointer; valid for the duration of the callback.
	void *userData;            //!< User value registered with the callback or rack description.
} SceNgsCallbackInfo;
VITASDK_BUILD_ASSERT_EQ(0x1C, SceNgsCallbackInfo); // size is from FW 3.60

/** Header shared by every module-specific parameter structure. */
typedef struct SceNgsParamsDescriptor {
	SceNgsParamsID id;
	SceSize size; //!< Size beginning at \a id, including this field and the module-specific payload.
} SceNgsParamsDescriptor;
VITASDK_BUILD_ASSERT_EQ(0x8, SceNgsParamsDescriptor); // size is from FW 3.60

/** Error codes returned by the FW 3.60 NGS implementation. */
typedef enum SceNgsErrorCode {
	SCE_NGS_OK                           = 0x00000000,
	SCE_NGS_ERROR                        = 0x804A0001,
	SCE_NGS_ERROR_INVALID_PARAM          = 0x804A0002,
	SCE_NGS_ERROR_INVALID_ALIGNMENT      = 0x804A0003,
	SCE_NGS_ERROR_NOT_IMPL               = 0x804A0004,
	SCE_NGS_ERROR_DEPENDENCY             = 0x804A0005,
	SCE_NGS_ERROR_OUT_OF_ASSETS          = 0x804A0006,
	SCE_NGS_ERROR_MODULE_NOT_AVAIL       = 0x804A0007,
	SCE_NGS_ERROR_RESOURCE_LOCKED        = 0x804A0008,
	SCE_NGS_ERROR_PARAM_OUT_OF_RANGE     = 0x804A0009,
	SCE_NGS_ERROR_INVALID_VOICE_TYPE     = 0x804A000A,
	SCE_NGS_ERROR_SYSTEM_MISMATCH        = 0x804A000B,
	SCE_NGS_ERROR_INVALID_HANDLE         = 0x804A000C,
	SCE_NGS_ERROR_SIZE_MISMATCH          = 0x804A000D,
	SCE_NGS_ERROR_PATCH_NOT_AVAIL        = 0x804A000E,
	SCE_NGS_ERROR_PARAM_TYPE_MISMATCH    = 0x804A000F,
	SCE_NGS_ERROR_INVALID_STATE          = 0x804A0010,
	SCE_NGS_ERROR_INTERNAL_ALLOC         = 0x804A0011,
	SCE_NGS_ERROR_INTERNAL_PROCESSING    = 0x804A0012,
	SCE_NGS_ERROR_INVALID_BUFFER         = 0x804A0013
} SceNgsErrorCode;

/** Flags accepted by ::sceNgsSystemSetFlags. */
typedef enum SceNgsSystemFlag {
	SCE_NGS_SYSTEM_FLAG_CHECK_PARAMS = 0x00000001 //!< Validate parameter ranges when parameter blocks are unlocked or applied.
} SceNgsSystemFlag;

/** Reset operations selected by ::sceNgsVoiceInit. */
typedef enum SceNgsVoiceInitFlag {
	SCE_NGS_VOICE_INIT_BASE      = 0x00000000,
	SCE_NGS_VOICE_INIT_ROUTING   = 0x00000001, //!< Remove every input and output patch connected to the voice.
	SCE_NGS_VOICE_INIT_PRESET    = 0x00000002, //!< Apply the supplied preset, or the voice definition's default preset when it is NULL.
	SCE_NGS_VOICE_INIT_CALLBACKS = 0x00000004, //!< Clear the module and voice-finished callbacks.
	SCE_NGS_VOICE_INIT_ALL       = 0x00000007
} SceNgsVoiceInitFlag;

/** Bitfield reported by ::SceNgsVoiceInfo::voiceState. */
typedef enum SceNgsVoiceState {
	SCE_NGS_VOICE_STATE_AVAILABLE = 0x00000000,
	SCE_NGS_VOICE_STATE_ACTIVE    = 0x00000001,
	SCE_NGS_VOICE_STATE_FINALIZE  = 0x00000004,
	SCE_NGS_VOICE_STATE_UNLOADING = 0x00000008,
	SCE_NGS_VOICE_STATE_PENDING   = 0x00000010,
	SCE_NGS_VOICE_STATE_PAUSED    = 0x00000020,
	SCE_NGS_VOICE_STATE_KEY_OFF   = 0x00000040
} SceNgsVoiceState;

/** Values accepted and returned by the module-bypass functions. */
typedef enum SceNgsModuleBypassFlag {
	SCE_NGS_MODULE_FLAG_NOT_BYPASSED = 0,
	SCE_NGS_MODULE_FLAG_BYPASSED     = 2
} SceNgsModuleBypassFlag;

/** Byte range and edge skips for decoding a requested ATRAC9 sample interval. */
typedef struct SceNgsAT9SkipBufferInfo {
	SceInt32 startByteOffset; //!< Byte offset of the first required ATRAC9 packet.
	SceInt32 numBytes;        //!< Number of bytes to make available to the decoder.
	SceInt16 startSkip;       //!< Samples to discard at the start of the first decoded packet.
	SceInt16 endSkip;         //!< Samples to discard at the end of the last decoded packet.
	SceInt32 isSuperPacket;   //!< 1 for the four-frame packet layout, otherwise 0.
} SceNgsAT9SkipBufferInfo;
VITASDK_BUILD_ASSERT_EQ(0x10, SceNgsAT9SkipBufferInfo); // size is from FW 3.60

/** Header of one record accepted by ::sceNgsVoiceSetParamsBlock. */
typedef struct SceNgsModuleParamHeader {
	SceInt32 moduleIndex;  //!< Zero-based index in the voice definition.
	SceInt32 channelIndex; //!< Channel to update, or -1 to update every channel represented by the following parameter descriptor.
} SceNgsModuleParamHeader;
VITASDK_BUILD_ASSERT_EQ(0x8, SceNgsModuleParamHeader); // size is from FW 3.60

/** Gain matrix indexed as `matrix[outputChannel][inputChannel]`. */
typedef struct SceNgsVolumeMatrix {
	SceFloat32 matrix[SCE_NGS_MAX_SYSTEM_CHANNELS][SCE_NGS_MAX_SYSTEM_CHANNELS];
} SceNgsVolumeMatrix;
VITASDK_BUILD_ASSERT_EQ(0x10, SceNgsVolumeMatrix); // size is from FW 3.60

/** Current route and gain information returned by ::sceNgsPatchGetInfo. */
typedef struct SceNgsPatchRouteInfo {
	SceInt32 outputChannels;         //!< Source-rack channel count.
	SceInt32 inputChannels;          //!< Destination-rack channel count.
	SceNgsVolumeMatrix volumeMatrix; //!< Current output-to-input gain matrix.
} SceNgsPatchRouteInfo;
VITASDK_BUILD_ASSERT_EQ(0x18, SceNgsPatchRouteInfo); // size is from FW 3.60

/** Current voice and voice-definition information. */
typedef struct SceNgsVoiceInfo {
	SceUInt32 voiceState;          //!< Bitwise OR of ::SceNgsVoiceState values.
	SceUInt32 numModules;          //!< Number of modules in the voice definition.
	SceUInt32 numInputs;           //!< Number of dynamic input modules in the voice definition.
	SceUInt32 numOutputs;          //!< Number of outputs exposed by the voice definition.
	SceUInt32 numPatchesPerOutput; //!< Patch slots allocated for each output.
	SceUInt32 updatePassed;        //!< Number of processing updates completed for this voice since its most recent initialization or play.
} SceNgsVoiceInfo;
VITASDK_BUILD_ASSERT_EQ(0x18, SceNgsVoiceInfo); // size is from FW 3.60

/** Sulpha NGS trace-agent capacities. */
typedef struct SceSulphaNgsConfig {
	SceUInt32 maxNamedSamples;    //!< Maximum sample-memory ranges that can be named; the FW 3.60 default is 1024.
	SceUInt32 apiTraceBufferSize; //!< NGS API-call trace buffer size in bytes; only complete 16-byte records are used. The FW 3.60 default is 4096.
} SceSulphaNgsConfig;
VITASDK_BUILD_ASSERT_EQ(0x8, SceSulphaNgsConfig); // size is from FW 3.60

/**
 * Compute the encoded byte range and edge skips for an ATRAC9 sample range.
 *
 * The output is cleared before validation. FW 3.60 accepts ATRAC9 frame-duration
 * codes 1, 4, and 7 and requires the low byte of \a nConfigData to be 0xFE.
 * Bit 31 of \a nStartSampleOffset requests one frame of decoder preroll; the
 * remaining 31 bits contain the sample offset and are backed up by 64, 128, or
 * 256 samples according to the frame-duration code, clamped at zero.
 *
 * @param[in] nStartSampleOffset - First requested decoded sample.
 * @param[in] nNumSamples - Number of requested samples; must be at least zero.
 * @param[in] nConfigData - Packed ATRAC9 configuration word.
 * @param[out] pAt9InfoBuffer - Receives the required byte range and sample skips.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsAT9GetSectionDetails(SceInt32 nStartSampleOffset, SceInt32 nNumSamples, SceInt32 nConfigData, SceNgsAT9SkipBufferInfo *pAt9InfoBuffer);

/**
 * Return the number of built-in presets provided by a module type.
 *
 * @param[in] hSystemHandle - Initialized system handle.
 * @param[in] uModuleID - Module type ID, not a module index in a voice.
 * @param[out] puNumPresets - Receives the preset count.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsModuleGetNumPresets(SceNgsHSynSystem hSystemHandle, SceNgsModuleID uModuleID, SceUInt32 *puNumPresets);

/**
 * Copy one built-in module preset into a caller-owned buffer.
 *
 * Initialize both members of \a pParamsBuffer before the call. Its data buffer
 * must be large enough for the selected preset; the function does not allocate
 * memory or change the buffer-size member.
 *
 * @param[in] hSystemHandle - Initialized system handle.
 * @param[in] uModuleID - Module type ID, not a module index in a voice.
 * @param[in] uPresetIndex - Zero-based preset index.
 * @param[in,out] pParamsBuffer - Destination pointer and capacity in bytes.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsModuleGetPreset(SceNgsHSynSystem hSystemHandle, SceNgsModuleID uModuleID, SceUInt32 uPresetIndex, SceNgsBufferInfo *pParamsBuffer);

/**
 * Create a route from one voice output to one voice input.
 *
 * Both voices must belong to the same system. A source sub-index of -1 selects
 * the first free patch slot. The route is initialized with zero gains. NGS
 * rejects a route that would introduce an unschedulable dependency cycle.
 *
 * @param[in] pPatchInfo - Source and destination endpoints.
 * @param[out] pPatchHandle - Receives the new patch handle.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsPatchCreateRouting(const SceNgsPatchSetupInfo *pPatchInfo, SceNgsHPatch *pPatchHandle);

/**
 * Get the current source/destination channel counts and gains of a patch.
 *
 * @param[in] hPatchHandle - Patch handle.
 * @param[out] pRouteInfo - Receives current channel counts and matrix information.
 * @param[out] pSetup - Optional destination for the original patch setup.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsPatchGetInfo(SceNgsHPatch hPatchHandle, SceNgsPatchRouteInfo *pRouteInfo, SceNgsPatchSetupInfo *pSetup);

/**
 * Remove a voice-to-voice patch and release its source output slot.
 *
 * @param[in] hPatchHandle - Patch handle to remove.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsPatchRemoveRouting(SceNgsHPatch hPatchHandle);

/**
 * Compute the rack-buffer size required for a description.
 *
 * @param[in] hSystemHandle - System that will own the rack.
 * @param[in] pRackDesc - Rack description.
 * @param[out] pnSize - Receives the required number of bytes.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsRackGetRequiredMemorySize(SceNgsHSynSystem hSystemHandle, const SceNgsRackDescription *pRackDesc, SceSize *pnSize);

/**
 * Return one voice allocated by a rack.
 *
 * @param[in] hRackHandle - Initialized rack handle.
 * @param[in] uIndex - Zero-based voice index, less than the rack's voice count.
 * @param[out] pVoiceHandle - Receives the voice handle.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsRackGetVoiceHandle(SceNgsHRack hRackHandle, SceUInt32 uIndex, SceNgsHVoice *pVoiceHandle);

/**
 * Construct a rack and all of its voices in caller-owned memory.
 *
 * The buffer base must be 16-byte aligned and its capacity must be at least the
 * value returned by ::sceNgsRackGetRequiredMemorySize. NGS retains and modifies
 * this memory until the rack has been released. Initialization consumes one
 * rack slot and ::SceNgsRackDescription::voiceCount voice slots from the
 * system limits.
 *
 * @param[in] hSystemHandle - Owning system handle.
 * @param[in] pRackBuffer - Rack buffer pointer and capacity.
 * @param[in] pRackDesc - Rack topology and allocation description.
 * @param[out] pRackHandle - Receives the rack handle.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsRackInit(SceNgsHSynSystem hSystemHandle, const SceNgsBufferInfo *pRackBuffer, const SceNgsRackDescription *pRackDesc, SceNgsHRack *pRackHandle);

/**
 * Stop every voice, remove every connected patch, and release a rack.
 *
 * With a NULL callback, release is completed synchronously. With a callback,
 * destruction is finalized by a subsequent ::sceNgsSystemUpdate and the
 * callback receives the rack and ::SceNgsRackDescription::userData.
 *
 * @param[in] hRackHandle - Rack to release.
 * @param[in] callbackFuncPtr - Optional asynchronous completion callback.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsRackRelease(SceNgsHRack hRackHandle, SceNgsRackReleaseCallbackFunc callbackFuncPtr);

/**
 * Set the parameter-range callback for one rack.
 *
 * This callback takes precedence over the system-wide callback and is invoked
 * by ::sceNgsSystemUpdate on the thread that calls it.
 *
 * @param[in] hRackHandle - Rack handle.
 * @param[in] callbackFuncPtr - Callback, or NULL to clear it.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsRackSetParamErrorCallback(SceNgsHRack hRackHandle, SceNgsParamsErrorCallbackFunc callbackFuncPtr);

/**
 * Compute the system-buffer size required by an initialization configuration.
 *
 * @param[in] pSynthParams - System limits and processing configuration.
 * @param[out] pnSize - Receives the required number of bytes.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemGetRequiredMemorySize(const SceNgsSystemInitParams *pSynthParams, SceSize *pnSize);

/**
 * Initialize an NGS system in caller-owned memory.
 *
 * The memory base must be 16-byte aligned and the size must be at least the
 * result of ::sceNgsSystemGetRequiredMemorySize. FW 3.60 permits at most eight
 * simultaneous systems in one process and initializes each system with
 * ::SCE_NGS_SYSTEM_FLAG_CHECK_PARAMS enabled.
 *
 * @param[in,out] pSynthSysMemory - Memory retained and modified by NGS until release.
 * @param[in] uMemSize - Available bytes at \a pSynthSysMemory.
 * @param[in] pSynthParams - System limits and processing configuration.
 * @param[out] pSystemHandle - Receives the initialized system handle.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemInit(void *pSynthSysMemory, SceSize uMemSize, const SceNgsSystemInitParams *pSynthParams, SceNgsHSynSystem *pSystemHandle);

/**
 * Lock the system's internal update state.
 *
 * Calls must be balanced with ::sceNgsSystemUnlock.
 *
 * @param[in] hSystemHandle - System to lock.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemLock(SceNgsHSynSystem hSystemHandle);

/**
 * Release a system after all of its racks have been released.
 *
 * @param[in] hSystemHandle - System to release.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemRelease(SceNgsHSynSystem hSystemHandle);

/**
 * Replace the system flags used by subsequent parameter operations.
 *
 * @param[in] hSystemHandle - System handle.
 * @param[in] uSystemFlags - Bitwise OR of ::SceNgsSystemFlag values.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemSetFlags(SceNgsHSynSystem hSystemHandle, SceUInt32 uSystemFlags);

/**
 * Set the fallback parameter-range callback for a system.
 *
 * @param[in] hSystemHandle - System handle.
 * @param[in] callbackFuncPtr - Callback, or NULL to clear it.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemSetParamErrorCallback(SceNgsHSynSystem hSystemHandle, SceNgsParamsErrorCallbackFunc callbackFuncPtr);

/**
 * Unlock a system previously locked with ::sceNgsSystemLock.
 *
 * @param[in] hSystemHandle - System to unlock.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemUnlock(SceNgsHSynSystem hSystemHandle);

/**
 * Process one audio grain and dispatch every queued callback.
 *
 * Callback functions execute synchronously on the thread calling this function,
 * after the kernel-side processing update has completed.
 *
 * @param[in] hSystemHandle - System to update.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsSystemUpdate(SceNgsHSynSystem hSystemHandle);

/**
 * Set the bypass state of one module in a voice definition.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index, not a ::SceNgsModuleID.
 * @param[in] uBypassFlag - One of ::SceNgsModuleBypassFlag.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceBypassModule(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, SceUInt32 uBypassFlag);
/** Return the complex ATRAC9 playback voice definition with four sends. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetAtrac9Voice(void);

/** Return an input-mix, compressor, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetCompressorBuss(void);

/** Return a two-input side-chain compressor bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetCompressorSideChainBuss(void);

/** Return an input-mix, modulated-delay, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetDelayBuss(void);

/** Return an input-mix, distortion, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetDistortionBuss(void);

/** Return an input-mix, amplitude-envelope, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetEnvelopeBuss(void);

/** Return an input-mix, four-band parametric-EQ, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetEqBuss(void);

/** Return the master bus definition that exposes PCM output to the caller. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetMasterBuss(void);

/** Return a bus definition that mixes multiple input voices. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetMixerBuss(void);

/** Return an input-mix, click-free pauser, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetPauserBuss(void);

/** Return an input-mix, pitch-shift, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetPitchShiftBuss(void);

/** Return an input-mix, environmental-reverb, and output bus definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetReverbBuss(void);

/** Return the libSas-emulation voice definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetSasEmuVoice(void);

/** Return the Scream ATRAC9 playback voice definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetScreamAtrac9Voice(void);

/** Return the Scream waveform playback voice definition. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetScreamVoice(void);

/** Return the basic ATRAC9 playback voice definition with two sends. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetSimpleAtrac9Voice(void);

/** Return the basic PCM/VAG playback voice definition with two sends. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetSimpleVoice(void);

/** Return the complex waveform playback template with signal generation and four sends. */
const SceNgsVoiceDefinition *sceNgsVoiceDefGetTemplate1(void);

/**
 * Get one voice's current key state and topology information.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[out] pInfo - Receives the voice snapshot.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetInfo(SceNgsHVoice hVoiceHandle, SceNgsVoiceInfo *pInfo);

/**
 * Get the bypass state of one module.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[out] puBypassFlag - Receives one of ::SceNgsModuleBypassFlag.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetModuleBypass(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, SceUInt32 *puBypassFlag);

/**
 * Map a zero-based module index to its module type ID.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[out] pModuleType - Receives the ::SceNgsModuleID.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetModuleType(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, SceNgsModuleID *pModuleType);

/**
 * Return the patch occupying one voice output slot.
 *
 * @param[in] hVoiceHandle - Source voice handle.
 * @param[in] nOutputIndex - Zero-based output index.
 * @param[in] nSubIndex - Zero-based patch slot within that output.
 * @param[out] pPatchHandle - Receives the patch handle, or zero when the slot is empty.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetOutputPatch(SceNgsHVoice hVoiceHandle, SceInt32 nOutputIndex, SceInt32 nSubIndex, SceNgsHPatch *pPatchHandle);

/**
 * Format the most recent parameter-range result for one module.
 *
 * The destination must provide at least 128 bytes. The resulting string is
 * either a module mismatch/range diagnostic or `No value was out of range\n`.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[out] pszMessageBuffer - Writable 128-byte string buffer.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetParamsOutOfRange(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, char *pszMessageBuffer);

/**
 * Copy one module's runtime state snapshot.
 *
 * The requested size must exactly match that module's state-data size; otherwise
 * ::SCE_NGS_ERROR_SIZE_MISMATCH is returned.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[out] pMem - Destination buffer.
 * @param[in] uMemSize - Exact module state-data size in bytes.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceGetStateData(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, void *pMem, SceSize uMemSize);

/**
 * Reinitialize a stopped voice and optionally reset patches, preset, or callbacks.
 *
 * The voice must not be playing or keyed off. Bits outside
 * ::SceNgsVoiceInitFlag are ignored on FW 3.60.
 *
 * @param[in] hVoiceHandle - Voice to initialize.
 * @param[in] pPreset - Optional preset selected by ::SCE_NGS_VOICE_INIT_PRESET.
 * @param[in] uInitFlags - Bitwise OR of ::SceNgsVoiceInitFlag values.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceInit(SceNgsHVoice hVoiceHandle, const SceNgsVoicePreset *pPreset, SceUInt32 uInitFlags);

/**
 * Request release/envelope processing for a playing voice.
 *
 * This changes the key state from playing to keyed off; processing continues
 * until the voice finishes or is killed.
 *
 * @param[in] hVoiceHandle - Playing voice to key off.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceKeyOff(SceNgsHVoice hVoiceHandle);

/**
 * Immediately stop a playing or keyed-off voice and return it to the stopped state.
 *
 * @param[in] hVoiceHandle - Voice to stop.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceKill(SceNgsHVoice hVoiceHandle);

/**
 * Lock and expose one module's live parameter block.
 *
 * Only one lock for a module may be active at a time. The returned pointer
 * refers directly to the rack buffer and remains valid until the rack is
 * released, but changes are committed by ::sceNgsVoiceUnlockParams.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[in] uParamsInterfaceId - Expected parameter-interface ID; FW 3.60 compares its low 16 bits.
 * @param[out] pParamsBuffer - Receives the live parameter pointer and size.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceLockParams(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, SceNgsParamsID uParamsInterfaceId, SceNgsBufferInfo *pParamsBuffer);

/**
 * Set one gain in a patch's `[output][input]` matrix.
 *
 * @param[in] hPatchHandle - Patch handle.
 * @param[in] nOutputChannel - Output channel, less than the source channel count.
 * @param[in] nInputChannel - Destination input channel; must be 0 or 1.
 * @param[in] fVol - Linear gain stored without clamping on FW 3.60.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoicePatchSetVolume(SceNgsHPatch hPatchHandle, SceInt32 nOutputChannel, SceInt32 nInputChannel, SceFloat32 fVol);

/**
 * Set the one or two input-channel gains for a patch output.
 *
 * @param[in] hPatchHandle - Patch handle.
 * @param[in] nOutputChannel - Output row to replace.
 * @param[in] pVolumes - Array containing a nVols gains.
 * @param[in] nVols - Number of gains; valid callers use 1 or 2.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoicePatchSetVolumes(SceNgsHPatch hPatchHandle, SceInt32 nOutputChannel, const SceFloat32 *pVolumes, SceInt32 nVols);

/**
 * Replace a patch's complete 2x2 output-to-input gain matrix.
 *
 * A mono source consumes only the first row; a stereo source consumes all four
 * values.
 *
 * @param[in] hPatchHandle - Patch handle.
 * @param[in] pMatrix - Replacement gain matrix.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoicePatchSetVolumesMatrix(SceNgsHPatch hPatchHandle, const SceNgsVolumeMatrix *pMatrix);

/**
 * Pause voice processing without changing whether the voice is playing or keyed off.
 *
 * @param[in] hVoiceHandle - Voice to pause.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoicePause(SceNgsHVoice hVoiceHandle);

/**
 * Start a stopped voice.
 *
 * A voice must be initialized before it can be played.
 *
 * @param[in] hVoiceHandle - Voice to start.
 *
 * @return 0 on success, or ::SCE_NGS_ERROR_INVALID_STATE if it is not stopped.
 */
SceInt32 sceNgsVoicePlay(SceNgsHVoice hVoiceHandle);

/**
 * Resume a paused voice without changing its play/key-off state.
 *
 * @param[in] hVoiceHandle - Voice to resume.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceResume(SceNgsHVoice hVoiceHandle);

/**
 * Set the callback queued when a voice reaches its finished state.
 *
 * The callback runs from ::sceNgsSystemUpdate and receives \a pUserData through
 * ::SceNgsCallbackInfo::userData.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] callbackFuncPtr - Callback, or NULL to clear it.
 * @param[in] pUserData - Opaque value returned to the callback.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceSetFinishedCallback(SceNgsHVoice hVoiceHandle, SceNgsCallbackFunc callbackFuncPtr, void *pUserData);

/**
 * Set the callback for module-defined events emitted by one module index.
 *
 * The callback runs from ::sceNgsSystemUpdate. Its reason and data fields are
 * defined by the selected module type.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index.
 * @param[in] callbackFuncPtr - Callback, or NULL to clear it.
 * @param[in] pUserData - Opaque value returned to the callback.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceSetModuleCallback(SceNgsHVoice hVoiceHandle, SceUInt32 uModule, SceNgsModuleCallbackFunc callbackFuncPtr, void *pUserData);

/**
 * Apply a packed sequence of module parameter records.
 *
 * Valid records are applied even when another record fails. If non-NULL,
 * \a pnErrorCount receives the number of rejected records.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] pParamData - First packed record header. Each header is followed
 *                         by a ::SceNgsParamsDescriptor and its module-specific
 *                         payload.
 * @param[in] uSize - Total byte size of the record sequence.
 * @param[out] pnErrorCount - Optional rejected-record count.
 *
 * @return 0 when every record is valid, or the error encountered while parsing or applying a record.
 */
SceInt32 sceNgsVoiceSetParamsBlock(SceNgsHVoice hVoiceHandle, const SceNgsModuleParamHeader *pParamData, SceSize uSize, SceInt32 *pnErrorCount);

/**
 * Restore module defaults, then apply an optional voice preset.
 *
 * A NULL preset restores the definition's module defaults and clears the preset
 * bypass list. The voice does not have to be stopped, but locked parameter
 * interfaces cause ::SCE_NGS_ERROR_RESOURCE_LOCKED.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] pVoicePreset - Preset to apply, or NULL for module defaults.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceSetPreset(SceNgsHVoice hVoiceHandle, const SceNgsVoicePreset *pVoicePreset);

/**
 * Commit and unlock a module parameter block returned by ::sceNgsVoiceLockParams.
 *
 * When ::SCE_NGS_SYSTEM_FLAG_CHECK_PARAMS is set, invalid values are rejected
 * and the module is not marked dirty for processing.
 *
 * @param[in] hVoiceHandle - Voice handle.
 * @param[in] uModule - Zero-based module index previously locked.
 *
 * @return 0 on success, or a negative ::SceNgsErrorCode value.
 */
SceInt32 sceNgsVoiceUnlockParams(SceNgsHVoice hVoiceHandle, SceUInt32 uModule);
/**
 * Fill a Sulpha NGS configuration with the FW 3.60 defaults.
 *
 * @param[out] config - Receives capacities 1024 and 4096.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsGetDefaultConfig(SceSulphaNgsConfig *config);

/**
 * Compute all memory needed by the Sulpha NGS agent and its fixed NGS pools.
 *
 * @param[in] config - Sulpha NGS capacities.
 * @param[out] sizeInBytes - Receives the required byte count.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsGetNeededMemory(const SceSulphaNgsConfig *config, SceSize *sizeInBytes);

/**
 * Initialize the process-global Sulpha NGS tracing agent.
 *
 * The supplied memory must remain valid until ::sceSulphaNgsShutdown. Use
 * ::sceSulphaNgsGetNeededMemory rather than relying on the implementation's
 * smaller local minimum-size check.
 *
 * @param[in] config - Sulpha NGS capacities.
 * @param[in,out] buffer - Caller-owned tracing memory.
 * @param[in] sizeInBytes - Available bytes in \a buffer.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsInit(const SceSulphaNgsConfig *config, void *buffer, SceSize sizeInBytes);

/**
 * Assign a diagnostic Sulpha name to an initialized rack.
 *
 * @param[in] rackHandle - Rack handle.
 * @param[in] name - Diagnostic name forwarded to Sulpha.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsSetRackName(SceNgsHRack rackHandle, const char *name);

/**
 * Associate a diagnostic name with a sample-memory range.
 *
 * @param[in] location - Non-NULL sample-data address.
 * @param[in] length - Nonzero sample-data size in bytes.
 * @param[in] name - Diagnostic name forwarded to Sulpha.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsSetSampleName(const void *location, SceSize length, const char *name);

/**
 * Assign a diagnostic Sulpha name to an initialized NGS system.
 *
 * @param[in] synthHandle - System handle.
 * @param[in] name - Diagnostic name forwarded to Sulpha.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsSetSynthName(SceNgsHSynSystem synthHandle, const char *name);

/**
 * Assign a diagnostic Sulpha name to an initialized voice.
 *
 * @param[in] voiceHandle - Voice handle.
 * @param[in] name - Diagnostic name forwarded to Sulpha.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsSetVoiceName(SceNgsHVoice voiceHandle, const char *name);

/**
 * Unregister the process-global Sulpha NGS agent and release its registrations.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsShutdown(void);

/**
 * Emit \a message as a Sulpha trace event in the `Ngs` category.
 *
 * @param[in] message - Literal message; it is passed through a `%s` format.
 *
 * @return 0 on success, or a negative Sulpha error code.
 */
SceInt32 sceSulphaNgsTrace(const char *message);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_NGS_H_ */
