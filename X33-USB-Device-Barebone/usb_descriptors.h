#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H
/**
 * @file usb_descriptors.h
 * 
 * @brief USB Descriptors
 * 
 * @note  According USB Complete of Jan Axelson
 * 
 * @note  The structs must be packed. I.e., with internal padding.
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
 * @brief General format of descriptors
 * 
 */
struct PACKED USB_GeneralDescriptor {
    uint8_t     bLength;            // Descriptor size in bytes
    uint8_t     bType;              // 
    //....
};


/**
 * @brief Device Descriptor
 * 
 * @note  See symbols for bcdUSB below
 * 
 * @note  See symbols for DeviceClass below
 * 
 * @note bMaxPacketSize0
 * 
 *   | Speed | bMaxPacketSize0                  |
 *   |-------|----------------------------------|
 *   | Low   |     8                            |
 *   | Full  |     8, 16, 32 or 64              |
 *   | High  |     64                           |
 *   | Super |     512                          |
 *    
 *  For SuperSpeed, size is 2^bMaxPacketSize0 and the field must be set to 9.
 */

 struct PACKED USB_DeviceDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (18)
    uint8_t      bDescriptorType;   // The constant DEVICE (01h)
    uint16_t     bcdUSB;            // USB specification release number (BCD)
    uint8_t      bDeviceClass;      // Class code
    uint8_t      bDeviceSubclass;   // Subclass code
    uint8_t      bDeviceProtocol;   // Protocol Code
    uint8_t      bMaxPacketSize0;   // Maximum packet size for endpoint zero
    uint16_t     idVendor;          // Vendor ID
    uint16_t     idProduct;         // Product ID
    uint16_t     bcdDevice;         // Device release number (BCD)
    uint8_t      iManufacturer;     // Index of string descriptor for the manufacturer
    uint8_t      iProduct;          // Index of string descriptor for the product
    uint8_t      iSerialNumber;     // 
    uint8_t      bNumConfigurations;// Number of possible configurations
};


/**
 * @brief Device Qualifier Descriptor
 * 
 */
struct PACKED USB_DeviceQualifierDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (10)
    uint8_t      bDescriptorType;   // The constant DEVICE_QUALIFIER (06h)
    uint16_t     bcdUSB;            // USB specification release number (BCD)
    uint8_t      bDeviceClass;      // Class code
    uint8_t      bDeviceSubclass;   // Subclass code
    uint8_t      bDeviceProtocol;   // Protocol Code
    uint8_t      bMaxPacketSize0;   // Maximum packet size for endpoint zero
    uint8_t      bNumConfigurations;// Number of possible configurations
    uint8_t      Reserved;          // For future use
};


/**
 * @brief Configuration Descriptor
 * 
 * @note  It is the header of set of descriptors like
 * 
 *        * Interface Descriptor for Interface #0
 *          - Endpoint Descriptor #1
 *          - Endpoint Descriptor #2
 *        * Interface Descriptor for Interface #1
 *          - HID Descriptor
 *          - Endpoint Descriptor #1
 * 
 * @note  From "USB in a Nutshell": When the configuration descriptor is read, 
 *        it returns the entire configuration hierarchy which includes all 
 *        related interface and endpoint descriptors. The wTotalLength field 
 *        reflects the number of bytes in the hierarchy.
 * 
 * @note  bmAttributes
 *          6: Self-Powered
 *          5: Remote Wake-up
 * 
 * @note  bMaxPower is expressed in 2 mA units.
 *        50 corresponds to 50*2=100 mA
 */
struct PACKED USB_ConfigurationDescriptor {
    uint8_t      bLength;            // Descriptor size in bytes (9)
    uint8_t      bDescriptorType;    // The constant CONFIGURATION (02h)
    uint16_t     wTotalLength;       // The number of bytes in the configuration descriptor and all of its subordinate descriptors
    uint8_t      bNumInterfaces;     // Number of interfaces in the configuration
    uint8_t      bConfigurationValue;//Identifier for Set Configuration and Get Configuration requests
    uint8_t      iConfiguration;     // Index of string descriptor for the configuration
    uint8_t      bmAttributes;       // Self/bus power and remote wakeup settings
    uint8_t      bMaxPower;           // Bus power required in units of 2 mA (USB 2.0) or 8 mA (SuperSpeed).
};

