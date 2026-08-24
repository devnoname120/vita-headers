/**
 * \kernelgroup{SceIofilemgr}
 * \usage{psp2kern/kernel/iofilemgr.h,SceIofilemgrForDriver_stub}
 */

#ifndef _PSP2KERN_KERNEL_IOFILEMGR_H_
#define _PSP2KERN_KERNEL_IOFILEMGR_H_

#include <vitasdk/build_utils.h>
#include <psp2kern/types.h>
#include <psp2kern/io/fcntl.h>
#include <psp2kern/io/devctl.h>
#include <psp2kern/io/stat.h>
#include <psp2kern/io/dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceIoFdInfo {
	SceUID fd;
	SceUID pid;
} SceIoFdInfo;
VITASDK_BUILD_ASSERT_EQ(8, SceIoFdInfo);

/**
  * Get global file descriptor
  *
  * @param[in]  vis_level - unknown, set 5
  * @param[out] dst       - fd list output
  * @param[in]  max_size  - dst array count
  * @param[out] res_size  - result array count output
  *
  * @return < 0 on error.
  */
int ksceIoGetGUIDFdListForDebugger(int vis_level, SceIoFdInfo *dst, SceSize max_size, SceSize *res_size);

#define ksceIoGetGUIDFdList ksceIoGetGUIDFdListForDebugger

/**
  * Get process file descriptor
  *
  * @param[in]  vis_level - unknown, set 5
  * @param[out] dst       - fd list output
  * @param[in]  max_size  - dst array count
  * @param[out] res_size  - result array count output
  *
  * @return < 0 on error.
  */
int ksceIoGetPUIDFdListForDebugger(int vis_level, SceIoFdInfo *dst, SceSize max_size, SceSize *res_size);

#define ksceIoGetPUIDFdList ksceIoGetPUIDFdListForDebugger

typedef struct SceIofileInfo { // size is 0x828
	char path[0x400];
	char path2[0x400];
	SceUID pid;
	int data_0x804;
	int data_0x808;
	int data_0x80C;
	int data_0x810;
	int data_0x814;
	int data_0x818;
	int data_0x81C;
	int data_0x820; // maybe media type
	int data_0x824;
} SceIofileInfo;
VITASDK_BUILD_ASSERT_EQ(0x828, SceIofileInfo);

/**
  * Get file info
  *
  * @param[in]  fd   - file descriptor
  * @param[in]  pid  - process id
  * @param[out] info - fd info output
  *
  * @return < 0 on error.
  */
int ksceIoGetFileInfo(SceUID fd, SceUID pid, SceIofileInfo *info);

/**
  * Mounts a device
  *
  * @param[in]  id          Device to mount
  * @param[in]  path        Where to mount to
  * @param[in]  permission  Permission flags
  * @param      a4          Unknown, set to 0
  * @param      a5          Unknown, set to 0
  * @param      a6          Unknown, set to 0
  *
  * @return < 0 on error.
  */
int ksceIoMount(int id, const char *path, int permission, int a4, int a5, int a6);

/**
  * Unmounts a device
  *
  * @param[in]  id    Device to unmount
  * @param[in]  a2    Unknown, set to 0
  * @param[in]  a3    Unknown, set to 0
  * @param[in]  a4    Unknown, set to 0
  *
  * @return < 0 on error.
  */
int ksceIoUmount(int id, int a2, int a3, int a4);

/**
  * Get Remote Kernel Process Local Storage Data
  *
  * @param[in]  pid - The target process id
  * @param[out] dst - The pointer of RemoteKPLS output buffer. size is 0x1C.
  *
  * @return < 0 on error.
  */
int ksceIoGetRemoteKPLSData(SceUID pid, void *dst);

typedef struct SceVfsMount SceVfsMount;
typedef struct SceVfsMountData SceVfsMountData;
typedef struct SceVfsVnode SceVfsVnode;

/** Event bits accepted by ::ksceIoCreateMountEvent. */
typedef enum SceIoMountEvent {
	SCE_IO_MOUNT_EVENT_MOUNTED          = 0x001, //!< A VFS mount operation succeeded.
	SCE_IO_MOUNT_EVENT_UNMOUNTED        = 0x002, //!< A VFS unmount operation succeeded.
	SCE_IO_MOUNT_EVENT_MOUNT_FAILED     = 0x010, //!< A VFS mount operation failed.
	SCE_IO_MOUNT_EVENT_UNMOUNT_FAILED   = 0x020, //!< A VFS unmount operation failed.
	SCE_IO_MOUNT_EVENT_DEVICE_ATTACHED  = 0x100, //!< The mount's backing device was attached.
	SCE_IO_MOUNT_EVENT_DEVICE_DETACHED  = 0x200  //!< The mount's backing device was detached.
} SceIoMountEvent;

