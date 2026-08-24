/**
 * \usergroup{SceVshBridge}
 * \usage{psp2/vshbridge.h,SceVshBridge_stub}
 */

#ifndef _PSP2_VSHBRIDGE_H_
#define _PSP2_VSHBRIDGE_H_

#include <vitasdk/build_utils.h>
#include <psp2common/appmgr.h>
#include <psp2common/kernel/msif.h>
#include <psp2/types.h>
#include <psp2common/sblssmgr.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/modulemgr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SceVshMountId {
    SCE_VSH_MOUNT_SD0  = 0x00100,
    SCE_VSH_MOUNT_OS0  = 0x00200,
    SCE_VSH_MOUNT_VS0  = 0x00300,
    SCE_VSH_MOUNT_VD0  = 0x00400,
    SCE_VSH_MOUNT_TM0  = 0x00500,
    SCE_VSH_MOUNT_UR0  = 0x00600,
    SCE_VSH_MOUNT_UD0  = 0x00700,
    SCE_VSH_MOUNT_UX0  = 0x00800,
    SCE_VSH_MOUNT_GRO0 = 0x00900,
    SCE_VSH_MOUNT_GRW0 = 0x00A00,
    SCE_VSH_MOUNT_SA0  = 0x00B00,
    SCE_VSH_MOUNT_PD0  = 0x00C00,
    SCE_VSH_MOUNT_IMC0 = 0x00D00,
    SCE_VSH_MOUNT_XMC0 = 0x00E00,
    SCE_VSH_MOUNT_UMA0 = 0x00F00,
    SCE_VSH_MOUNT_LMA0 = 0x10000,
    SCE_VSH_MOUNT_LMB0 = 0x20000,
    SCE_VSH_MOUNT_MFA0 = 0x50000,
    SCE_VSH_MOUNT_MFB0 = 0x60000,
} SceVshMountId;

/**
 * Gets real system firmware information.
 *
 * @param[out] data - firmware information.
 */
int _vshSblGetSystemSwVersion(SceKernelFwInfo *data);

/**
 * @brief Get service/manufacturing information (factory/minimum firmware).
 *
 * @param[out] info - The info output pointer
 *
 * @return 0 on success, < 0 on error.
 */
int _vshSblAimgrGetSMI(SceUInt32 *info);

/**
 * Get the console identifier.
 *
 * @param[out] consoleId - Receives 16 bytes with the layout of ::SceConsoleId.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, < 0 on error.
 */
int _vshSblAimgrGetConsoleId(char consoleId[0x10]);

/**
 * @brief Check if a module is loaded.
 *
 * @param[in] module_name - The module's name
 * @param[in] buffer - Set a buffer with a size of 8 bytes
 *
 * @return the SceUID of the module on success, < 0 on error.
 */
SceUID _vshKernelSearchModuleByName(const char *module_name, const void *buffer);

/**
 * @brief Mount a partition.
 *
 * @param[in] id - Mount ID (one of ::SceVshMountId).
 * @param[in] path - Mount path (optional, can be NULL).
 * @param[in] permission - `1` for read-only, `2` for read-write.
 * @param[in] buf - Work buffer. Allocate `0x100` and (important!) fill with `0`.
 *
 * @return `>= 0` on success, `< 0` on error.
 */
int _vshIoMount(SceVshMountId id, const char *path, int permission, void *buf);

/**
 * @brief Unmount a partition.
 *
 * @warning Unmounting system partitions can cause system instability, even if
 *          you remount them back. For example, FIOS overlays used by the Shell
 *          won't get recreated and would effectively render the Shell unusable
 *
 * @param[in] id - Mount ID (one of ::SceVshMountId).
 * @param[in] force - `1` to force unmount, `0` otherwise.
 * @param[in] unk2 - Unknown, set to `0`.
 * @param[in] unk3 - Unknown, set to `0`.
 *
 * @return `>= 0` on success, `< 0` on error.
 */
int vshIoUmount(SceVshMountId id, int force, int unk2, int unk3);

int vshIdStorageIsDirty(void);
int vshIdStorageIsFormatted(void);
int vshIdStorageIsReadOnly(void);

/**
 * @param[in]  leafnum - The read target leaf number
 * @param[out] buf     - The leaf data buffer pointer, size is 512 byte
 *
 * @return 0 on success, < 0 on error.
 */
