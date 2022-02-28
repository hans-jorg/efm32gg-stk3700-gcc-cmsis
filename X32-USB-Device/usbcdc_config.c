/**
 * @file usbcdc_config.c
 *
 * @brief Info mainly obtained from Gecko example
 *
 * @author your name (you@domain.com)
 * @version 0.1
 * @date 2021-12-14
 *
 * @copyright Copyright (c) 2021
 *
 */


#include "em_device.h"
#include "clock_efm32gg2.h"
#include "gpio.h"
#include "usb_packets.h"
#include "usb_descriptors.h"
#include "usbcdc_descriptors.h"

#ifndef USB_USE_OWN_CONFIG
#include "usb_config.h"
#endif

#define USB_FULLSPEED_BULK_MAXPACKET                    64
#define USB_FULLSPEED_INTR_MAXPACKET                    64
#define USB_SLOWSPEED_BULK_MAXPACKET                     8
#define USB_SLOWSPEED_INTR_MAXPACKET                     8

#define DATAENDPOINT_OUT                                0x01
#define DATAENDPOINT_IN                                 0x81
#define NOTIFYENDPOINT_IN                               0x82



ALIGNED const struct USB_DeviceDescriptor DeviceDescriptor = {
    .bLength              =  USB_DESCSIZE_DEVICE,
    .bDescriptorType      =  USB_DESCTYPE_DEVICE,
    .bcdUSB               =  USB_DEVDESC_VERSION_20,
    .bDeviceClass         =  USB_DEVDESC_DEVCLASS_CDC,
    .bDeviceSubclass      =  0,
    .bDeviceProtocol      =  0,
    .bMaxPacketSize0      =  USB_FULLSPEED_BULK_MAXPACKET,
    .idVendor             =  USB_VENDORID,   // From Gecko example
    .idProduct            =  USB_PRODUCTID,  // From Gecko example
    .bcdDevice            =  USB_DEVICEID,   // From Gecko example
    .iManufacturer        =  1,              // Index# on String Descriptor
    .iProduct             =  2,              // Index# on String Descriptor
    .iSerialNumber        =  0,              // Index# on String Descriptor
    .bNumConfigurations   =  1               // #Configurations
};


/* Configuration Descriptor */
const USB_ConfigurationDescriptor ConfigurationDescriptor = {
    .bLength              =  USB_DESCSIZE_CONFIGURATION,
    .bDescriptorType      =  USB_DESCTYPE_CONFIGURATION,
    .wTotalLength         =  0, // Will be filled later
    .bNumInterfaces       =  2,
    .bConfigurationValue  =  1,
    .iConfiguration       =  0,
    .bmAttributes         =  USB_CONFDESC_ATTRIBUTES_SELFPOWERED,
    .bMaxPower            =  50
};

/* Interface Descriptor #0: Communication */
const USB_InterfaceDescriptor InterfaceDescriptorComm = {
     .bLength             =  USB_DESCSIZE_INTERFACE,
     .bDescriptorType     =  USB_DESCTYPE_INTERFACE,
     .bInterfaceNumber    =  0,
     .bAlternateSetting   =  0,
     .bNumEndpoints       =  1,
     .bInterfaceClass     =  USBCDC_COMMINTCLASS,
     .bInterfaceSubclass  =  USBCDC_COMMDEVSUBCLASS_ACM,
     .bInterfaceProtocol  =  0,
     .iInterface          =  0
};

/* Interface Descriptor #1: Data */
const USB_InterfaceDescriptor InterfaceDescriptorData = {
     .bLength             =  USB_DESCSIZE_INTERFACE,
     .bDescriptorType     =  USB_DESCTYPE_INTERFACE,
     .bInterfaceNumber    =  1,
     .bAlternateSetting   =  0,
     .bNumEndpoints       =  2,
     .bInterfaceClass     =  USBCDC_DATAINTCLASS,
     .bInterfaceSubclass  =  USBCDC_DATAINTSUBCLASS,
     .bInterfaceProtocol  =  USBCDC_DATAINTPROT_NONE,
     .iInterface          =  0
};