/**
 * @brief Other Speed Descriptors
 * 
 */
struct PACKED USB_OtherSpeedDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (9)
    uint8_t      bDescriptorType;   // The constant OTHER_SPEED_CONFIGURATION (07h)
    uint16_t     wTotalLength;      // The number of bytes in the configuration descriptor and all of its subordinate descriptors
    uint8_t      bNumInterfaces;    // Number of interfaces in the configuration
    uint8_t      bConfigurationValue;//Identifier for Set Configuration and Get Configuration requests
    uint8_t      iConfiguration;    // Index of string descriptor for the configuration
    uint8_t      bmAttributes;      // Self/bus power and remote wakeup settings
    uint8_t      MaxPower;          // Bus power required in units of 2 mA (USB 2.0) or 8 mA (SuperSpeed).
};


/**
 * @brief Interface Association Descriptor
 * 
 */
struct PACKED USB_InterfaceAssociationDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (8)
    uint8_t      bDescriptorType;   // The constant Interface Association (0Bh)
    uint8_t      bFirstInterface;   // Number identifying the first interface associated with the function
    uint8_t      bInterfaceCount;   // The number of contiguous interfaces associated with the function
    uint8_t      bFunctionClass;    // Class code
    uint8_t      bFunctionSubClass; // Subclass code
    uint8_t      bFunctionProtocol; // Protocol code
    uint8_t      iFunction;         // Index of string descriptor for the function
};


/**
 * @brief Interface Descriptor
 * 
 * @note  
 */

struct PACKED USB_InterfaceDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (9)
    uint8_t      bDescriptorType;   // The constant Interface (04h)
    uint8_t      bInterfaceNumber;  // Number identifying this interface
    uint8_t      bAlternateSetting; // A number that identifies a descriptor with alternate settings for this bInterfaceNumber.
    uint8_t      bNumEndpoints;     // Number of endpoints supported not counting endpoint zero
    uint8_t      bInterfaceClass;   // Class code. See symbols below
    uint8_t      bInterfaceSubclass;// Subclass code
    uint8_t      bInterfaceProtocol;// Protocol code
    uint8_t      iInterface;        // Index of string descriptor for the interface
};

/**
 * @brief Endpoint Descriptor
 * 
 */
struct PACKED USB_EndpointDescriptor {
    uint8_t      bLength;           // Descriptor size in bytes (7)
    uint8_t      bDescriptorType;   // The constant Endpoint (05h)
    uint8_t      bEndpointAddress;  // Endpoint number and direction
    uint8_t      bmAttributes;      // Transfer type and supplementary information
    uint16_t     wMaxPacketSize;    // Maximum packet size supported
    uint8_t      bInterval;         // Service interval or NAK rate
};


/**
 * @brief SuperSpeed Endpoint Companion
 * 
 * @note  bMaxBurst is maximum number of packets the endpoint can send or 
 *        receive as part of a burst - 1. 
 * 
 * @note  bmAttributes is the maximum number of streams for bulk endpoints.
 *        For isochronous endpoints, it is the maximum number of packets in a 
 *        service interval.  
 * 
 * @note  wBytesPerInterval is the For periodic interrupt and isochronous 
 *        endpoints, the maximum number of bytes the endpoint expects to 
 *        transfer per service interval.
 */
struct PACKED USB_EndPointCompanionSuperSpeedDescriptor {
    uint8_t       bLength;          // Descriptor size in bytes (6)
    uint8_t       bDescriptorType;  // SUPERSPEED_USB_ENDPOINT_COMPANION (30h)
    uint8_t       bMaxBurst;        // Maximum number of packets - 1.
    uint8_t       bmAttributes;     // Maximum number of streams or packets 
    uint16_t      wBytesPerInterval;// Maximum number of bytes the endpoint expects.
};