int vshIdStorageReadLeaf(SceSize leafnum, void *buf);

/**
 * @param[in] leafnum - The write target leaf number
 * @param[in] buf     - The leaf data buffer pointer, size is 512 byte
 *
 * @return 0 on success, < 0 on error.
 *
 * note - Writing to leaf requires manufacturing mode.
 */
int vshIdStorageWriteLeaf(SceSize leafnum, const void *buf);

/**
 * Verify a eboot.pbp signature "__sce_ebootpbp"
 *
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  eboot_signature        - The pointer of data of __sce_ebootpbp signature. size is 0x200
 * @param[in]  eboot_signature_magic  - The pointer of a pointer of magic number within __sce_ebootpbp, "NPUMDSIG" or "PSISOSIG"
 *
 * @return 0 on success, < 0 on error.
*/
int _vshNpDrmEbootSigVerify(const char *eboot_pbp_path, const char *eboot_signature, char** eboot_signature_header);

/**
 * Verify an older 0x100 byte eboot.pbp signature "__sce_ebootpbp" from firmware <2.00
 *
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  eboot_signature        - The pointer of data of __sce_ebootpbp signature. size is 0x100
 * @param[in]  eboot_signature_magic  - The pointer of a pointer of magic number within __sce_ebootpbp, "NPUMDSIG" or "PSISOSIG"
 *
 * @return 0 on success, < 0 on error.
*/
int _vshNpDrmPspEbootVerify(const char *eboot_pbp_path, const char *eboot_signature, char** eboot_signature_header);

/**
 * Generate an older 0x100 byte eboot.pbp signature "__sce_ebootpbp" for a PSP game - this is unused in firmware >2.00
 * 
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  eboot_sha256           - The pointer of SHA256 hash of first (data.psar offset + 0x1C0000) bytes into the EBOOT.PBP file
 * @param[out] eboot_signature        - The pointer of the output eboot signature data. size is 0x100
 *
 * @return eboot_signature size on success, < 0 on error.
*/
int _vshNpDrmPspEbootSigGen(const char *eboot_pbp_path, const void *eboot_sha256, void *eboot_signature);

/**
 * Convert an older 0x100 byte eboot.pbp signature "__sce_ebootpbp" to a 0x200 byte one used in firmwares >2.00
 * 
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  old_eboot_signature    - The pointer of old eboot signature data. size is 0x100
 * @param[out] new_eboot_signature    - The pointer of new eboot signature data. size is 0x200
 *
 * @return eboot_signature size on success, < 0 on error.
*/
int _vshNpDrmEbootSigConvert(const char *eboot_pbp_path, const void* old_eboot_signature, void* new_eboot_signature); 


/**
 * Generate eboot.pbp signature "__sce_ebootpbp" for a PSP game
 *
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  eboot_sha256           - The pointer of SHA256 hash of first (data.psar offset + 0x1C0000) bytes into the EBOOT.PBP file
 * @param[out] eboot_signature        - The pointer of the output eboot signature data. size is 0x200
 * @param[in]  sw_version             - The pointer of the minimum firmware version the signature can be used on. cannot be lower than current firmware
 *
 * @return eboot_signature size on success, < 0 on error.
*/
int _vshNpDrmEbootSigGenPsp(const char *eboot_pbp_path, const void* eboot_sha256, void *eboot_signature, int *sw_version);


/**
 * Generate eboot.pbp signature "__sce_ebootpbp" for a single-disc PS1 game
 *
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  eboot_sha256           - The pointer of SHA256 hash of first (data.psar offset + 0x1C0000) bytes into the EBOOT.PBP file
 * @param[out] eboot_signature        - The pointer of the output eboot signature data. size is 0x200
 * @param[in]  sw_version             - The pointer of the minimum firmware version the signature can be used on. cannot be lower than current firmware
 *
 * @return eboot_signature size on success, < 0 on error.
*/
int _vshNpDrmEbootSigGenPs1(const char *eboot_pbp_path, const void *eboot_sha256, void *eboot_signature, int *sw_version);

