/**
 * \kernelgroup{SceSuspend}
 * \usage{psp2kern/kernel/suspend.h,SceKernelSuspendForDriver_stub}
 */


#ifndef _PSP2KERN_KERNEL_SUSPEND_H_
#define _PSP2KERN_KERNEL_SUSPEND_H_

#include <psp2common/kernel/processmgr.h>
#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * System suspend/resume event handler.
 *
 * @param[in] resume - Zero while suspending, or one while resuming.
 * @param[in] eventId - Event identifier.
 * @param[in] eventParam - Dispatch-specific event data.
 * @param[in] args - Private argument supplied when the handler was registered.
 *
 * @return 0 on success, < 0 on error.
 */
typedef int (*SceSysEventHandler)(int resume, int eventId, void *eventParam, void *args);


/**
 * Cancel specified idle timers to prevent entering in power save processing.
 *
 * @param[in] type - One of ::SceKernelPowerTickType
 *
 * @return 0
*/
int ksceKernelPowerTick(SceKernelPowerTickType type);

/**
 * Register system event handler
 *
 * @param[in] name - Name of handler
 * @param[in] handler - The handler
 * @param[in] args - Handler arguments
 *
 * @return 0 on success, < 0 on error.
*/
int ksceKernelRegisterSysEventHandler(const char *name, SceSysEventHandler handler, void *args);

/**
 * Dispatches a suspend or resume event to all registered handlers.
 *
 * If a handler fails and \p failedHandlerId is non-NULL, dispatch stops and the
 * failing handler UID is returned through it. If \p failedHandlerId is NULL,
 * dispatch continues after handler failures and returns the last handler result.
 *
 * @param[in] resume - Zero while suspending, or one while resuming.
 * @param[in] eventId - Event identifier.
 * @param[in] eventParam - Dispatch-specific event data forwarded to each handler.
 * @param[out] failedHandlerId - Optional output for the first failing handler UID.
 *
 * @return The last invoked handler result, or 0 when no handler is registered.
 */
int ksceKernelSysEventDispatch(SceBool resume, SceUInt32 eventId, void *eventParam, SceUID *failedHandlerId);

/**
 * Unregisters a system event handler.
 *
 * @param[in] handlerId - UID returned by ::ksceKernelRegisterSysEventHandler.
 *
 * @return 0 on success, < 0 on error.
 */
int ksceKernelUnregisterSysEventHandler(SceUID handlerId);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_KERNEL_SUSPEND_H_ */