/**
 * @brief String Descriptor for Endpoint #0
 * 
 * @note  wLANGID[] contains one or more 16 bit languages ID.
 *        Commonly it is 0x0409 (US English). 
 * 
 * @note  The  *flexible array member* feature of C99 is used in the
 *        last elements of the struct. This enables the definition during
 *        the initialization. At run time, a large amount of data can be
 *        allocated to store the struct data.
 * 
 * @note  bLength is sizeof(USB_StringDescriptor)+sizeof(wLANGID).
 *
 */
struct PACKED USB_StringDescriptorEP0 {
    uint8_t     bLength;            // Descriptor size in bytes (variable)
    uint8_t     bDescriptorType;    // The constant String (03h)
    uint16_t    wLangID[];          // Only for descriptor #0
};


/**
 * @brief String Descriptor for Endpoints greater than 0
 * 
 * @note  wLANGID[] contains one or more 16 bit languages ID.
 *        Commonly it is 0x0409 (US English). 
 * 
 * @note  The  *flexible array member* feature of C99 is used in the
 *        last elements of the struct. This enables the definition during
 *        the initialization. At run time, a large amount of data can be
 *        allocated to store the struct data.
 * 
 * @note  bLength is sizeof(USB_StringDescriptor)+sizeof(wLANGID).
 *
 */
struct PACKED USB_StringDescriptorEPN {
    uint8_t     bLength;            // Descriptor size in bytes (variable)
    uint8_t     bDescriptorType;    // The constant String (03h)
    uint16_t    wString[];          // Valid for descriptors #1 or higher
};

/**
 * @brief Device Capability Descriptor
 * 
 * @note  bData is a *flexible array member* feature of C99. This descriptor
 *        has size (3+sizeof(data))
 */
struct PACKED USB_DeviceCapabilityDescriptor {
    uint8_t     bLength;            // Descriptor length in bytes (varies).
    uint8_t     bDescriptorType;    // DEVICE CAPABILITY (10h)
    uint8_t     bDevCapabilityType; // See symbols below
    uint8_t     bData[];            // data with variable size
};


/**
 * @brief Device Object Store (BOS) Descriptor
 * 
 * @note  wTotalLength is the number of bytes in this descriptor and all of
 *        its subordinate descriptors
 * 
 * @note  bNumDeviceCaps is the number of device capability descriptors 
 *        subordinated to this BOS descriptor.
 */
struct PACKED USB_DeviceObjectStoreDescriptor {
    uint8_t     bLength;            // Descriptor size in bytes (5).
    uint8_t     bDescriptorType;    // BOS (0Fh)
    uint16_t    wTotalLength;       // Size of this descriptor and all of subordinates
    uint8_t     bNumDeviceCaps;     // Number of device capability descriptors.
};


/**
 * @brief Typedef for Descriptors
 * 
 * @note  TODO Maybe define a pointer type too.
 */

typedef struct USB_GeneralDescriptor USB_GeneralDescriptor;;
typedef struct USB_DeviceDescriptor USB_DeviceDescriptor;
typedef struct USB_DeviceQualifierDescriptor USB_DeviceQualifierDescriptor;
typedef struct USB_ConfigurationDescriptor USB_ConfigurationDescriptor;
typedef struct USB_OtherSpeedDescriptor USB_OtherSpeedDescriptor;
typedef struct USB_InterfaceAssociationDescriptor USB_InterfaceAssociationDescriptor;
typedef struct USB_InterfaceDescriptor USB_InterfaceDescriptor;
typedef struct USB_EndpointDescriptor USB_EndpointDescriptor;
typedef struct USB_EndPointCompanionSuperSpeedDescriptor USB_EndPointCompanionSuperSpeedDescriptor;
typedef struct USB_StringDescriptorEP0 USB_StringDescriptorEP0;
typedef struct USB_StringDescriptorEPN USB_StringDescriptorEPN;
typedef struct USB_DeviceCapabilityDescriptor USB_DeviceCapabilityDescriptor; 
typedef struct USB_DeviceObjectStoreDescriptor USB_DeviceObjectStoreDescriptor;