const USB_EndpointDescriptor EndpointDescriptorCommIN = {
     .bLength             =  USB_DESCSIZE_ENDPOINT,
     .bDescriptorType     =  USB_DESCTYPE_ENDPOINT,
     .bEndpointAddress    =  NOTIFYENDPOINT_IN,
     .bmAttributes        =  USB_ENDPDESC_ATTRIBUTES_INTERRUPT,
     .wMaxPacketSize      =  USB_FULLSPEED_INTR_MAXPACKET,
     .bInterval           =  0xFF
};

/** Endpoints for Data */
const USB_EndpointDescriptor EndpointDescriptorDataIN = {
     .bLength             =  USB_DESCSIZE_ENDPOINT,
     .bDescriptorType     =  USB_DESCTYPE_ENDPOINT,
     .bEndpointAddress    =  DATAENDPOINT_IN,
     .bmAttributes        =  USB_ENDPDESC_ATTRIBUTES_BULK,
     .wMaxPacketSize      =  USB_FULLSPEED_BULK_MAXPACKET,
     .bInterval           =  0
};
                          
const USB_EndpointDescriptor EndpointDescriptorDataOUT = {
     .bLength             =  USB_DESCSIZE_ENDPOINT,
     .bDescriptorType     =  USB_DESCTYPE_ENDPOINT,
     .bEndpointAddress    =  DATAENDPOINT_OUT,
     .bmAttributes        =  USB_ENDPDESC_ATTRIBUTES_BULK,
     .wMaxPacketSize      =  USB_FULLSPEED_BULK_MAXPACKET,
     .bInterval           =  0
};

const USBCDC_HeaderDescriptor HeaderDescriptor = {
     .bFunctionLength     =  USBCDC_DESCSIZE_HEADER,
     .bDescriptorType     =  USBCDC_INTERFACE,
     .bDescriptorSubType  =  USBCDC_DESCSUBTYPE_HEADER,
     .bcdDCD              =  USBCDC_VERSION_12 
};


const USBCDC_CallManagementDescriptor CallManagementDescriptor = {
     .bFunctionLength     =  USBCDC_DESCSIZE_CALL_MANAGEMENT,
     .bDescriptorType     =  USBCDC_INTERFACE,
     .bDescriptorSubType  =  USBCDC_DESCSUBTYPE_CALL_MANAGEMENT,
     .bmCapabilities      =  0,
     .bDataInterface      =  1,
};

const USBCDC_ACManagementDescriptor ACManagementDescriptor = {
     .bFunctionLength     =  USBCDC_DESCSIZE_ACM_MANAGEMENT,
     .bDescriptorType     =  USBCDC_INTERFACE,
     .bDescriptorSubType  =  USBCDC_DESCSUBTYPE_ACM_MANAGEMENT,
     .bmCapabilities      =  0x02
};

/**
 * @brief Configuration lists for Configuration and Strings
 */

USB_GeneralDescriptor *Configuration[] = {
    /* Configuration  */
   (USB_GeneralDescriptor *)   &DeviceDescriptor,
   /* Communication Interface */
   (USB_GeneralDescriptor *)   &InterfaceDescriptorComm,
   (USB_GeneralDescriptor *)   &HeaderDescriptor,
   (USB_GeneralDescriptor *)   &CallManagementDescriptor,
   (USB_GeneralDescriptor *)   &ACManagementDescriptor,
   /* Data Interface */
   (USB_GeneralDescriptor *)   &InterfaceDescriptorData,
   (USB_GeneralDescriptor *)   &EndpointDescriptorDataIN,
   (USB_GeneralDescriptor *)   &EndpointDescriptorDataOUT,
   (USB_GeneralDescriptor *)   0
};



/* String Descriptors */
DECLARELANGUAGEDESC(LanguageString, 1, 0x409 );
DECLARESTRINGDESC(ManufacturerString,11, 
                'E','x','a','m','p','l','e','.','c','o','m' );
DECLARESTRINGDESC(ProductString, 13,
                'E','F','M','3','2','-','S','T','K','3','7','0','0' );


const struct USB_StringDescriptorEPN const *Strings[] = {
   (const struct USB_StringDescriptorEPN *const) LanguageString,
   ManufacturerString,
   ProductString,
   0
};