/** Event bits accepted by ::ksceIoCreateErrorEvent. */
typedef enum SceIoErrorEvent {
	SCE_IO_ERROR_EVENT_OCCURRED = 0x1
} SceIoErrorEvent;

/**
 * One entry returned by ::vfsGetMntList.
 *
 * Pointer fields are borrowed kernel addresses owned by SceIofilemgr. They
 * must not be freed and can become invalid after the mount is removed.
 */
typedef struct SceIoMount {
	SceVfsMount *mnt; //!< Internal VFS mount.
	int vshMountId; //!< One of ::SceVshMountId, or -1 for a PFS mount.
	char assignName[0x20]; //!< Assigned Vita device name, such as `ux0:`.
	SceUInt32 openEntryCount; //!< Number of open entries on the mount.
	SceBool isPfsMount; //!< Nonzero when the mount uses PFS_GDSD_INF.
	void *implementationData; //!< Filesystem-implementation mount data.
} SceIoMount;
VITASDK_BUILD_ASSERT_EQ(0x34, SceIoMount); // size is from FW 3.60

/**
 * FW 3.60 snapshot returned by ::vfsGetMntInfo.
 *
 * Pointer fields are borrowed kernel addresses owned by SceIofilemgr. They
 * must not be freed and can become invalid as mount state changes.
 */
typedef struct SceIoMountInfo {
	SceVfsMount *mnt; //!< Internal VFS mount.
	int vshMountId; //!< One of ::SceVshMountId.
	char assignName[0x20]; //!< Assigned Vita device name.
	SceUInt32 openEntryCount; //!< Number of open entries on the mount.
	SceUInt32 state; //!< Internal mount state.
	SceUInt32 devMinor; //!< Internal device-minor value.
	SceUInt32 devMajor; //!< Internal device-major value.
	SceVfsMount *mountCopy; //!< Same pointer as ::SceIoMountInfo::mnt.
	SceVfsVnode *mountpoint; //!< Vnode on which this filesystem is mounted.
	SceUID poolUid; //!< Mount allocator UID.
	SceUInt32 referenceCount; //!< Internal mount reference count.
	SceUInt32 vnodeCount; //!< Number of vnodes belonging to the mount.
	SceVfsVnode *vnodeListHead; //!< First vnode belonging to the mount.
	SceVfsMount *mountedOnList; //!< First mount attached to this mount.
	SceVfsMount *mountedOnListPrev; //!< Previous entry in the mounted-on list.
	SceSize defaultIoCacheSize; //!< Default I/O-cache size.
	void *implementationData; //!< Filesystem-implementation mount data.
	void *fdLock; //!< Internal file-descriptor lock object.
	SceUID fdLockMutex; //!< File-descriptor lock mutex UID.
	SceUID fdLockCond; //!< File-descriptor lock condition-variable UID.
	SceUInt32 fdLockOperationState; //!< File-descriptor lock operation state.
	SceUInt32 fdLockWaiterCount; //!< Number of file-descriptor lock waiters.
	void *mountCc; //!< Internal mount cache-control object.
	SceUInt32 mountCcDirty; //!< Cache-control dirty value.
	SceUInt32 mountCcFlag; //!< Value referenced by the cache-control flag pointer.
	SceUID mountCcEventFlag; //!< Cache-control event-flag UID.
	SceVfsMountData **mountDataAddress; //!< Address of the mount-data pointer.
	char unixMountPath[0x400]; //!< Internal Unix mount path.
	SceVfsMountData *mountData; //!< Mount names and VSH mount ID descriptor.
	char filesystem[0x40]; //!< Filesystem implementation name.
	char blockDevicePrimary[0x40]; //!< Primary block-device name.
	char blockDeviceSecondary[0x40]; //!< Secondary block-device name.
} SceIoMountInfo;
VITASDK_BUILD_ASSERT_EQ(0x54C, SceIoMountInfo); // size is from FW 3.60

/** Path-mapping callback installed by ::ksceIoSetPathMappingFunction. */
typedef int (SceIoPathMappingFunc)(const char *path, ScePID pid, char *mappedPath, SceSize mappedPathSize);