/**
 * @brief Definitions of symbols for descriptor types and sizes
 *
 *   | Descriptor |   Size   | Description                      |
 *   |------------|----------| ---------------------------------|
 *   | 01h        |    18    | device                           |
 *   | 02h        |     9    | configuration                    |
 *   | 03h        |    10    | string                           |
 *   | 04h        |     9    | interface                        |
 *   | 05h        |     7    | endpoint                         |
 *   | 06h        |    10    | device_qualifier                 |
 *   | 07h        |     9    | other_speed_configuration        |
 *   | 08h        |     X    | interface_power                  |
 *   | 09h        |     X    | OTG                              |
 *   | 0Ah        |     X    | debug                            |
 *   | 0Bh        |     8    | interface_association            |
 *   | 0Ch        |     X    | security                         |
 *   | 0Dh        |     X    | key                              |
 *   | 0Eh        |     X    | encryption type                  |
 *   | 0Fh        |     5    | binary device object store (BOS) |
 *   | 10h        |     X    | device capability                |
 *   | 11h        |     X    | wireless_endpoint_companion      |
 *   | 30h        |     6    | SuperSpeed_endpoint_companion    |
 * 
 * @note A simple name without the descriptor specifier is used
 *       because the fields are used in all descriptors
 */



///@{
/** Sizes of descriptors */
#define USB_DESCSIZE_DEVICE                             (18)
#define USB_DESCSIZE_CONFIGURATION                      (9)
#define USB_DESCSIZE_STRING                             (10)
#define USB_DESCSIZE_INTERFACE                          (9)
#define USB_DESCSIZE_ENDPOINT                           (7)
#define USB_DESCSIZE_DEVICEQUALIFIER                    (10)
#define USB_DESCSIZE_OTHERSPEED                         (9)
#define USB_DESCSIZE_POWER                              (X)
#define USB_DESCSIZE_OTG                                (X)
#define USB_DESCSIZE_DEBUG                              (X)
#define USB_DESCSIZE_INTERFACEASSOCIATION               (8)
#define USB_DESCSIZE_SECURITY                           (X)
#define USB_DESCSIZE_KEY                                (X)
#define USB_DESCSIZE_ENCRYPTION                         (X)
#define USB_DESCSIZE_BOS                                (5)
#define USB_DESCSIZE_DEVICECAPABILITY                   (X)
#define USB_DESCSIZE_WIRELESSENDPOINT                   (X)
#define USB_DESCSIZE_ENDPOINTSUPERSPEED                 (6)
///@}

///@{
/* Descriptor codes */
#define USB_DESCTYPE_DEVICE                             0x01
#define USB_DESCTYPE_CONFIGURATION                      0x02
#define USB_DESCTYPE_STRING                             0x03
#define USB_DESCTYPE_INTERFACE                          0x04
#define USB_DESCTYPE_ENDPOINT                           0x05
#define USB_DESCTYPE_DEVICEQUALIFIER                    0x06
#define USB_DESCTYPE_OTHERSPEED                         0x07
#define USB_DESCTYPE_POWER                              0x08
#define USB_DESCTYPE_OTG                                0x09
#define USB_DESCTYPE_DEBUG                              0x0A
#define USB_DESCTYPE_INTERFACEASSOCIATION               0x0B
#define USB_DESCTYPE_SECURITY                           0x0C
#define USB_DESCTYPE_KEY                                0x0D
#define USB_DESCTYPE_ENCRYPTION                         0x0E
#define USB_DESCTYPE_BOS                                0x0F
#define USB_DESCTYPE_DEVICECAPABILITY                   0x10
#define USB_DESCTYPE_WIRELESSENDPOINT                   0x11
#define USB_DESCTYPE_SUPERSPEEDENDPOINT                 0x30
///@}

/**
 * @brief USB version definitions
 * 
 * @note  They are 2 bytes long. 
 * 
 * @note  It is used in many descriptors
 * 
 * @note  For Little Endian Systems. For Big Endian, the bytes
 *        must be reversed. E.g. 0x0210 -> 0x1002
 */
