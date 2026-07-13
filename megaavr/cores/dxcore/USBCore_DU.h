/**
 * USBCore_DU.h - C-callable bridge between the AVR DU low-level USB stack
 *                (usb_core.c / usb_standard.c / usb_descriptors.c, all C)
 *                and the C++ PluggableUSB framework (api/PluggableUSB.cpp).
 *
 * Implementation in USBCore_DU.cpp.
 */
#pragma once
#if defined(USB0)

#include <stdint.h>
#include <stdbool.h>
#include "usb_core.h"   /* for usb_setup_t */

#ifdef __cplusplus
extern "C" {
#endif

/* --- Accumulator buffer (control-IN descriptor assembly) ---------------- */
/* The accumulator backs USB_SendControl: each call appends bytes to a
 * static buffer; the caller (handle_get_descriptor in usb_standard.c)
 * then sends the buffer with the existing ep0_start_data_in().            */
void           usbcore_acc_reset(void);
const uint8_t *usbcore_acc_buf(void);
uint16_t       usbcore_acc_len(void);

/* --- Dynamic CONFIGURATION descriptor builder --------------------------- *
 * Resets the accumulator, emits the CDC interfaces (always present), then
 * iterates the PluggableUSB modules' getInterface() to append the rest.
 * Fixes up wTotalLength and bNumInterfaces in the accumulator header.     */
void usbcore_build_config_descriptor(void);

/* --- PluggableUSB dispatch hooks (called from usb_standard.c) ----------- *
 * `try_setup`        - non-CDC class / vendor requests
 * `try_get_descriptor` - non-standard descriptor types (HID report, etc.) *
 * Both return true if a registered PluggableUSB module handled it.        *
 * On true return, send accumulator buffer (which the module filled via    *
 * USB_SendControl) as the control-IN response; on false, STALL.           */
bool usbcore_try_plugged_setup(const usb_setup_t *s);
bool usbcore_try_plugged_get_descriptor(const usb_setup_t *s);

/* --- Endpoint configuration ---------------------------------------------- *
 * Iterate the epBuffer[] entries (filled by PluggableUSB.plug at static    *
 * init) and program the DU endpoint table accordingly. Call from           *
 * handle_set_configuration after CDC EPs have been configured.            */
void usbcore_init_plugged_endpoints(void);

/* --- TRNCOMPL servicing for dynamic EPs (called from USB ISR) ----------- *
 * For now this just clears flags; data movement for those EPs is handled  *
 * by USB_Send/Recv via polling (blocking writes).                         */
void usbcore_service_dynamic_ep_trncompl(void);

/* --- Control-OUT data stage for plugged (HID) host->device requests ----- *
 * HID SET_REPORT (keyboard LED state, feature reports, ...) carries its     *
 * payload in an EP0 OUT data stage that must be received BEFORE the owning  *
 * PluggableUSB module can consume it. Because our SETUP handler runs in ISR *
 * context we cannot block inside the module's setup() the way the 32U4 core *
 * does; instead we stage the OUT data into a buffer, then re-dispatch       *
 * setup() once it has landed, so the module's USB_RecvControl() reads it    *
 * synchronously - identical API semantics to the Arduino AVR core.         *
 *                                                                           *
 *   usbcore_ctrl_out_begin()   - save SETUP, return armed length (0=reject) *
 *   usbcore_ctrl_out_buf()     - EP0 OUT landing buffer to arm              *
 *   usbcore_ctrl_out_pending() - a plugged ctrl-OUT is awaiting its data    *
 *   usbcore_ctrl_out_dispatch()- data landed: re-run the module's setup()   */
uint16_t usbcore_ctrl_out_begin(const usb_setup_t *s);
uint8_t *usbcore_ctrl_out_buf(void);
bool     usbcore_ctrl_out_pending(void);
void     usbcore_ctrl_out_dispatch(void);

#ifdef __cplusplus
}
#endif

#endif /* USB0 */
