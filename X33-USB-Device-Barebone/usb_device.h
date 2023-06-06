#ifndef USB_DEVICE_H
#define USB_DEVICE_H
/**
 * @file    usb_device.h
 *
 * @brief   Implementation of a USB Device on the STK3700. 
 * 
 * @note    For now, NO support for OTG.
 * 
 * @note    For now, it supports ONLY Energy Level EM0 (Run mode). 
 * 
 * @note    This implementation is specific for the STK3700 board. It does not
 *          use the DMPU signal. So, it should not work as a low speed device
 *          but probably will work.
 * 
 * @note    The core is designed to be *interrupt-driven*. Polling interrupt 
 *          mechanism is not recommended: this may result in undefined
 *          resolutions.
 *
 * 
 * @author Hans
 * @version 0.1
 * @date 2021-11-19
 * 
 */ 
#include <stdint.h>

#include "usb_descriptors.h"
/**
 * @brief Power state
 * 
 */
typedef enum {
    USB_POWER_UNDEF, 
    USB_POWER_UP, 
    USB_POWER_DOWN 
} USB_PowerStatus_t;


/**
 * @brief  USB interface state
 * 
 */
typedef enum {
    USB_STATE_DISABLED,
    USB_STATE_POWERED,
    USB_STATE_ATTACHED,
    USB_STATE_CONFIGURED,
    USB_STATE_ADDRESSED,
    USB_STATE_SUSPENDED,
    USB_STATE_UNDEFINED
} USB_State_t;

/**
 * @brief Configuration flags for USB_Init
 */
#define USB_CONF_LOWSPEED                    1
#define USB_CONF_FULLSPEED                   2
#define USB_CONF_BUSPOWERED                  4
#define USB_CONF_SELFPOWERED                 8
#define USB_CONF_VBUSENPOLARITY             16
#define USB_CONF_DEBUG                    1024

int  USB_Init(uint32_t conf);

void USB_Delay(unsigned ms);
void USB_ResetCore(void);
void USB_Stop(void);

int USB_Read( uint8_t *data, uint16_t len, uint16_t addr);
int USB_Write( uint8_t*data, uint16_t maxlen, uint16_t addr);


typedef enum { LOW_SPEED, FULL_SPEED } USB_Speed_t;


typedef struct {
    USB_DeviceDescriptor        *device_descriptor;
    USB_InterfaceDescriptor     *interface_descriptor;
    USB_ConfigurationDescriptor *configuration_descriptor;
    
    USB_Speed_t                 speed;
    USB_State_t                 state;
    USB_State_t                 saved_state;
    USB_State_t                 last_state;
    USB_PowerStatus_t           power_status;
    uint16_t                    address;
    uint8_t                     num_strings;
    uint8_t                     num_interfaces;

} USB_DevInfo_t;

extern USB_DevInfo_t USB_Devinfo;
    
#endif // USB_DEVICE_H