/**
 * Generate eboot.pbp signature "__sce_discinfo" for a multi-disc PS1 game
 *
 * @param[in]  eboot_pbp_path         - The pointer of the file path of the EBOOT.PBP file
 * @param[in]  sce_discinfo           - The pointer of contents of vs0:/app/NPXS10028/__sce_discinfo
 * @param[out] eboot_signature        - The pointer of the output eboot signature data. size is 0x100
 * @param[in]  sw_version             - The pointer of the minimum firmware version the signature can be used on. cannot be lower than current firmware
 *
 * @return eboot_signature size on success, < 0 on error.
*/
int _vshNpDrmEbootSigGenMultiDisc(const char *eboot_pbp_path, const void *sce_discinfo, void *eboot_signature, int *sw_version);

int vshSblAimgrIsCEX(void);
int vshSblAimgrIsDEX(void);
int vshSblAimgrIsVITA(void);
int vshSblAimgrIsGenuineVITA(void);
int vshSblAimgrIsDolce(void);
int vshSblAimgrIsGenuineDolce(void);
int vshSblAimgrIsTest(void);
int vshSblAimgrIsTool(void);
int vshSblSsIsDevelopmentMode(void);

int vshSysconHasWWAN(void);

int vshSysconIsDownLoaderMode(void);
int vshSysconIsIduMode(void);
int vshSysconIsMCEmuCapable(void);
int vshSysconIsShowMode(void);

int vshSysconIduModeSet(void);
int vshSysconIduModeClear(void);

int vshSysconShowModeSet(void);
int vshSysconShowModeClear(void);

int vshMemoryCardGetCardInsertState(void);
int vshRemovableMemoryGetCardInsertState(void);

int vshMsifGetMsInfo(SceMsInfo *info);

typedef struct SceVshAppMgrCheckPfsMountedOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceVshAppMgrCheckPfsMountedOpt;
VITASDK_BUILD_ASSERT_EQ(8, SceVshAppMgrCheckPfsMountedOpt); // size is from FW 3.60

typedef struct SceVshAppMgrBgdlQueueStatusEntry {
	SceUInt32 downloadStatus; //!< Derived from the SceShell state nibble: 7 maps to 2, 8 to 1,
	                          //!< 9 to 3, 0xC to 4, and every other value to 0.
	SceBool unknownBoolean; //!< Set for SceShell state 9, or when an unidentified source word is zero,
	                        //!< flags 0x300 are clear, and flag 0x40000 is set. AppMgr
	                        //!< preserves but does not test this field; its purpose is unknown.
	char identifier[0x30]; //!< NUL-terminated BGDL identifier; AppMgr treats bytes 7 through 15 as a title ID.
} SceVshAppMgrBgdlQueueStatusEntry;
VITASDK_BUILD_ASSERT_EQ(0x38, SceVshAppMgrBgdlQueueStatusEntry); // size is from FW 3.60

typedef struct SceVshAppMgrBgdlQueueStatus {
	SceVshAppMgrBgdlQueueStatusEntry entries[32];
} SceVshAppMgrBgdlQueueStatus;
VITASDK_BUILD_ASSERT_EQ(0x700, SceVshAppMgrBgdlQueueStatus); // size is from FW 3.60

typedef struct SceVshSblAuthMgrVerifySpsfoOpt {
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceVshSblAuthMgrVerifySpsfoOpt;
VITASDK_BUILD_ASSERT_EQ(8, SceVshSblAuthMgrVerifySpsfoOpt); // size is from FW 3.60

typedef struct SceVshNpDrmGetLegacyDocKeyOpt {
	SceUInt8 *documentKey; //!< Destination for the 16-byte document key.
	SceSize documentDataSize; //!< Number of bytes to copy from the document data, at most 0x200.
	SceUInt32 reserved[2]; //!< Ignored on FW 3.60.
} SceVshNpDrmGetLegacyDocKeyOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, SceVshNpDrmGetLegacyDocKeyOpt); // size is from FW 3.60

typedef struct SceVshSblSsCreatePassPhraseOpt {
	SceSize passPhraseSize; //!< Number of pass-phrase bytes to copy, at most 0x200.
	SceUInt32 reserved; //!< Ignored on FW 3.60.
} SceVshSblSsCreatePassPhraseOpt;
VITASDK_BUILD_ASSERT_EQ(8, SceVshSblSsCreatePassPhraseOpt); // size is from FW 3.60

