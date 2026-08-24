/**
 * \kernelgroup{SceDeci4pDbgp}
 * \usage{psp2kern/deci4p.h,SceDeci4pDbgpForDriver_stub}
 */

#ifndef _PSP2KERN_DECI4P_H_
#define _PSP2KERN_DECI4P_H_

#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DTrace breakpoint callback.
 *
 * The callback runs from SceDeci4pDbgp's prefetch-abort hook for a user-mode
 * breakpoint, with IRQs enabled. Exactly one opcode argument is initialized:
 * \a arm_opcode when \a is_thumb is false, or \a thumb_opcode when it is true.
 * The other opcode argument contains indeterminate exception-stack data and
 * must not be read. The opcode value uses the CPU's native little-endian
 * representation.
 *
 * The handler registered by ::ksceDbgpSetDTraceUsdtHandler is called for ARM
 * opcode `0xE1200970` or Thumb opcode `0xBE90`, both of which encode
 * `BKPT #0x90`. A nonnegative result handles that breakpoint and advances the
 * saved PC; a negative result lets Dbgp continue normal debugger handling.
 * The handler registered by ::ksceDbgpSetDTraceBreakpointHandler is called
 * for ARM opcode `0xE1200971` or Thumb opcode `0xBE91`, which encode
 * `BKPT #0x91`; SceDeci4pDbgp ignores its return value.
 *
 * @param[in] pid          Process containing the faulting thread.
 * @param[in] thid         Faulting thread ID.
 * @param[in] bkpt_addr    Address of the breakpoint instruction in \a pid.
 * @param[in] sp           Saved stack pointer at the breakpoint.
 * @param[in] cpsr         Saved CPSR at the breakpoint.
 * @param[in] arm_opcode   ARM instruction word; valid only when \a is_thumb is false.
 * @param[in] thumb_opcode Thumb instruction halfword; valid only when \a is_thumb is true.
 * @param[in] is_thumb     Nonzero when the fault occurred in Thumb state.
 */
typedef SceInt32 (SceDbgpBreakpointHandler)(ScePID pid, SceUID thid,
	ScePVoid bkpt_addr, SceUIntPtr sp, SceUInt32 cpsr,
	SceUInt32 arm_opcode, SceUInt16 thumb_opcode, SceBool is_thumb);

/**
 * Get the DTrace fasttrap `BKPT #0x91` handler.
 *
 * @return The currently registered handler, or NULL if none is registered.
 */
SceDbgpBreakpointHandler *ksceDbgpGetDTraceBreakpointHandler(void);

/**
 * Get the DTrace USDT `BKPT #0x90` handler.
 *
 * @return The currently registered handler, or NULL if none is registered.
 */
SceDbgpBreakpointHandler *ksceDbgpGetDTraceUsdtHandler(void);

/**
 * Set the DTrace fasttrap `BKPT #0x91` handler.
 *
 * The implementation stores the raw pointer without locking, validation,
 * reference counting, or ownership transfer. The callback and its containing
 * module must remain valid while registered.
 *
 * @param[in] handler Handler to register, or NULL to unregister it.
 */
void ksceDbgpSetDTraceBreakpointHandler(SceDbgpBreakpointHandler *handler);

/**
 * Set the DTrace USDT `BKPT #0x90` handler.
 *
 * The implementation stores the raw pointer without locking, validation,
 * reference counting, or ownership transfer. The callback and its containing
 * module must remain valid while registered.
 *
 * @param[in] handler Handler to register, or NULL to unregister it.
 */
void ksceDbgpSetDTraceUsdtHandler(SceDbgpBreakpointHandler *handler);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_DECI4P_H_ */
