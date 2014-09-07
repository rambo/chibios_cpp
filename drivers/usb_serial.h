#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include "ch.h"
#include "hal.h"

extern SerialUSBDriver SDU;

#ifdef __cplusplus
extern "C" {
#endif

void usb_serial_init(void);
#ifdef __cplusplus
}
#endif

#endif /* USB_SERIAL_H */