/**
 * Obtains add-on-content installation data.
 *
 * @param[in] titleId - Pointer to a 0x10-byte buffer containing a NUL-terminated, nine-character title ID.
 * @param[in] addcontId - Pointer to a 0x14-byte buffer containing a NUL-terminated, 16-character add-on-content ID.
 * @param[out] result - Output ::SceAppMgrAcInstResult structure containing the
 *                      add-on-content keystone and localized game title.
 *
 * @note The title ID must contain four uppercase letters followed by five
 *       decimal digits. The add-on-content ID must contain exactly 16
 *       uppercase alphanumeric characters. The calling process must be
 *       authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshAppMgrAcInstGetAcdirParam(const char titleId[0x10], const char addcontId[0x14], SceAppMgrAcInstResult *result);

/**
 * Updates the background-download queue status table.
 *
 * @param[in] queueStatus - Pointer to a ::SceVshAppMgrBgdlQueueStatus structure.
 *
 * The provider synchronously copies all 0x700 bytes; the caller retains
 * ownership of the structure.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshAppMgrBgdlSetQueueStatus(const SceVshAppMgrBgdlQueueStatus *queueStatus);

/**
 * Checks whether a PFS path is mounted.
 *
 * @param[in] path - NUL-terminated `ux0:` path shorter than 0x124 bytes.
 * @param[in] opt - Required 8-byte structure whose contents are ignored on FW 3.60.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 when the path is a mounted PFS directory, or a negative error code.
 */
int _vshAppMgrCheckPfsMounted(const char *path, const SceVshAppMgrCheckPfsMountedOpt *opt);

/**
 * Creates and signs a cloud-data header.
 *
 * @param[in] setupKey - 0x18-byte setup-key buffer. Its first 0x10 bytes contain the ASCII account ID.
 * @param[in] dataSize - Size of the cloud-data payload, at most 0x40000000 bytes.
 * @param[in] digest - 0x20-byte payload digest.
 * @param[out] header - 0x170-byte "PSVB" header containing the timestamp,
 *                     data size, account ID, digest, and RSA signature.
 *
 * The last eight setup-key bytes are copied by the bridge but ignored by
 * AppMgr on FW 3.60. An all-zero ASCII account ID is rejected. The output
 * header is cleared when creation fails.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshAppMgrCloudDataCreateHeader(const SceUInt8 setupKey[0x18], SceSize dataSize, const SceUInt8 digest[0x20], SceUInt8 header[0x170]);

/**
 * Changes selected attributes of a path.
 *
 * @param[in] file - NUL-terminated path of at most 0x3FF characters.
 * @param[in] stat - Input ::SceIoStat structure.
 * @param[in] bits - Attribute-selection mask.
 * @param[in] opt - Required 8-byte structure whose contents are ignored on FW 3.60.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshIoChstat(const char *file, const SceIoStat *stat, unsigned int bits, const sceIoChstatOpt *opt);

/**
 * Reads the next directory entry.
 *
 * @param[in] fd - Directory descriptor.
 * @param[in,out] dir - ::SceIoDirent structure copied both to and from the provider.
 *
 * The descriptor is a caller-process PUID and is converted to its kernel GUID
 * before the read operation.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 1 when an entry is read, 0 at end of directory, or a negative error code.
 */
int _vshIoDread(SceUID fd, SceIoDirent *dir);

