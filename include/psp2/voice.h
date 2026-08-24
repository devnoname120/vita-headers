/**
 * \usergroup{SceVoice}
 * \usage{psp2/voice.h,SceVoice_stub}
 */

#ifndef _PSP2_VOICE_H_
#define _PSP2_VOICE_H_

#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_VOICE_VERSION             100
#define SCE_VOICE_WORKING_MEMORY_SIZE 0x40000
#define SCE_VOICE_MAX_EVENT_PORTS     7

typedef enum SceVoiceErrorCode {
	SCE_VOICE_ERROR_NOT_INITIALIZED     = 0x804E0801, //!< ::sceVoiceInit has not succeeded.
	SCE_VOICE_ERROR_ALREADY_INITIALIZED = 0x804E0802, //!< A process-local instance already exists.
	SCE_VOICE_ERROR_INTERNAL            = 0x804E0803, //!< An internal backend operation failed.
	SCE_VOICE_ERROR_INVALID_PORT_ID     = 0x804E0804, //!< The port ID is out of range or unallocated for the operation.
	SCE_VOICE_ERROR_INVALID_ARGUMENT    = 0x804E0805, //!< An argument, format, or port type is invalid.
	SCE_VOICE_ERROR_INVALID_MEMBLOCK    = 0x804E0806, //!< The working-memory UID could not be resolved.
	SCE_VOICE_ERROR_INVALID_PORT        = 0x804E0807, //!< The port does not exist or is unsuitable for the operation.
	SCE_VOICE_ERROR_RESOURCE_LIMIT      = 0x804E0808, //!< A port, connection, codec, or memory limit was reached.
	SCE_VOICE_ERROR_NOT_STARTED         = 0x804E080A  //!< ::sceVoiceStart has not attached working memory.
} SceVoiceErrorCode;

/**
 * Voice application profiles accepted by ::sceVoiceInit.
 *
 * FW 3.60 treats both values identically: they select the same port and
 * working-memory limits. SceParty and SceVoiceQoS use 0x20000000; no FW 3.60
 * system module uses 0x80000000.
 */
typedef enum SceVoiceApplicationType {
	SCE_VOICE_APPLICATION_TYPE_UNK_0x20000000 = 0x20000000,
	SCE_VOICE_APPLICATION_TYPE_UNK_0x80000000 = 0x80000000
} SceVoiceApplicationType;

typedef enum SceVoicePortType {
	SCE_VOICE_PORT_TYPE_IN_DEVICE    = 0, //!< Microphone/device input.
	SCE_VOICE_PORT_TYPE_IN_PCMAUDIO  = 1, //!< Signed 16-bit PCM supplied by the application.
	SCE_VOICE_PORT_TYPE_IN_VOICE     = 2, //!< Encoded voice supplied by the application.
	SCE_VOICE_PORT_TYPE_OUT_PCMAUDIO = 3, //!< Signed 16-bit PCM read by the application.
	SCE_VOICE_PORT_TYPE_OUT_VOICE    = 4, //!< Encoded voice read by the application.
	SCE_VOICE_PORT_TYPE_OUT_DEVICE   = 5  //!< Speaker/headset output.
} SceVoicePortType;

/** Port processing states reported by ::sceVoiceGetPortInfo on FW 3.60. */
typedef enum SceVoicePortState {
	SCE_VOICE_PORT_STATE_INACTIVE  = 0, //!< The port backend is not currently processing.
	SCE_VOICE_PORT_STATE_IDLE      = 1, //!< Backend active and waiting for data.
	SCE_VOICE_PORT_STATE_BUFFERING = 2, //!< Application input buffering or rebuffering to its effective threshold.
	SCE_VOICE_PORT_STATE_ACTIVE    = 3  //!< Processing after sufficient data became available.
} SceVoicePortState;

/** 8 kHz CELP bitrates supported by the FW 3.60 Voice codec. */
typedef enum SceVoiceBitRate {
	SCE_VOICE_BIT_RATE_3850  = 3850,  //!< 20 encoded bytes and 320 decoded samples per 40-ms frame.
	SCE_VOICE_BIT_RATE_4650  = 4650,  //!< 24 encoded bytes and 320 decoded samples per 40-ms frame.
	SCE_VOICE_BIT_RATE_5700  = 5700,  //!< 15 encoded bytes and 160 decoded samples per 20-ms frame.
	SCE_VOICE_BIT_RATE_7300  = 7300,  //!< 19 encoded bytes and 160 decoded samples per 20-ms frame.
	SCE_VOICE_BIT_RATE_12200 = 12200  //!< 16 encoded bytes and 80 decoded samples per 10-ms frame.
} SceVoiceBitRate;

