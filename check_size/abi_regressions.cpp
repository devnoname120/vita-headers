#include <stddef.h>
#include <type_traits>
#include <psp2/coredump.h>
#include <psp2/lsdb.h>
#include <psp2/npdrmpackage.h>
#include <psp2/voice.h>
#include <psp2kern/appmgr.h>
#include <psp2kern/coredump.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/sysmem.h>

// Structure-size checks alone miss short enums hidden by padding.
#define CHECK_WORD_OFFSET(type, member, offset) \
	static_assert(sizeof(((type *)0)->member) == 4, #type "::" #member " must be a word"); \
	static_assert(offsetof(type, member) == offset, #type "::" #member " offset mismatch")

CHECK_WORD_OFFSET(SceLsdbAppInfoFilter, operation, 0x14);
static_assert(offsetof(SceLsdbAppInfoFilter, mask) == 0x18, "AppInfo filter mask offset mismatch");
static_assert(offsetof(SceLsdbAppInfoFilter, next) == 0x20, "AppInfo filter next offset mismatch");

CHECK_WORD_OFFSET(SceLsdbLiveAreaObject, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaBackgroundImage, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaSystemFunctionZone, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaStartupImage, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaTitleColor, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaFrameElement, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaVisualElement, type, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaVisualElement, horizontalAlignment, 0x8);
CHECK_WORD_OFFSET(SceLsdbLiveAreaVisualElement, verticalAlignment, 0xC);
CHECK_WORD_OFFSET(SceLsdbLiveAreaImageElement, origin, 0x30);
CHECK_WORD_OFFSET(SceLsdbLiveAreaTextElement, textAlignment, 0x24);
CHECK_WORD_OFFSET(SceLsdbLiveAreaTextElement, verticalTextAlignment, 0x28);
CHECK_WORD_OFFSET(SceLsdbLiveAreaTextElement, lineAlignment, 0x2C);
CHECK_WORD_OFFSET(SceLsdbLiveAreaTextElement, origin, 0x30);
CHECK_WORD_OFFSET(SceLsdbLiveAreaFrame, objectType, 0x4);
CHECK_WORD_OFFSET(SceLsdbLiveAreaFrame, multiMode, 0x14);
CHECK_WORD_OFFSET(SceLsdbLiveAreaParser, compatibilityMode, 0x28);

CHECK_WORD_OFFSET(SceVoiceEvent, eventType, 0x0);
CHECK_WORD_OFFSET(SceVoiceInitParam, applicationType, 0x0);
CHECK_WORD_OFFSET(SceVoicePortParam, portType, 0x0);
CHECK_WORD_OFFSET(SceVoicePortParam, data.bitRate, 0xC);
CHECK_WORD_OFFSET(SceVoicePortParam, pcmDataType, 0x10);
CHECK_WORD_OFFSET(SceVoicePortParam, samplingRate, 0x14);
static_assert(offsetof(SceVoicePortParam, threshold) == 0x4, "Voice threshold offset mismatch");
static_assert(offsetof(SceVoicePortParam, muteFlag) == 0x6, "Voice mute flag offset mismatch");
CHECK_WORD_OFFSET(SceVoicePortInfo, portType, 0x0);
CHECK_WORD_OFFSET(SceVoicePortInfo, state, 0x4);
CHECK_WORD_OFFSET(SceVoicePortInfo, dataSize, 0xC);
CHECK_WORD_OFFSET(SceVoicePortInfo, frameSize, 0x10);

CHECK_WORD_OFFSET(SceAppMgrLaunchParam, processExitSpawnMode, 0x20);
CHECK_WORD_OFFSET(SceCoredumpTriggerParam, output_mode, 0x8);
CHECK_WORD_OFFSET(SceKernelHeapMemoryOpt, mappingAction, 0x4);
CHECK_WORD_OFFSET(SceKernelModuleListInfo, nid, 0x48);

#undef CHECK_WORD_OFFSET

static_assert(std::is_same<decltype(&sceLsdbGetLiveAreaActivityDisabled),
	SceUInt32 (*)(const SceLsdbAppInfo *)>::value,
	"Preserve the activity-flag getter signature");
static_assert(std::is_same<decltype(&sceLsdbGetLiveAreaPreviewDisabled),
	int (*)(const SceLsdbAppInfo *)>::value,
	"Preserve the preview-flag getter signature");
static_assert(std::is_same<decltype(&sceLsdbGetExtendedMemoryMode),
	SceInt64 (*)(const SceLsdbAppInfo *)>::value,
	"The extended-memory mode getter must keep its 64-bit return type");
static_assert(std::is_same<decltype(&sceLsdbIsLiveAreaActivitySupported),
	int (*)(const SceLsdbAppInfo *)>::value,
	"Preserve the activity-support query signature");
static_assert(SCE_LSDB_EXTENDED_MEMORY_MODE_DEFAULT == 0 &&
	SCE_LSDB_EXTENDED_MEMORY_MODE_29_MIB == 1 &&
	SCE_LSDB_EXTENDED_MEMORY_MODE_77_MIB == 2 &&
	SCE_LSDB_EXTENDED_MEMORY_MODE_109_MIB == 3,
	"Extended-memory modes are selectors, not byte counts");

static_assert(std::is_same<SceCoredumpHandler, int (*)(void *)>::value,
	"The public coredump callback takes the registered argument");
static_assert(std::is_same<decltype(&sceVoiceSetBitRate), int (*)(SceVoicePortId, SceUInt32)>::value,
	"Voice bitrate input must accept a 32-bit value");
static_assert(std::is_same<decltype(&sceVoiceGetBitRate), int (*)(SceVoicePortId, SceUInt32 *)>::value,
	"Voice bitrate output must point to a 32-bit word");
static_assert(std::is_same<decltype(&_sceNpDrmPackageDecrypt),
	int (*)(void *, SceSize, SceNpDrmPackageDecryptOpt *)>::value,
	"Preserve the package decrypt function-pointer type");
static_assert(std::is_same<decltype(&ksceKernelMemBlockTypeGetPrivileges),
	int (*)(SceKernelMemBlockType)>::value,
	"Preserve the integer permission-mask return type");

// Compile the documented bitmask call pattern without an explicit enum cast.
void checkPermissionMaskCalls(ScePID pid, const void *base, SceSize size, void **physicalAddress)
{
	ksceKernelIsAccessibleRange(SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, base, size);
	ksceKernelIsAccessibleRangeProc(pid, SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, base, size);
	ksceKernelIsEqualAccessibleRangeProcBySW(pid, SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, base, size);
	ksceKernelProcModeVAtoPA(pid, SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, base, physicalAddress);
}