///@{'
#define USB_DEVDESC_VERSION_11                                  0x0110
#define USB_DEVDESC_VERSION_20                                  0x0200
#define USB_DEVDESC_VERSION_21                                  0x0210
#define USB_DEVDESC_VERSION_25                                  0x0250
#define USB_DEVDESC_VERSION_30                                  0x0300
#define USB_DEVDESC_VERSION_40                                  0x0400
///@}

/**
 * @brief Definitions of symbols for Device Classes
 * 
 * @note  They are used in the device and interface descriptor. 
 *        Some of them are specific to one of them.
 *
 * @note Device Class Code from https://www.usb.org/defined-class-codes
 *
 * @note  Table of Device Classes used in Device Descriptors
 * 
 * | bDeviceClass | Description                                                |
 * |--------------|------------------------------------------------------------|
 * | 00h          | The interface descriptor specifies the class and the       |
 * |  ^           | function doesn’t use an interface association descriptor.  |
 * |  ^           | (See EFh below.)                                           |
 * | 02h          | Communications device (can instead be declared at the      |
 * |  ^           | interface level)                                           |
 * | 09h          | Hub                                                        |
 * | 0Fh          | Personal healthcare device (declaring at the interface     |
 * |  ^           | level preferred)                                           |
 * | DCh          | Diagnostic device (can instead be declared at the interface|
 * |  ^           | level)                                                     |
 * | E0h          | Wireless Controller (can instead be declared at the        | 
 * |  ^           | interface level)                                           |
 * | EFh          | Miscellaneous
 * |    ^         | bDeviceSubclass = 01h
 * |    ^         |   bDeviceProtocol = 01h: active sync
 * |    ^         |   bDeviceProtocol = 02h: Palm sync
 * |    ^         | bDeviceSubclass = 02h
 * |    ^         |   bDeviceProtocol = 01h: interface association descriptor
 * |    ^         |   bDeviceProtocol = 01h: wire adapter multifunction peripheral (Wireless USB).
 * | FFh          | Vendor-specific (can instead be declared at the interface level) 
 *
 * 
 *  
 * @note  Table of Device Classes used in Interface Descriptors
 *
 * |Device Class| Description
 * |------------|-----------------------------------------------------
 * |     00     | Reserved
 * |     01     | Audio
 * |     02     | Communications device class: communication interface
 * |     03     | Human interface device
 * |     05     | Physical
 * |     06     | Image
 * |     07     | Printer
 * |     08     | Mass storage
 * |     09     | Hub
 * |     0A     | Communications device class: data interface
 * |     0B     | Smart Card
 * |     0D     | Content Security
 * |     0E     | Video
 * |     0F     | Personal healthcare device (can instead be declared at the device level)
 * |     DC     | Diagnostic device (can instead be declared at the device level)
 * |      ^     | bInterfaceSubclass= 01h
 * |      ^     | bInterfaceProtocol = 01h USB2 compliance device
 * |     E0     | Wireless controller
 * |      ^     | bInterfaceSubclass = 01h
 * |      ^     |    bInterfaceProtocol = 01h: Bluetooth programming interface (can also be declared at the device level)
 * |      ^     |    bInterfaceProtocol = 02h: UWB Radio control interface (Wireless USB)
 * |      ^     |    bInterfaceProtocol = 03h: remote NDIS
 * |      ^     | bInterfaceSubclass = 02h. Host and device wire adapters (Wireless USB)
 * |     EF     | Miscellaneous
 * |      ^     | bInterfaceSubclass = 01h
 * |      ^     |    bInterfaceProtocol = 01h: active sync
 * |      ^     |    bInterfaceProtocol = 02h: Palm sync
 * |      ^     | bInterfaceSubclass = 03h. Cable based association framework (Wireless USB)
 * 
 *
 */