typedef struct SceVfsChstatArgs {
	const char *path;
	const SceIoStat *stat;
	SceUInt32 cbit; //!< Bitwise OR of `SCE_CST_*` values.
	SceBool allowAttributeChange; //!< Preserve the internal 0x10000 attribute-change bit.
} SceVfsChstatArgs;
VITASDK_BUILD_ASSERT_EQ(0x10, SceVfsChstatArgs); // size is from FW 3.60

typedef struct SceVfsChstatByFdArgs {
	SceUID fd;
	const SceIoStat *stat;
	SceUInt32 cbit; //!< Bitwise OR of `SCE_CST_*` values.
} SceVfsChstatByFdArgs;
VITASDK_BUILD_ASSERT_EQ(0xC, SceVfsChstatByFdArgs); // size is from FW 3.60

typedef struct SceVfsCloseArgs {
	SceUID fd;
} SceVfsCloseArgs;
VITASDK_BUILD_ASSERT_EQ(0x4, SceVfsCloseArgs); // size is from FW 3.60

typedef struct SceVfsIoctlArgs {
	SceUID fd;
	int cmd;
	const void *indata; //!< Input buffer.
	SceSize inlen; //!< Size of ::SceVfsIoctlArgs::indata.
	void *outdata; //!< Output buffer.
	SceSize outlen; //!< Size of ::SceVfsIoctlArgs::outdata.
} SceVfsIoctlArgs;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVfsIoctlArgs); // size is from FW 3.60

typedef struct SceVfsOpenArgs {
	const char *filename;
	int flags; //!< Bitwise OR of ::SceIoMode values.
	SceMode mode; //!< File access permissions.
} SceVfsOpenArgs;
VITASDK_BUILD_ASSERT_EQ(0xC, SceVfsOpenArgs); // size is from FW 3.60

typedef struct SceVfsPreadArgs {
	SceUID fd;
	void *data;
	SceSize size;
	SceUInt32 padding; //!< Alignment padding; ignored on FW 3.60.
	SceOff offset;
} SceVfsPreadArgs;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVfsPreadArgs); // size is from FW 3.60

typedef struct SceVfsPwriteArgs {
	SceUID fd;
	const void *data;
	SceSize size;
	SceUInt32 padding; //!< Alignment padding; ignored on FW 3.60.
	SceOff offset;
} SceVfsPwriteArgs;
VITASDK_BUILD_ASSERT_EQ(0x18, SceVfsPwriteArgs); // size is from FW 3.60

typedef struct SceVfsReadArgs {
	SceUID fd;
	void *data;
	SceSize size;
} SceVfsReadArgs;
VITASDK_BUILD_ASSERT_EQ(0xC, SceVfsReadArgs); // size is from FW 3.60

typedef struct SceVfsWriteArgs {
	SceUID fd;
	const void *data;
	SceSize size;
} SceVfsWriteArgs;
VITASDK_BUILD_ASSERT_EQ(0xC, SceVfsWriteArgs); // size is from FW 3.60

/**
 * Caller-owned user-memory result block for ::ksceIoCreateErrorEvent.
 *
 * The reserved fields must be zero at creation. When the event is signaled,
 * FW 3.60 writes the other fields and resets the reserved fields to zero.
 */
typedef struct SceIoErrorEventInfo {
	SceUInt32 vshMountId; //!< Mount ID that produced the error.
	int errorCode; //!< I/O error code.
	SceUInt32 reserved1[2]; //!< Must be zero when creating the event.
	int errorDetail; //!< Additional error value supplied by the I/O subsystem.
	SceUInt32 reserved2[2]; //!< Must be zero when creating the event.
} SceIoErrorEventInfo;
VITASDK_BUILD_ASSERT_EQ(0x1C, SceIoErrorEventInfo); // size is from FW 3.60

/** VshBridge-facing chstat entry point that permits the internal attribute-change bit. */
int ksceIoChstatForVshbridge(const char *name, const SceIoStat *stat, unsigned int cbit);

/** Clear an error event's one-shot notification latch. */
int ksceIoClearErrorEvent(SceUID eventUid);

/**
 * Create an I/O error event.
 *
 * @param[in] vshMountId - Nonzero mount ID identifying a registered VSH mount.
 * @param[in] eventBits - Must be ::SCE_IO_ERROR_EVENT_OCCURRED.
 * @param[in,out] eventInfo - Required writable user-memory block. It must remain
 *                            valid until ::ksceIoDeleteErrorEvent is called.
 *
 * @return Kernel event GUID on success, or a negative error code.
 */
SceUID ksceIoCreateErrorEvent(int vshMountId, SceUInt32 eventBits, SceIoErrorEventInfo *eventInfo);

