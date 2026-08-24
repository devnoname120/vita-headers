/**
 * \kernelgroup{SceTouch}
 * \usage{psp2kern/touch.h,SceTouchForDriver_stub}
 */


#ifndef _PSP2KERN_TOUCH_H_
#define _PSP2KERN_TOUCH_H_

#include <psp2common/touch.h>
#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set touch enable flag
 *
 * @param[in] port   - The port number.
 * @param[in] enable - The enable flag.
 *
 * @return 0 on success. < 0 on error.
 */
int ksceTouchSetEnableFlag(SceUInt32 port, SceBool enable);

/**
 * Get touch-panel device information.
 *
 * The complete output is initialized to zero and then populated from the
 * selected panel's cached metadata while CPU interrupts are suspended under
 * the Touch state spinlock. This function does not require the panel to be
 * actively sampling.
 *
 * @param[in] panel - One of ::SceTouchPortType.
 * @param[out] pInfo - Receives the complete 0xC-byte device-information record.
 *
 * @return 0 on success, or ::SCE_TOUCH_ERROR_INVALID_ARG if \a panel is not
 *         ::SCE_TOUCH_PORT_FRONT or ::SCE_TOUCH_PORT_BACK, or if \a pInfo is
 *         NULL.
 */
int ksceTouchGetDeviceInfo(SceUInt32 panel, SceTouchDeviceInfo *pInfo);

/**
 * Set front and back touch-emulation data.
 *
 * Each argument is independently optional and, when non-NULL, points to one
 * 0x90-byte ::SceTouchData record for the corresponding panel. The FW 3.60
 * provider is a no-op: it does not read, retain, or modify either record.
 *
 * @param[in] pFrontData - Optional pointer to a front-panel ::SceTouchData.
 * @param[in] pBackData - Optional pointer to a back-panel ::SceTouchData.
 *
 * @return 0 on FW 3.60, including when both pointers are NULL.
 */
int ksceTouchSetTouchEmulationData(const void *pFrontData, const void *pBackData);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_TOUCH_H_ */