///@{
/** Device Descriptor */
#define USB_DEVDESC_DEVCLASS_DEFERRED                       0x00
#define USB_DEVDESC_DEVCLASS_CDC                            0x02
#define USB_DEVDESC_DEVCLASS_HUB                            0x09
#define USB_DEVDESC_DEVCLASS_BILLBOARD                      0x11
#define USB_DEVDESC_DEVCLASS_DIAGNOSTIC                     0xDC
#define USB_DEVDESC_DEVCLASS_MISCELLANEOUS                  0xEF
#define USB_DEVDESC_DEVCLASS_VENDOR                         0xFF
///@} 

///@{
/** Interface Descriptors */
#define USB_INTDESC_DEVCLASS_AUDIO                          0x01
#define USB_INTDESC_DEVCLASS_CDC_COMM                       0x02
#define USB_INTDESC_DEVCLASS_HDI                            0x03
#define USB_INTDESC_DEVCLASS_PHYS                           0x05
#define USB_INTDESC_DEVCLASS_IMAGE                          0x06
#define USB_INTDESC_DEVCLASS_PRINTER                        0x07
#define USB_INTDESC_DEVCLASS_MSD                            0x08
#define USB_INTDESC_DEVCLASS_CDC_DATA                       0x0A
#define USB_INTDESC_DEVCLASS_SMARTCARD                      0x0B
#define USB_INTDESC_DEVCLASS_SECURITY                       0x0D
#define USB_INTDESC_DEVCLASS_VIDEO                          0x0E
#define USB_INTDESC_DEVCLASS_HEALTHCARE                     0x0F
#define USB_INTDESC_DEVCLASS_AUDIOVIDEO                     0x10
#define USB_INTDESC_DEVCLASS_BRIDGE                         0x12
#define USB_INTDESC_DEVCLASS_DIAGNOSTIC                     0xDC
#define USB_INTDESC_DEVCLASS_WIRELESS                       0xE0
#define USB_INTDESC_DEVCLASS_MISCELLANEOUS                  0xEF
#define USB_INTDESC_DEVCLASS_APPLICATION                    0xFE
#define USB_INTDESC_DEVCLASS_VENDOR                         0xFF
///@} 

/**
 * @brief Device Subclasses for Device Descriptors
 * 
 * @note  Generally defined in Class Specific Docs
 */
#define USB_DEVDESC_SUBCLASS_DEFERRED                       0x00
#define USB_DEVDESC_SUBCLASS_HUB_FS                         0x00
#define USB_DEVDESC_SUBCLASS_HUB_HS_TT                      0x01
#define USB_DEVDESC_SUBCLASS_HUB_HS_TTS                     0x02
#define USB_DEVDESC_SUBCLASS_BILLBOARD                      0x00
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_USB2                0x01
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_DEBUG               0x02
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_VERDORTRACE         0x03
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_VENDORDFXDBC        0x04
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_GP                  0x05
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_VENDORDFX           0x06
#define USB_DEVDESC_SUBCLASS_DIAGNOSTIC_VENDORDVC           0x07

/**
 * @brief Protocol for Device Descriptors
 * 
 * @note  Generally defined in Class Specific Docs
 */
///@{
#define USB_DEVDESC_PROTOCOL_DEFERRED                       0x00
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_USB2                0x01
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_DEBUG_VENDOR        0x00
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_DEBUG_GNU           0x01
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_VERDORTRACE         0x01
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_VENDORDFXDBC        0x01
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_GP_VENDOR           0x00
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_GP_GNU              0x01
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_VENDORDFX           0x06
#define USB_DEVDESC_PROTOCOL_DIAGNOSTIC_VENDORDVC           0x07
///@}

/**
 * @brief Subclasses for Interface Descriptors
 */
///@{
#define USB_INTDESC_SUBCLASS_IMAGE                          0x01
#define USB_INTDESC_SUBCLASS_SECURITY                       0x00
#define USB_INTDESC_SUBCLASS_AUDIOVIDEO_CONTROL             0x00
#define USB_INTDESC_SUBCLASS_AUDIOVIDEO_VIDEO               0x00
#define USB_INTDESC_SUBCLASS_AUDIOVIDEO_AUDIO               0x00
#define USB_INTDESC_SUBCLASS_BILLBOARD                      0x00
#define USB_INTDESC_SUBCLASS_BRIDGE                         0x00
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_USB2                0x01
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_DEBUG               0x02
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_VENDORTRACEDBC      0x03
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_VENDORDFXDBC        0x04
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_DVC                 0x05
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_VENDORDFX           0x06
#define USB_INTDESC_SUBCLASS_DIAGNOSTIC_VENDORTRACEDVC      0x07