/**
 * Gets path metadata.
 *
 * @param[in] file - NUL-terminated path of at most 0x3FF characters.
 * @param[out] stat - Output ::SceIoStat structure.
 * @param[in] opt - Required 8-byte structure whose contents are ignored on FW 3.60.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshIoGetstat(const char *file, SceIoStat *stat, const sceIoGetstatOpt *opt);

/**
 * Gets the compiled SDK version of a process.
 *
 * @param[in] pid - Process ID.
 * @param[out] sdkVersion - Result value of type ::SceUInt32.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshKernelGetCompiledSdkVersionByPid(ScePID pid, SceUInt32 *sdkVersion);

/**
 * Derives the 16-byte key for a legacy document.
 *
 * @param[in] rifData - Optional 0x200-byte RIF data buffer. Some PSPEDAT
 *                      document modes require it.
 * @param[in] documentData - PSPEDAT document data, at least 0x90 bytes.
 * @param[in] documentDataSize - Size of documentData in bytes; must match opt->documentDataSize.
 * @param[in] opt - Required copy-size and document-key output parameters.
 *
 * The raw bridge can copy at most 0x200 document bytes. The caller owns every
 * buffer, and the provider synchronously writes exactly 16 key bytes.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshNpDrmGetLegacyDocKey(const SceUInt8 rifData[0x200], const void *documentData, SceSize documentDataSize, const SceVshNpDrmGetLegacyDocKeyOpt *opt);

/**
 * Gets the primary platform security code.
 *
 * @param[out] pscode - Output ::ScePsCode structure.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSblAimgrGetPscode(ScePsCode *pscode);

/**
 * Gets the secondary platform security code.
 *
 * @param[out] pscode - Output ::ScePsCode structure.
 *
 * This variant obtains the value from the secure module instead of the cached
 * primary value and is available only while product mode is enabled.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSblAimgrGetPscode2(ScePsCode *pscode);

/**
 * Gets the 0x20-byte visible ID when the system is in product mode.
 *
 * @param[out] visibleId - Output ::SceVisibleId structure.
 *
 * @note Product mode must be enabled and the calling process must be authorized
 *       as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSblAimgrGetVisibleId(SceVisibleId *visibleId);

/**
 * Verifies a signed PARAM.SFO file and copies its embedded payload.
 *
 * @param[in] path - NUL-terminated path to the signed PARAM.SFO file, at most
 *                   0xFF characters.
 * @param[out] verifiedData - Output payload buffer.
 * @param[in] verifiedDataCapacity - Capacity of verifiedData in bytes.
 * @param[in] opt - Required 8-byte structure whose contents are ignored on FW 3.60.
 *
 * The verified embedded payload must be nonempty and fit completely in
 * verifiedData; partial copies are not performed.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSblAuthMgrVerifySpsfo(const char *path, void *verifiedData, SceSize verifiedDataCapacity, const SceVshSblAuthMgrVerifySpsfoOpt *opt);

/**
 * Creates a 0x200-byte account pass phrase.
 *
 * @param[in] args - Pass-phrase input parameters.
 * @param[out] passPhrase - Output pass-phrase buffer.
 * @param[in] opt - Required output-copy parameters.
 *
 * SceSblSsMgr always creates 0x200 bytes internally. The bridge copies the
 * first opt->passPhraseSize bytes, which must not exceed 0x200. All pointers
 * remain owned by the caller.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSblSsCreatePassPhrase(const SceSblSsCreatePassPhraseParam *args, void *passPhrase, const SceVshSblSsCreatePassPhraseOpt *opt);

/**
 * Gets the manufacturing-status word.
 *
 * @param[out] manufacturingStatus - Result value of type ::SceUInt32.
 *
 * FW 3.60 returns the four-byte response to Syscon command 0x15. No observed
 * importer interprets its individual bits, so their purposes are unknown.
 *
 * @note Product mode must be enabled and the calling process must be authorized
 *       as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int _vshSysconGetManufacturesStatus(SceUInt32 *manufacturingStatus);

/**
 * Creates a mount event.
 *
 * @param[in] mountId - One of ::SceVshMountId.
 * @param[in] eventBits - Bitwise OR of ::SceIoMountEvent values. FW 3.60
 *                        discards bits outside 0x333; at least one accepted bit
 *                        must remain.
 *
 * The returned UID is a PUID belonging to the calling process.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return Event UID on success, or a negative error code.
 */
SceUID vshIoCreateMountEvent(SceVshMountId mountId, SceUInt32 eventBits);

/**
 * Notifies the kernel that SceShell is ready.
 *
 * @param[in] eventId - Event ID. FW 3.60 accepts only 0, denoting that SceShell
 *                      is ready.
 *
 * The notification is sent to SceSysroot only on the first successful call;
 * later calls with zero still return success.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 0 on success, or a negative error code.
 */
int vshKernelSendSysEvent(SceUInt32 eventId);

/**
 * Reports whether the communication-test flag is set.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 1 when set, 0 when clear, or a negative authorization error.
 */
int vshSblUtMgrHasComTestFlag(void);

/**
 * Reports whether the NP-test flag is set.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 1 when set, 0 when clear, or a negative authorization error.
 */
int vshSblUtMgrHasNpTestFlag(void);

/**
 * Reports whether the store flag is set.
 *
 * @note The calling process must be authorized as a system program.
 *
 * @return 1 when set, 0 when clear, or a negative authorization error.
 */
int vshSblUtMgrHasStoreFlag(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_VSHBRIDGE_H_ */