typedef enum SceVoicePcmDataType {
	SCE_VOICE_PCM_DATA_TYPE_S16LE = 0 //!< Signed 16-bit little-endian PCM.
} SceVoicePcmDataType;

typedef enum SceVoiceSamplingRate {
	SCE_VOICE_SAMPLING_RATE_16000 = 16000
} SceVoiceSamplingRate;

typedef enum SceVoicePortAttr {
	SCE_VOICE_PORT_ATTR_AUDIO_INPUT_OWNERSHIP = 1000 //!< Audio-input adoption state of a device-input port.
} SceVoicePortAttr;

typedef enum SceVoiceEventType {
	SCE_VOICE_EVENT_TYPE_PORT_DATA_READY               = 1, //!< One or more OUT_VOICE ports contain encoded data.
	SCE_VOICE_EVENT_TYPE_AUDIO_INPUT_OWNERSHIP_CHANGED = 2  //!< Audio-input ownership was acquired or lost.
} SceVoiceEventType;

typedef SceUInt32 SceVoicePortId;

typedef struct SceVoiceEventPortDataReady {
	SceInt16 portCount; //!< Number of valid entries in @p portIds.
	SceUInt16 portIds[SCE_VOICE_MAX_EVENT_PORTS]; //!< Output-voice ports with encoded data available.
} SceVoiceEventPortDataReady;
VITASDK_BUILD_ASSERT_EQ(0x10, SceVoiceEventPortDataReady); // size is from FW 3.60

typedef struct SceVoiceEventAudioInputOwnershipChanged {
	SceUInt8 owned; //!< Nonzero when this process owns the adopted audio-input port.
	SceUInt8 reserved[0xF];
} SceVoiceEventAudioInputOwnershipChanged;
VITASDK_BUILD_ASSERT_EQ(0x10, SceVoiceEventAudioInputOwnershipChanged); // size is from FW 3.60

typedef struct SceVoiceEvent {
	SceUInt32 eventType; //!< One of ::SceVoiceEventType.
	void *userData; //!< Value supplied to ::sceVoiceInit.
	union {
		SceVoiceEventPortDataReady portDataReady;
		SceVoiceEventAudioInputOwnershipChanged audioInputOwnershipChanged;
	} eventPayload;
} SceVoiceEvent;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVoiceEvent); // size is from FW 3.60

/**
 * Voice event callback.
 *
 * Callbacks run serially on SceVoice's private event thread. The event points
 * to transient thread-stack storage and is valid only until the callback
 * returns. Other SceVoice APIs may be called from the callback, but
 * ::sceVoiceEnd must not be called because it waits for the event thread.
 *
 * @param[in] event - Event record valid for the duration of the callback.
 */
typedef void (*SceVoiceEventCallback)(const SceVoiceEvent *event);

typedef struct SceVoiceInitParam {
	SceUInt32 applicationType; //!< One of ::SceVoiceApplicationType.
	SceVoiceEventCallback eventCallback; //!< Optional event callback, or NULL to disable events.
	void *userData; //!< Copied to ::SceVoiceEvent::userData.
	SceUInt32 reserved[5]; //!< Ignored on FW 3.60.
} SceVoiceInitParam;
VITASDK_BUILD_ASSERT_EQ(0x20, SceVoiceInitParam); // size is from FW 3.60

typedef struct SceVoiceStartParam {
	SceUID memBlockId; //!< Memory block providing at least ::SCE_VOICE_WORKING_MEMORY_SIZE bytes.
	SceUInt32 reserved[7]; //!< Ignored on FW 3.60.
} SceVoiceStartParam;
VITASDK_BUILD_ASSERT_EQ(0x20, SceVoiceStartParam); // size is from FW 3.60

typedef struct SceVoicePortParam {
	SceUInt32 portType; //!< One of ::SceVoicePortType.
	SceUInt16 threshold; //!< Application-fed input threshold in milliseconds; at least two frame durations are used.
	SceUInt16 muteFlag; //!< Nonzero to create the port muted.
	float volume; //!< Linear volume; 1.0 is unity gain.
	union {
		SceSize bufferSize; //!< PCM byte capacity for PCMAUDIO ports.
		SceUInt32 bitRate; //!< One of ::SceVoiceBitRate for VOICE ports.
	} data;
	SceUInt32 pcmDataType; //!< One of ::SceVoicePcmDataType for PCMAUDIO ports.
	SceUInt32 samplingRate; //!< One of ::SceVoiceSamplingRate for PCMAUDIO ports.
} SceVoicePortParam;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVoicePortParam); // size is from FW 3.60