#define USB_INTDESC_SUBCLASS_WIRELESS_BTUWBNDIS             0x01
#define USB_INTDESC_SUBCLASS_WIRELESS_HOSTDEVICEWIRE        0x02
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_SYNC             0x01
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_DEFERRED         0x02
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_CABLE            0x03
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_RNDIS            0x04
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_VISION           0x05
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_STEP             0x06
#define USB_INTDESC_SUBCLASS_MISCELLANEOUS_CMD              0x07
#define USB_INTDESC_SUBCLASS_APPLICATION_DFU                0x01
#define USB_INTDESC_SUBCLASS_APPLICATION_IRDABRIDGE         0x02
#define USB_INTDESC_SUBCLASS_APPLICATION_TEST               0x03
///@} 

/**
 * @brief Protocol for Interface Descriptors
 */
///@{
#define USB_INTDESC_PROTOCOL_IMAGE                          0x01
#define USB_INTDESC_PROTOCOL_SECURITY                       0x00
#define USB_INTDESC_PROTOCOL_AUDIOVIDEO                     0x00
#define USB_INTDESC_PROTOCOL_BRIDGE                         0x00
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_USB2                0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_DEBUG_VENDOR        0x00
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_DEBUG_GNU           0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_VENDORTRACEDBC      0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_VENDORDFXDBC        0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_DVC_VENDOR          0x00
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_DVC_GNU             0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_VENDORDFX           0x01
#define USB_INTDESC_PROTOCOL_DIAGNOSTIC_VENDORTRACEDVC      0x01
#define USB_INTDESC_PROTOCOL_WIRELESS_BTUWBNDIS_BLPI        0x01
#define USB_INTDESC_PROTOCOL_WIRELESS_BTUWBNDIS_UWB         0x02
#define USB_INTDESC_PROTOCOL_WIRELESS_BTUWBNDIS_RNDIS       0x03
#define USB_INTDESC_PROTOCOL_WIRELESS_BTUWBNDIS_BLAMP       0x04
#define USB_INTDESC_PROTOCOL_WIRELESS_HOSTDEVICEWIRE_HOST   0x01
#define USB_INTDESC_PROTOCOL_WIRELESS_HOSTDEVICEWIRE_DEV    0x02
#define USB_INTDESC_PROTOCOL_WIRELESS_HOSTDEVICEWIRE_ISO    0x03
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_SYNC_ACTIVE      0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_SYNC_PALM        0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_IAD              0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_WAMPPI           0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_CABLE            0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_ETH        0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_WIFI       0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_WIMAX      0x03
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_WWAN       0x04
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_IPV4       0x05
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_IPV6       0x06
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_RNDIS_GPRS       0x07
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_VISION_CONTROL   0x00
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_VISION_EVENT     0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_VISION_STREAM    0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_STEP             0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_STEP_RAW         0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_CMD_IAD          0x01
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_CMD              0x02
#define USB_INTDESC_PROTOCOL_MISCELLANEOUS_MEDIA            0x03
#define USB_INTDESC_PROTOCOL_APPLICATION_DFU                0x01
#define USB_INTDESC_PROTOCOL_APPLICATION_IRDABRIDGE         0x00
#define USB_INTDESC_PROTOCOL_APPLICATION_TEST               0x00
#define USB_INTDESC_PROTOCOL_APPLICATION_TEST_USB488        0x01
///@} 

/**
 * @brief Definitions of attributes in Configuration Descriptor
 */
///@{
#define USB_CONFDESC_ATTRIBUTES_SELFPOWERED                (1<<6)
#define USB_CONFDESC_ATTRIBUTES_REMOTEWAKEUP               (1<<5)
///@}


/**
 * @brief Definitions of attributes in Endpoint Descriptor
 */
