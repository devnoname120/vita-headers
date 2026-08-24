/**
 * \kernelgroup{SceCoredump}
 * \usage{psp2kern/coredump.h,SceCoredumpForDriver_stub}
 */


#ifndef _PSP2KERN_COREDUMP_H_
#define _PSP2KERN_COREDUMP_H_

#include <vitasdk/build_utils.h>
#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Coredump section-selection levels accepted by ::SceCoredumpTriggerParam. */
typedef enum SceCoredumpDumpLevel {
	SCE_COREDUMP_DUMP_LEVEL_CONFIGURED = 0,      //!< Use the FW 3.60 registry configuration.
	SCE_COREDUMP_DUMP_LEVEL_MINIMAL    = 0xF,    //!< Generate the minimal coredump.
	SCE_COREDUMP_DUMP_LEVEL_FULL       = 0x1EF0  //!< Generate the full coredump.
} SceCoredumpDumpLevel;

typedef enum SceCoredumpOutputMode {
	SCE_COREDUMP_OUTPUT_MODE_AUTO              = 0,  //!< Try the configured host0 path, then sd0, then ux0:data.
	SCE_COREDUMP_OUTPUT_MODE_HOST0             = 1,  //!< Write to the host0 root.
	SCE_COREDUMP_OUTPUT_MODE_UX0_DATA          = 2,  //!< Write to ux0:data.
	SCE_COREDUMP_OUTPUT_MODE_HOST0_CUSTOM_PATH = 10  //!< Write below a caller-supplied host0 path.
} SceCoredumpOutputMode;

/**
 * Options passed to ::ksceKernelSysrootCoredumpTrigger.
 *
 * FW 3.60 accepts a size-versioned prefix. Sizes from 4 through 7 select the
 * minimal dump level. Sizes from 8 through 19 also provide \c dump_level.
 * Sizes from 20 through 51 additionally provide the output mode and custom
 * path. A size of 0x34 or greater provides every field.
 */
typedef struct SceCoredumpTriggerParam {
	SceSize size; //!< Size of the provided structure prefix; normally `sizeof(SceCoredumpTriggerParam)`.
	int dump_level; //!< One of ::SceCoredumpDumpLevel; zero resolves to 0xF or 0x1EF0 from the registry.
	int output_mode; //!< One of ::SceCoredumpOutputMode.
	int custom_path_len; //!< Size of \c custom_path including its terminating NUL; maximum 0x400.
	int custom_path; //!< Pointer to the custom host0 subdirectory, represented as an \c int for backwards compatibility.
	SceSize titleid_len; //!< Number of bytes to copy from \c titleid; maximum 10.
	const char *titleid; //!< Title ID; required when \c titleid_len is nonzero.
	SceSize app_name_len; //!< Number of bytes to copy from \c app_name; maximum 0x80.
	const char *app_name; //!< Application name; required when \c app_name_len is nonzero.
	int app_version; //!< Decimal application version; for example, 100 is 01.00.
	int crash_cause; //!< -2 requests a manual dump, 1 identifies a GPU exception, and 3 identifies an AppMgr-detected hang.
	SceUID crash_thid; //!< Thread whose stop reason is inspected for crash causes that do not force one.
	int use_gpu_crash_filename; //!< Nonzero selects a GPUCRASH filename when \c crash_cause is 1.
} SceCoredumpTriggerParam;
VITASDK_BUILD_ASSERT_EQ(0x34, SceCoredumpTriggerParam); // size is from FW 3.60

/**
 * Coredump progress callback.
 *
 * FW 3.60 reports percentages from 0 through 100, with intermediate updates
 * quantized to five-percentage-point boundaries. The callback return value is
 * ignored.
 *
 * @param[in] task_id Coredump task identifier.
 * @param[in] pid Process identifier.
 * @param[in] progress Current coredump progress.
 */
typedef int (*SceKernelCoredumpStateUpdateCallback)(int task_id, ScePID pid, int progress);

/**
 * Coredump completion callback.
 *
 * @param[in] task_id Coredump task identifier.
 * @param[in] pid Process identifier.
 * @param[in] result Coredump operation result.
 * @param[in] path Path of the generated coredump.
 * @param[in] path_len Length of \p path including its terminating NUL.
 * @param[in] is_caf Nonzero if \p path identifies a CAF crash report; zero if
 * it identifies a plain PSP2 coredump.
 *
 * The path is owned by SceCoredump and must be copied before this callback
 * returns. The callback return value is ignored.
 */
typedef int (*SceKernelCoredumpStateFinishCallback)(int task_id, ScePID pid, int result, const char *path, SceSize path_len, int is_caf);

typedef enum SceCoredumpCafSegmentMode {
	SCE_COREDUMP_CAF_SEGMENT_MODE_HMAC_SHA256     = 0, //!< Authenticate the source without encryption.
	SCE_COREDUMP_CAF_SEGMENT_MODE_AES_CBC_ENCRYPT = 1, //!< Encrypt, then authenticate the ciphertext.
	SCE_COREDUMP_CAF_SEGMENT_MODE_AES_CBC_DECRYPT = 2  //!< Authenticate the ciphertext, then decrypt it.
} SceCoredumpCafSegmentMode;

/**
 * Initializes CAF cryptographic support.
 *
 * This validates and makes the coredump key stores available. A repeated call
 * succeeds when they are already available.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafInit(void);

/**
 * Creates a CAF cryptographic context.
 *
 * A context contains one mutable HMAC/AES operation state. Initialization of
 * a header or segment replaces that state, and its matching final function
 * clears it. Callers must serialize access to an individual context.
 *
 * @return The context UID on success, < 0 on error.
 */
