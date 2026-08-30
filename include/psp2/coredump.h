/**
 * \usergroup{SceCoredump}
 * \usage{psp2/coredump.h,SceCoredump_stub}
 */

#ifndef _PSP2_COREDUMP_H_
#define _PSP2_COREDUMP_H_

#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Coredump handler invoked as a normal Vita thread entry point.
 *
 * FW 3.60 starts the handler with \p args equal to 8 and \p argp pointing to a
 * transient two-word block containing `{handler, handler_arg}`. The second word
 * is the argument registered with ::sceCoredumpRegisterCoredumpHandler.
 *
 * @param[in] args Size of the transient argument block; 8 on FW 3.60.
 * @param[in] argp Pointer to the transient argument block described above.
 * @return The thread-entry return value; ignored by the coredump path.
 */
typedef int (*SceCoredumpHandler)(SceSize args, void *argp);

/**
 * Registers a callback that can append application-specific data to a coredump.
 *
 * The callback runs on a dedicated user thread whose stack has the requested
 * size. FW 3.60 waits up to three seconds for the callback. The target process
 * is suspended while it runs, so the callback must not issue ordinary system
 * calls; it should use ::sceCoredumpWriteUserData to contribute data.
 *
 * @param[in] handler Pointer to a ::SceCoredumpHandler function.
 * @param[in] stack_size Stack size of the thread used to invoke the handler.
 * Must be at least 0x1000 bytes on FW 3.60.
 * @param[in] handler_arg Argument stored in the handler's transient argument
 * block. When non-NULL, FW 3.60 probes exactly 4 readable bytes at registration
 * time. It must remain valid until the handler is unregistered.
 *
 * @retval 0 Success.
 * @retval 0x800A0000 Invalid handler, stack size, or handler argument, or
 * failure to allocate the handler stack.
 * @retval 0x800A0002 A handler is already registered for this process.
 */
int sceCoredumpRegisterCoredumpHandler(SceCoredumpHandler handler, SceSize stack_size, void *handler_arg);

/**
 * Unregisters the current process's coredump handler.
 *
 * @retval 0 Success.
 * @retval 0x800A0001 No handler is registered for this process.
 */
int sceCoredumpUnregisterCoredumpHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_COREDUMP_H_ */
