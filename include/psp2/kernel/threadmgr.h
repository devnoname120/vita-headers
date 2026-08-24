/**
 * \usergroup{SceThreadMgr}
 * \usage{psp2/kernel/threadmgr.h,SceKernelThreadMgr_stub}
 */

#ifndef _PSP2_KERNEL_THREADMGR_H_
#define _PSP2_KERNEL_THREADMGR_H_

#include <vitasdk/build_utils.h>
#include <psp2/kernel/threadmgr/callback.h>
#include <psp2/kernel/threadmgr/cond.h>
#include <psp2/kernel/threadmgr/eventflag.h>
#include <psp2/kernel/threadmgr/lw_cond.h>
#include <psp2/kernel/threadmgr/lw_mutex.h>
#include <psp2/kernel/threadmgr/msgpipe.h>
#include <psp2/kernel/threadmgr/mutex.h>
#include <psp2/kernel/threadmgr/rwlock.h>
#include <psp2/kernel/threadmgr/semaphore.h>
#include <psp2/kernel/threadmgr/signal.h>
#include <psp2/kernel/threadmgr/thread.h>
#include <psp2/kernel/threadmgr/vfp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sceKernelGetThreadInfo_opt {
	SceSize infoSize; //!< Number of bytes to copy to and from pInfo.
	SceUInt32 unused; //!< Ignored; the public wrapper leaves this word uninitialized.
} _sceKernelGetThreadInfo_opt;
VITASDK_BUILD_ASSERT_EQ(8, _sceKernelGetThreadInfo_opt); // size is from FW 3.60

typedef struct _sceKernelGetMutexInfo_opt {
	SceSize infoSize; //!< Number of bytes to copy to and from pInfo.
	SceUInt32 unused; //!< Ignored; the public wrapper leaves this word uninitialized.
} _sceKernelGetMutexInfo_opt;
VITASDK_BUILD_ASSERT_EQ(8, _sceKernelGetMutexInfo_opt); // size is from FW 3.60

typedef struct sceKernelCreateLwMutex_opt {
	SceInt32 initCount; //!< Initial lock count.
	const SceKernelLwMutexOptParam *pOptParam; //!< Optional lightweight mutex parameters.
	SceUInt32 unused[2]; //!< Ignored.
} sceKernelCreateLwMutex_opt;
VITASDK_BUILD_ASSERT_EQ(0x10, sceKernelCreateLwMutex_opt); // size is from FW 3.60

typedef struct sceKernelRegisterThreadEventHandlerOpt {
	SceKernelThreadEventHandler handler; //!< Event callback.
	void *common; //!< Value passed to the handler.
	SceUInt32 unused[2]; //!< Ignored.
} sceKernelRegisterThreadEventHandlerOpt;
VITASDK_BUILD_ASSERT_EQ(0x10, sceKernelRegisterThreadEventHandlerOpt); // size is from FW 3.60

typedef struct sceKernelCreateThreadForUser_opt {
	SceSize size; //!< Ignored on FW 3.60; the public wrapper sets it to 0x18.
	SceSize stackSize; //!< Thread stack size.
	SceUInt32 attr; //!< Thread attributes.
	SceInt32 cpuAffinityMask; //!< CPU affinity encoded in bits 16-19.
	const SceKernelThreadOptParam *pOptParam; //!< Optional thread parameters.
	SceUIntVAddr callerAddress; //!< Wrapper return address used to identify the caller module.
} sceKernelCreateThreadForUser_opt;
VITASDK_BUILD_ASSERT_EQ(0x18, sceKernelCreateThreadForUser_opt); // size is from FW 3.60

/**
 * Creates a lightweight mutex through the raw user export.
 *
 * The work area must be 8-byte aligned and remain valid until the lightweight
 * mutex is deleted.
 *
 * @param[in,out] pWork Lightweight mutex work area.
 * @param[in] pName Required mutex name, limited to 31 bytes plus NUL.
 * @param[in] attr Mutex attributes.
 * @param[in] pOpt Required 0x10-byte wrapper option block. Only
 *                          ::sceKernelCreateLwMutex_opt::initCount and
 *                          ::sceKernelCreateLwMutex_opt::pOptParam are used.
 *
 * @return 0 on success, or < 0 on error.
 */
int __sceKernelCreateLwMutex(SceKernelLwMutexWork *pWork, const char *pName, SceUInt32 attr, const sceKernelCreateLwMutex_opt *pOpt);

/**
 * Retrieves size-prefixed mutex information through the raw user export.
 *
 * @param[in] mutexId Mutex identifier.
 * @param[in,out] pInfo Mutex information buffer.
 * @param[in] pOpt Required option block selecting the number of bytes copied.
 *
 * @return 0 on success, or < 0 on error.
 */
int _sceKernelGetMutexInfo(SceUID mutexId, SceKernelMutexInfo *pInfo, const _sceKernelGetMutexInfo_opt *pOpt);

/**
 * Retrieves size-prefixed thread information through the raw user export.
 *
 * @param[in] threadId Thread identifier.
 * @param[in,out] pInfo Thread information buffer.
 * @param[in] pOpt Required option block selecting the number of bytes copied.
 *
 * @return 0 on success, or < 0 on error.
 */
int _sceKernelGetThreadInfo(SceUID threadId, SceKernelThreadInfo *pInfo, const _sceKernelGetThreadInfo_opt *pOpt);

/**
 * Registers a thread event handler through the raw user export.
 *
 * The returned handler must be released with
 * ::sceKernelUnregisterThreadEventHandler when it is no longer needed.
 *
 * @param[in] name Required handler name, limited to 31 bytes plus NUL.
 * @param[in] threadId Target thread identifier.
 * @param[in] mask Bitwise OR of ::SceKernelThreadEventType values.
 * @param[in] pOpt Required option block containing the callback and common argument.
 *
 * @return Handler identifier on success, or < 0 on error.
 */
SceUID _sceKernelRegisterThreadEventHandler(const char *name, SceUID threadId, SceUInt32 mask, const sceKernelRegisterThreadEventHandlerOpt *pOpt);

/**
 * Creates a dormant user thread through the raw four-argument export.
 *
 * @param[in] pName Required thread name, limited to 31 bytes plus NUL.
 * @param[in] entry Thread entry point.
 * @param[in] initPriority Initial thread priority.
 * @param[in] pOpt Required 0x18-byte wrapper option block. Its leading
 *                          ::sceKernelCreateThreadForUser_opt::size field is
 *                          ignored on FW 3.60, but should be initialized to
 *                          the size of the structure.
 *
 * @return Thread identifier on success, or < 0 on error.
 */
SceUID sceKernelCreateThreadForUser(const char *pName, SceKernelThreadEntry entry, SceInt32 initPriority, const sceKernelCreateThreadForUser_opt *pOpt);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_KERNEL_THREADMGR_H_ */