/**
 * Create a mount event.
 *
 * @param[in] vshMountId - Nonzero mount ID identifying a registered VSH mount.
 * @param[in] eventBits - Bitwise OR of ::SceIoMountEvent values. Bits outside
 *                        0x333 are discarded; at least one accepted bit is required.
 *
 * @return Kernel event GUID on success, or a negative error code.
 */
SceUID ksceIoCreateMountEvent(int vshMountId, SceUInt32 eventBits);

/** Delete an I/O error event by kernel GUID. */
int ksceIoDeleteErrorEvent(SceUID eventUid);

/** Delete a mount event by kernel GUID. */
int ksceIoDeleteMountEvent(SceUID eventUid);

/** VshBridge-facing directory read that preserves ::SceIoStat::st_attr. */
int ksceIoDreadForVshbridge(SceUID fd, SceIoDirent *dir);

/**
 * Gets the media type for a path.
 *
 * If a path-mapping callback is registered, the path is remapped for the
 * specified process before its media type is queried. A media-type query
 * failure is converted to success after writing the fallback media type 8.
 *
 * @param[in]  pid        - Process whose path mapping is used.
 * @param[in]  path       - Path to query.
 * @param[in]  ignored    - Unused ABI slot; ignored on FW 3.60.
 * @param[out] mediaType - Receives a 32-bit media-type value.
 *
 * @return The non-negative media-type query result, or a negative error from
 * path remapping or temporary-buffer allocation.
 */
int ksceIoGetMediaType(ScePID pid, const char *path, int ignored, SceUInt32 *mediaType);

/** Return the current thread's default I/O priority using the system-priority view. */
int ksceIoGetThreadDefaultPriorityForSystem(void);

/** VshBridge-facing getstat entry point that preserves ::SceIoStat::st_attr. */
int ksceIoGetstatForVshbridge(const char *name, SceIoStat *stat);

/** Perform an ioctl using kernel buffers. */
int ksceIoIoctl(SceUID fd, int cmd, const void *argp, SceSize arglen, void *bufp, SceSize buflen);

/** Open a path after resolving overlays in another process's namespace. */
SceUID ksceIoOpenForPid(ScePID pid, const char *filename, int flags, SceMode mode);

/**
 * Install or remove the process-path mapping callback.
 *
 * Only one callback can be installed. Passing NULL removes the current callback;
 * attempting to replace a non-NULL callback directly fails.
 */
int ksceIoSetPathMappingFunction(SceIoPathMappingFunc *func);

/** Synchronize a file descriptor. */
int ksceIoSyncByFd2(SceUID fd, int flags);

/** Allocate a dummy internal VFS file object named \a name. */
SceUID vfsAllocateFileDummy(int flags, SceBool isDir, const char *name);

/** Synchronous VFS chstat worker. */
int vfsChstat(SceVfsChstatArgs *args);

/** Synchronous VFS chstat-by-descriptor worker. */
int vfsChstatByFd(SceVfsChstatByFdArgs *args);

/** Synchronous VFS close worker. */
int vfsClose(SceVfsCloseArgs *args);

/**
 * Snapshot one mounted filesystem.
 *
 * @param[in] mnt - Mount pointer previously returned by ::vfsGetMntList.
 * @param[out] info - Receives the FW 3.60 mount snapshot.
 *
 * @return 0 on success, or a negative error when \a mnt is not mounted.
 */
int vfsGetMntInfo(SceVfsMount *mnt, SceIoMountInfo *info);

/**
 * Enumerate mounted filesystems.
 *
 * Passing NULL for \a mounts returns the current mount count directly. With a
 * non-NULL array, at most \a capacity entries are written, \a count receives
 * the number written, and the function returns 0.
 */
int vfsGetMntList(SceIoMount *mounts, SceSize capacity, SceSize *count);

/** Synchronous VFS ioctl worker. */
int vfsIoctl(SceVfsIoctlArgs *args);

/** Synchronous VFS open worker. */
SceUID vfsOpen(SceVfsOpenArgs *args);

/** Synchronous positional-read worker. */
SceSSize vfsPread(SceVfsPreadArgs *args);

/** Synchronous positional-write worker. */
SceSSize vfsPwrite(SceVfsPwriteArgs *args);

/** Synchronous read worker. */
SceSSize vfsRead(SceVfsReadArgs *args);

/** Synchronous write worker. */
SceSSize vfsWrite(SceVfsWriteArgs *args);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_KERNEL_IOFILEMGR_H_ */
