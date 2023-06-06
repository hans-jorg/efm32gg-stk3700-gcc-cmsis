#ifndef USB_ENDPOINTS_H
#define USB_ENDPOINTS_H
/**
 * @file usb_endpoints.h
 * 
 * @brief USB Endpoints Management
 * 
 * @note  There are two way to manage endpoints: one use the endpoint number
 *        as an index and other, as a attribute. The latest requires a
 *        search to find the endpoint associated to an endpoint number
 * 
 * @author Hans (hans.schneebeli@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-27
 * 
 * @copyright Copyright (c) 2022
 * 
 * @note  
 */

#include <stdint.h>

#include "usb_descriptors.h"


typedef enum {
    USB_EP_FREE = 0, 
    USB_EP_IN   = 1, 
    USB_EP_OUT  = 2 
} USB_EP_Type;

typedef enum { 
    USB_EP_IDLE,
    USB_EP_TRANSMITTING,
    USB_EP_RECEIVING,
    USB_EP_STATUS_IN_EP0,
    USB_EP_STATUS_OUT_EP0
} USB_EP_Status;

typedef struct {
    USB_EP_Type     type;       ///< Free, IN, OUT
    USB_EP_Status   status;     ///< status; Idle, Txing, Rxing
    uint16_t        address;

    uint8_t         number;     ///< number of the corresponding endpoint
    
    uint8_t         *buffer;    ///<
    uint16_t        remaining;  ///<
    uint16_t        packetsize; ///<


} USB_Endpoint_t;





#endif // USB_ENDPOINTS_H