typedef struct SceVoiceResourceInfo {
	SceUInt16 maxVoiceInputPorts; //!< Maximum number of IN_VOICE ports.
	SceUInt16 maxVoiceOutputPorts; //!< Maximum number of OUT_VOICE ports.
	SceUInt16 maxDeviceInputPorts; //!< Maximum number of IN_DEVICE ports.
	SceUInt16 maxDeviceOutputPorts; //!< Maximum number of OUT_DEVICE ports.
	SceUInt16 maxPorts; //!< Maximum number of ports of all types.
} SceVoiceResourceInfo;
VITASDK_BUILD_ASSERT_EQ(0xA, SceVoiceResourceInfo); // size is from FW 3.60

typedef struct SceVoicePortInfo {
	SceUInt32 portType; //!< One of ::SceVoicePortType.
	SceUInt32 state; //!< One of ::SceVoicePortState.
	SceUInt32 reserved0; //!< Not written on FW 3.60.
	SceSize dataSize; //!< Free bytes in the input ring or queued bytes in the output ring.
	SceSize frameSize; //!< Encoded or PCM processing-frame size in bytes.
	SceUInt32 reserved1; //!< Not written on FW 3.60.
} SceVoicePortInfo;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVoicePortInfo); // size is from FW 3.60

/**
 * Initialize the process-local Voice instance.
 *
 * @param[in] param - Initialization parameters. The callback may be NULL.
 * @param[in] version - Must be ::SCE_VOICE_VERSION on FW 3.60.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceInit(const SceVoiceInitParam *param, SceUInt32 version);

/**
 * Terminate the process-local Voice instance and release its ports.
 *
 * If Voice is started, this function stops every backend and detaches the
 * working-memory block before destroying the ports and event thread. The
 * caller remains responsible for freeing the working-memory block.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceEnd(void);

/**
 * Attach the Voice instance to its working-memory block and start processing.
 *
 * FW 3.60 uses 0x28000 bytes as private memory followed by 0x18000 bytes as
 * shared memory. It does not validate the memory-block size, so the supplied
 * block must contain at least ::SCE_VOICE_WORKING_MEMORY_SIZE bytes.
 * SceVoice does not take ownership of the block; it must remain allocated
 * until ::sceVoiceStop or ::sceVoiceEnd returns.
 *
 * Calling this function for an already started instance succeeds without
 * changing the current working-memory block. A non-NULL @p param is still
 * required in that case.
 *
 * @param[in] param - Start parameters.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceStart(const SceVoiceStartParam *param);

/**
 * Stop Voice processing and detach the working-memory block.
 *
 * The configured ports and connections are retained, but their backends and
 * buffered data are discarded. A later ::sceVoiceStart recreates them.
 * SceVoice does not free the caller-owned memory block.
 *
 * Calling this function while the initialized instance is already stopped
 * succeeds.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceStop(void);

/**
 * Get the port limits assigned to this Voice instance.
 *
 * On FW 3.60 the returned limits are seven encoded input ports, seven encoded
 * output ports, one device input port, one device output port, and 64 ports in
 * total.
 *
 * @param[out] info - Receives the resource limits.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetResourceInfo(SceVoiceResourceInfo *info);

/**
 * Validate the current port graph.
 *
 * FW 3.60 checks that every created port has at least one connection. It also
 * requires each adjacent pair of OUT_VOICE ports in the internal output list
 * to use identical bitrate, normalized per-port mute, and exact volume values.
 * When the graph contains both IN_DEVICE and IN_PCMAUDIO ports, it additionally
 * rejects an OUT_VOICE port connected from IN_PCMAUDIO while that output's
 * encoder backend has not been assigned, as occurs before Voice is started.
 *
 * @return 0 when the topology is valid, or a negative ::SceVoiceErrorCode
 *         value.
 */
int sceVoiceCheckTopology(void);