///@{
#define USB_ENDPDESC_EP0OUT                                 0x00
#define USB_ENDPDESC_EP0IN                                  0x80
#define USB_ENDPDESC_EP1OUT                                 0x01
#define USB_ENDPDESC_EP1IN                                  0x81
#define USB_ENDPDESC_EP2OUT                                 0x02
#define USB_ENDPDESC_EP2IN                                  0x82
#define USB_ENDPDESC_EP3OUT                                 0x03
#define USB_ENDPDESC_EP3IN                                  0x83
#define USB_ENDPDESC_EP4OUT                                 0x04
#define USB_ENDPDESC_EP4IN                                  0x84
#define USB_ENDPDESC_EP5OUT                                 0x05
#define USB_ENDPDESC_EP5IN                                  0x85
#define USB_ENDPDESC_EP6OUT                                 0x06
#define USB_ENDPDESC_EP6IN                                  0x86
#define USB_ENDPDESC_ATTRIBUTES_CONTROL                     0x00
#define USB_ENDPDESC_ATTRIBUTES_ISOCHRONOUS                 0x01
#define USB_ENDPDESC_ATTRIBUTES_BULK                        0x02
#define USB_ENDPDESC_ATTRIBUTES_INTERRUPT                   0x03
#define USB_ENDPDESC_ATTRIBUTES_NOSYNC                      (0<<2)
#define USB_ENDPDESC_ATTRIBUTES_ASYNC                       (1<<2)
#define USB_ENDPDESC_ATTRIBUTES_ADAPTIVE                    (2<<2)
#define USB_ENDPDESC_ATTRIBUTES_SYNC                        (3<<2)
#define USB_ENDPDESC_ATTRIBUTES_DATA                        (0<<4)
#define USB_ENDPDESC_ATTRIBUTES_FEEDBACK                    (1<<4)
#define USB_ENDPDESC_ATTRIBUTES_IMPLICITFEEDBACK            (2<<4)
///@}

/**
 * @brief Definitions of symbols for device capability types
 * 
 * @note  where does this come from?
 * 
 * @note  See symbols for bDevCapabilityType below
 *               01h = Wireless USB
 *               02h = USB 2.0 EXTENSION
 *               03h = SUPERSPEED_USB
 *               04h = CONTAINER ID
 */
///@{
 #define USB_DEVCAP_WIRELESS                        0x01
 #define USB_DEVCAP_USB20_EXTENSION                 0x02
 #define USB_DEVCAP_SUPERSPEED_USB                  0x03
 #define USB_DEVCAP_CONTAINER_ID                    0x04
 #define USB_DEVCAP_RESERVED0                       0x00
 //@}


/*
 * @note  To avoid error messagens when using pedantic
 *        #pragma GCC diagnostic push
 *        #pragma GCC diagnostic ignored "-Wpedantic"
 *        // When using initialization of Flexible Member Array. Not Standard C.
 *        #pragma GCC diagnostic pop
*/

/**
 * @brief Macros to generate a string descriptor
 * 
 * @param NAME name of the descriptor
 * @param SIZE size of the string
 * @param ...  string as an array of characters 'A', 'B' ...
 * 
 * @note  It defines an uint16_t array and points to it.
 * 
 * @note  It uses C99 macros with variadic parameters
 * 
  */
///@{
#define DECLARESTRINGDESC(NAME,SIZE,STRING,...) \
    const uint16_t ARRAY##NAME[] = { (SIZE*2+2)<<8|0x03, __VA_ARGS__ };     \
    const struct USB_StringDescriptorEPN *const NAME = (const struct USB_StringDescriptorEPN *const) ARRAY##NAME

#define DECLARELANGUAGEDESC(NAME,SIZE,...) \
    const uint16_t ARRAY##NAME[] = { (SIZE*2+2)<<8|0x03, __VA_ARGS__ };     \
    const struct USB_StringDescriptorEP0 *const NAME = (const struct USB_StringDescriptorEP0 *const) ARRAY##NAME
///@}



#endif // USB_DESCRIPTORS_H