SceUID ksceCoredumpCafContextCreate(void);

/**
 * Destroys a CAF cryptographic context.
 *
 * @param[in] ctx CAF context UID.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafContextDestroy(SceUID ctx);

/**
 * Generates a CAF initialization vector.
 *
 * @param[out] iv Buffer that receives the 16-byte initialization vector.
 * @param[in] iv_size Size of \p iv. Must be 16 bytes.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafCreateIv(void *iv, SceSize iv_size);

/**
 * Initializes the HMAC-SHA256 state for a CAF header.
 *
 * @param[in] ctx CAF context UID.
 * @param[in] hmac_key_id Selector from 1 through 3 for the 32-byte HMAC key.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafHeaderInit(SceUID ctx, SceUInt32 hmac_key_id);

/**
 * Adds data to the CAF header's HMAC-SHA256 calculation.
 *
 * @param[in] ctx CAF context UID.
 * @param[in] data Header data. May be NULL only when \p size is zero.
 * @param[in] size Number of bytes to authenticate.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafHeaderTransform(SceUID ctx, const void *data, SceSize size);

/**
 * Finalizes the CAF header and writes its HMAC-SHA256 digest.
 *
 * The header's cryptographic state is cleared after a successful call.
 *
 * @param[in] ctx CAF context UID.
 * @param[out] digest Buffer that receives the 32-byte digest. May be NULL only
 * when \p digest_size is zero.
 * @param[in] digest_size Size of \p digest. Must be at least 32 bytes when
 * \p digest is non-NULL.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafHeaderFinal(SceUID ctx, void *digest, SceSize digest_size);

/**
 * Finalizes the current CAF segment and writes its HMAC-SHA256 digest.
 *
 * The segment's cryptographic state is cleared after a successful call.
 *
 * @param[in] ctx CAF context UID.
 * @param[out] digest Buffer that receives the 32-byte digest, or NULL to
 * discard it.
 * @param[in] digest_size Size of \p digest. Must be at least 32 bytes when
 * \p digest is non-NULL, and zero when \p digest is NULL.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafSegmentFinal(SceUID ctx, void *digest, SceSize digest_size);

/**
 * Initializes the cryptographic state for a CAF segment.
 *
 * @param[in] ctx CAF context UID.
 * @param[in] hmac_key_id Selector from 1 through 3 for the 32-byte HMAC key.
 * @param[in] mode One of ::SceCoredumpCafSegmentMode.
 * @param[in] aes_key_id Selector from 1 through 4, or 0x10000001, for the
 * 16-byte AES key.
 * @param[in] iv Initialization vector. Exactly 16 bytes are read, including
 * in HMAC-only mode.
 * @param[in] iv_size Size of \p iv. Must be 16 bytes.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafSegmentInit(SceUID ctx, SceUInt32 hmac_key_id, SceCoredumpCafSegmentMode mode, SceUInt32 aes_key_id, const void *iv, SceSize iv_size);

/**
 * Transforms and authenticates CAF segment data.
 *
 * HMAC-only mode authenticates \p src without writing \p dst. Encryption mode
 * AES-CBC encrypts \p src to \p dst and authenticates the ciphertext.
 * Decryption mode authenticates the ciphertext in \p src before decrypting it
 * to \p dst.
 *
 * @param[in] ctx CAF context UID.
 * @param[in] src Source buffer. Must be non-NULL when \p size is nonzero.
 * @param[out] dst Destination buffer. Must be non-NULL when \p size is nonzero,
 * including in HMAC-only mode where it is not written.
 * @param[in] size Number of bytes to process. Must be a multiple of 16 in the
 * AES-CBC modes.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafSegmentTransform(SceUID ctx, const void *src, void *dst, SceSize size);

/**
 * Creates a coredump for a process and waits for completion.
 *
 * FW 3.60 selects plaintext PSP2DMP or encrypted CAF output from the coredump
 * configuration and target process. This function uses module-global wait and
 * path state and must not be called concurrently. Its private completion
 * callback deliberately ignores the worker's coredump result, so a return of
 * zero proves that the request completed but not that dump generation
 * succeeded.
 *
 * @param[in] pid Process identifier.
 * @param[in] titleid Title ID. Must be non-NULL.
 * @param[in] titleid_len Number of bytes to copy from \p titleid; maximum 10.
 * @param[in] app_name Application name. Must be non-NULL.
 * @param[in] app_name_len Number of bytes to copy from \p app_name; maximum
 * 0x80.
 * @param[in] app_version Decimal application version; for example, 100 is
 * 01.00.
 * @param[out] path Optional buffer that receives the generated coredump path.
 * @param[in] path_size Size of \p path. Must be nonzero when \p path is
 * non-NULL.
 *
 * @return 0 after the request completes, or < 0 on a request, wait, or cleanup
 * error. The worker's dump-generation result is not returned.
 */
int ksceCoredumpCreateDump(ScePID pid, const char *titleid, SceSize titleid_len, const char *app_name, SceSize app_name_len, int app_version, char *path, SceSize path_size);

/**
 * Deletes the fixed CAF crash-report file at ux0:data/crash_report.caf.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpDeleteCrashReportCaf(void);

/**
 * Finalizes CAF cryptographic support.
 *
 * The coredump key stores are securely cleared. Existing context objects are
 * not destroyed; the next header or segment initialization reloads the needed
 * keys lazily.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceCoredumpCafFinal(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_COREDUMP_H_ */