/**
 * Set the process-wide mute-reporting override.
 *
 * A nonzero value forces ::sceVoiceGetMuteFlag to report every port as muted.
 * FW 3.60 audio processing reads each port's individual mute flag directly,
 * so this function does not itself silence the ports.
 *
 * @param[in] mute - Nonzero to force the reported mute state to one, or zero
 *                   to report each port's individual mute flag.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceSetMuteFlagAll(SceBool mute);

/**
 * Set one port's mute flag.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[in] mute - Nonzero to mute the port, or zero to unmute it.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceSetMuteFlag(SceVoicePortId portId, SceBool mute);

/**
 * Get one port's reported mute flag.
 *
 * The process-wide flag set by ::sceVoiceSetMuteFlagAll overrides the
 * individual flag returned by this function. On FW 3.60 that process-wide
 * flag does not affect audio processing.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[out] mute - Receives 1 when muted or 0 when unmuted.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetMuteFlag(SceVoicePortId portId, SceUInt16 *mute);

/**
 * Set one port's linear volume.
 *
 * FW 3.60 does not clamp the stored value. During PCM processing the scaled
 * samples themselves are saturated to the signed 16-bit range.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[in] volume - Linear volume; 1.0 is unity gain.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceSetVolume(SceVoicePortId portId, float volume);

/**
 * Get one port's linear volume.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[out] volume - Receives the stored volume.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetVolume(SceVoicePortId portId, float *volume);

/**
 * Change the bitrate of an encoded input or output port.
 *
 * Changing the bitrate recreates that port's codec backend and clears its
 * buffered data.
 *
 * @param[in] portId - Encoded input or output port identifier.
 * @param[in] bitRate - New codec bitrate; one of ::SceVoiceBitRate.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceSetBitRate(SceVoicePortId portId, SceUInt32 bitRate);

/**
 * Get the bitrate of an encoded input or output port.
 *
 * @param[in] portId - Encoded input or output port identifier.
 * @param[out] bitRate - Receives the configured codec bitrate; one of
 *                       ::SceVoiceBitRate.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetBitRate(SceVoicePortId portId, SceUInt32 *bitRate);

/**
 * Set a port attribute.
 *
 * This entry point is exported but FW 3.60 rejects every attribute for an
 * existing port with ::SCE_VOICE_ERROR_INVALID_ARGUMENT. A valid-range port ID
 * that does not identify a port produces ::SCE_VOICE_ERROR_INVALID_PORT_ID.
 *
 * @param[in] portId - Port identifier.
 * @param[in] attr - Attribute selector.
 * @param[in] value - Attribute value.
 *
 * @return A negative ::SceVoiceErrorCode value on FW 3.60.
 */
int sceVoiceSetPortAttr(SceVoicePortId portId, SceVoicePortAttr attr, const SceInt32 *value);

