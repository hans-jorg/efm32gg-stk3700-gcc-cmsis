/**
 * @file usb_endpoints.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "usb_endpoints.h"
#include "usb_device.h"

void USD_Endpoint_Finish(USB_Endpoint_t *ep) {

    if(   (ep->status == USB_EP_TRANSMITTING) 
       || (ep->status == USB_EP_TRANSMITTING) ) {
           ep->status = USB_EP_IDLE;
       }
}

void USB_Endpoint_Process_Endpint_0(USB_Endpoint_t *ep) {


    switch(ep->status) {
    case USB_EP_IDLE:

        break;
    case USB_EP_RECEIVING:

        break;
    case USB_EP_TRANSMITTING:


        break;
    case USB_EP_STATUS_IN_EP0:

        break;
    case USB_EP_STATUS_OUT_EP0:
        
        ep->status = USB_EP_IDLE;
        break;
    }

}


void USB_RestartSetup(void) {

    //USB_Devinfo


}