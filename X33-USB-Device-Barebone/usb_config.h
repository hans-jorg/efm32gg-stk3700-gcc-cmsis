#ifndef USB_CONFIG_H
#define USB_CONFIG_H
/**
 * @file usb_config.h
 * 
 * @brief General USB Configuration parameters
 * 
 * 
 * @author Hans
 * @brief 
 * @version 0.1
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdint.h>
#include "usb_packets.h"


/**
 * @brief  USB_NUM_ENDPOINTS_USED
 * 
 * @note   Must include the control endpoints (0)
 */
#ifndef USB_NUM_ENDPOINTS_USED
#define USB_NUM_ENDPOINTS_USED          (4)
#endif

/**
 * @brief Vendor ID
 * 
 * @note  For now, use Silicon Labs (?)
 */

#ifndef USB_VENDORID
#define USB_VENDORID                 0x10C4
#endif


/**
 * @brief Product ID
 *
 * @note  For now, use 1 
 */

#ifndef USB_PRODUCTID
#define USB_PRODUCTID                 0x0001
#endif


/**
 * @brief Device ID
 *
 * @note  For now, use 1 
 */

#ifndef USB_DEVICEID
#define USB_DEVICEID                 0x0001
#endif


/**
 * @brief Configuration for USB
 * 
/**
 * @note  Define USB_IMPLEMENTS_GPIO_IRQ_ROUTINE to force the implementation 
 *        of the GPIO IRQ routine in this module. 
 *       Comment out this definition if it is implemented elsewhere
 */
//#define USB_IMPLEMENTS_GPIO_IRQ_ROUTINE     (1)
/**
 * @brief Define USB_VBUSEN when using OTG
 * 
 * @note  It can be defined too when using USB as a device
 * 
 */
#define USB_USE_VBUSEN                      (1)

/**
 * @brief Features of the USB interface on the EFM32
 * 
 */
///@{
#define USB_CORECLOCKFREQUENCY              (48000000)
#define USB_MAXPACKETSIZE_FULLSPEED         (64)
#define USB_MAXPACKETSIZE_LOWSPEED          (8)
#define USB_MAXENDPOINTNUMIN                (6)
#define USB_MAXENDPOINTNUMIN                (6)
///@}

/**
 * @brief GPIO Definition for USB
 *
 * @note  Some pins must be configured BEFORE passed to USB controller
 */
///@{
#define USB_GPIO1PORT                GPIOF
#define USB_GPIO1PIN_VBUSEN          5
#define USB_GPIO1PIN_FAULT           6
#define USB_GPIO1PIN_DM              10
#define USB_GPIO1PIN_DP              11
#define USB_GPIO1PIN_ID              12

#define USB_GPIO2PORT                GPIOD
#define USB_GPIO2PIN_DMPU            2
///@}


#endif // USB_CONFIG_H