/**
 * Get a port attribute.
 *
 * ::SCE_VOICE_PORT_ATTR_AUDIO_INPUT_OWNERSHIP produces a defined value only
 * for a ::SCE_VOICE_PORT_TYPE_IN_DEVICE port on FW 3.60. Other port types
 * return success without modifying @p value.
 *
 * @param[in] portId - Port identifier.
 * @param[in] attr - Attribute selector.
 * @param[out] value - Receives the attribute value.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetPortAttr(SceVoicePortId portId, SceVoicePortAttr attr, SceInt32 *value);

/**
 * Create a Voice port.
 *
 * PCMAUDIO ports accept signed 16-bit little-endian PCM at 16000 Hz. Their
 * effective byte capacity is rounded down to a multiple of 512 and must be
 * from 1024 through 64512 bytes. VOICE ports use one of ::SceVoiceBitRate.
 * FW 3.60 truncates the rounded PCM capacity to 16 bits before validating it;
 * callers should pass a value in the documented range rather than relying on
 * that wraparound.
 *
 * For application-fed input ports, ::SceVoicePortParam::threshold must be
 * shorter than the duration represented by the port's buffer. The
 * type-specific union member must be zero for a device port.
 *
 * @param[out] portId - Receives a port identifier from 0 through 63.
 * @param[in] param - Port configuration.
 *
 * FW 3.60 writes 0xFF to @p portId before validation. If immediate backend
 * startup fails, the function returns that error after creating the port and
 * replacing 0xFF with its valid port ID.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceCreatePort(SceVoicePortId *portId, const SceVoicePortParam *param);

/**
 * Update an existing port's configuration.
 *
 * The port type cannot be changed. Changing an encoded bitrate recreates the
 * port backend and clears buffered data.
 *
 * Do not change a PCMAUDIO port's `data.bufferSize` on FW 3.60. The function
 * never reallocates or reinitializes the PCM rings. The first update stores
 * the new requested size while retaining the old derived accounting size; a
 * later update derives the accounting size from that stored value. Deleting
 * the port then releases resources according to the changed accounting size,
 * not the capacity originally allocated. The update path also does not
 * revalidate ::SceVoicePortParam::threshold against the queue duration.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[in] param - Replacement configuration with the same port type.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceUpdatePort(SceVoicePortId portId, const SceVoicePortParam *param);

/**
 * Connect an input port to an output port.
 *
 * Input types 0 through 2 can connect to output types 3 through 5. FW 3.60
 * rejects a direct IN_VOICE to OUT_VOICE connection and duplicate edges. An
 * instance can contain at most 128 connections.
 *
 * @param[in] inputPortId - Device, PCM, or encoded input port.
 * @param[in] outputPortId - PCM, encoded, or device output port.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceConnectIPortToOPort(SceVoicePortId inputPortId, SceVoicePortId outputPortId);

/**
 * Disconnect an input port from an output port.
 *
 * @param[in] inputPortId - Connected input port.
 * @param[in] outputPortId - Connected output port.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceDisconnectIPortFromOPort(SceVoicePortId inputPortId, SceVoicePortId outputPortId);

/**
 * Delete a Voice port and remove its connections.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceDeletePort(SceVoicePortId portId);

/**
 * Write application data to a PCM or encoded input port.
 *
 * On return, @p size contains the number of bytes accepted. PCM writes ignore
 * @p frameGap. For encoded input, a nonnegative frame gap inserts that many
 * missing frames before the supplied data. A negative value begins backfilling
 * previously marked missing frames at that relative frame offset behind the
 * current stream write point.
 *
 * @param[in] portId - ::SCE_VOICE_PORT_TYPE_IN_PCMAUDIO or
 *                     ::SCE_VOICE_PORT_TYPE_IN_VOICE port.
 * @param[in] data - Data to write.
 * @param[in,out] size - Requested byte count and resulting accepted byte count.
 * @param[in] frameGap - Encoded-stream frame gap or negative backfill offset;
 *                       ignored for PCM input.
 *
 * When the operation fails after argument validation, FW 3.60 sets @p size to
 * zero.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceWriteToIPort(SceVoicePortId portId, const void *data, SceSize *size, SceInt16 frameGap);

/**
 * Read application data from a PCM or encoded output port.
 *
 * On return, @p size contains the number of bytes copied. Encoded reads are
 * rounded down to complete codec frames; PCM reads are rounded down to an even
 * byte count.
 *
 * @param[in] portId - ::SCE_VOICE_PORT_TYPE_OUT_PCMAUDIO or
 *                     ::SCE_VOICE_PORT_TYPE_OUT_VOICE port.
 * @param[out] data - Destination buffer.
 * @param[in,out] size - Destination capacity and resulting byte count.
 *
 * When the operation fails after argument validation, FW 3.60 sets @p size to
 * zero.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceReadFromOPort(SceVoicePortId portId, void *data, SceSize *size);

/**
 * Get one port's type, processing state, available data, and frame size.
 *
 * If Voice has not been started, FW 3.60 clears the complete output structure
 * and returns ::SCE_VOICE_ERROR_NOT_STARTED. On success, it writes only
 * `portType`, `state`, `dataSize`, and `frameSize`; the two reserved members
 * are unchanged.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 * @param[out] info - Receives the port information.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceGetPortInfo(SceVoicePortId portId, SceVoicePortInfo *info);

/**
 * Clear one port's buffered data and reset its processing state.
 *
 * An ACTIVE port becomes IDLE. INACTIVE, IDLE, and BUFFERING ports retain
 * their current internal state. The state visible through
 * ::sceVoiceGetPortInfo is updated by a later processing synchronization.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceResetPort(SceVoicePortId portId);

/**
 * Pause processing for one port.
 *
 * Pausing retains buffered data. Application-fed input ports retain their
 * internal processing state; device and output processing paths change their
 * internal state to INACTIVE. FW 3.60 skips publication while a port is
 * paused, so ::sceVoiceGetPortInfo can continue reporting the state published
 * before the pause until processing resumes.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoicePausePort(SceVoicePortId portId);

/**
 * Resume processing for one port.
 *
 * @param[in] portId - Port identifier returned by ::sceVoiceCreatePort.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceResumePort(SceVoicePortId portId);

/**
 * Pause processing for every port in the process-local Voice instance.
 *
 * Pausing retains buffered data and each port's internal processing state.
 * FW 3.60 also retains the previously published state, so
 * ::sceVoiceGetPortInfo can continue reporting that state while all ports are
 * paused.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoicePausePortAll(void);

/**
 * Resume processing for every port in the process-local Voice instance.
 *
 * @return 0 on success, or a negative ::SceVoiceErrorCode value.
 */
int sceVoiceResumePortAll(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_VOICE_H_ */
