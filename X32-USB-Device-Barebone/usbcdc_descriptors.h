#ifndef USBCDC_DESCRIPTORS_H
#define USBCDC_DESCRIPTORS_H
/**
 * @file usbcdc-descriptors.h
 *
 * @brief descriptors for USB CDC Devices
 *
 *
 * @author Hans 
 * @brief 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdint.h>
#include "usb_packets.h"


/**
 * @brief CDC Device
 * 
 * @note  CDC devices use a set of classes
 * 
 * * Communication and CDC Device Class
 * * CDC-Control Interface
 * * CDC-Data Interface (optional, needed for bulk and isochronous transfers)
 * 
 * |Descriptor| Parameter      |Value | Description                                       |
 * |----------|----------------|------|--------------------------------------|
 * | Device   | bDeviceType    | 02h  | Communication and CDC Device Class   |
 * | Interface| bInterfaceClass| 02h  | CDC-Communication Interface Class    |
 * | Interface| bInterfaceClass| 0Ah  | CDC-Data Interface Class (optional)  |
 * | 
 * 
 * @note The CDC Communication Interface Class demands a management endpoint,
 *       (endpoint #0) and optionally, a otification endpoint (interrupt)
 * 
 * @note The CDC Data Interface Class demands a pair of endpoints, IN and OUT,
 *       bulk or isochronous.
 * 
 * @note  The main documentation is "Universal Serial Bus Class Definitions for
          Communications Devices Revision 1.2 (Errata 1)" of November 3, 2010.
          The "USB CDC Subclass Specification for PSTN Devices" is also used.

 */

/**
 * @note   *CDC Class Specific Codes*
 * 
 * @note   From Section 4 of "Universal Serial Bus Class Definitions for
 *         Communications Devices" revision 1.2 (Errata 1)
 * 
 * 
 * | Parameter         | Description                           | Code      |
 * |-------------------|---------------------------------------|-----------|
 * | bDeviceClass      | Device Class                          |  02h      |
 * | bInterfaceClass   | Control Interface Class               |  02h      |
 * | bInterfaceClass   | Data Interface Class                  |  0Ah      |
 * 
 * @note   Communication (Control) Interface
 * 
 * | Parameter         | Description                           |-----------|
 * |-------------------|---------------------------------------|-----------|
 * | bInterfaceSubClass| Class Subclass Codes                  | See below |
 * | bInterfaceProtocol| Interface Class Control Protocol Codes| See below | 
 * 
 * 
 * @note  Communication Class Subclass Codes
 * 
 * | Code  |  Subclass                          | Reference    |
 * |-------|------------------------------------|--------------|
 * |  00h  | RESERVED                           |              |
 * |  01h  | Direct Line Control Model          | USBPSTN 1.2  |
 * |  02h  | Abstract Control Model             | USBPSTN 1.2  |
 * |  03h  | Telephone Control Model            | USBPSTN 1.2  |
 * |  04h  | Multi-Channel Control Model        | USBISDN 1.2  |
 * |  05h  | CAPI Control Model                 | USBISDN 1.2  |
 * |  06h  | Ethernet Networking Control Model  | USBECM 1.2   |
 * |  07h  | ATM Networking Control Model       | USBATM 1.2   |
 * |  08h  | Wireless Handset Control Model     | USBWMC 1.1   |
 * |  09h  | Device Management                  | USBWMC 1.1   |
 * |  0Ah  | Mobile Direct Line Model           | USBWMC 1.1   |
 * |  0Bh  | OBEX                               | USBWMC 1.1   |
 * |  0Ch  | Ethernet Emulation Model           | USBEEM 1.0   |
 * |  0Dh  | Network Control Model              | USBNCM 1.0   |
 * |0Dh-7Fh| RESERVED (future use)              |              |
 * |80h-FEh| RESERVED (vendor specific)         |              |
 *
 * @note Communication Class Protocol Codes
 * 
 * | Code  | Description                                              |
 * |-------|----------------------------------------------------------|
 * |00h    | No class specific protocol required                      |
 * |01h    | AT Commands defined by V.250 etc.                        |
 * |02h    | AT Commands defined by PCCA-101                          |
 * |03h    | AT Commands defined by PCCA-101 & Annex O                |
 * |04h    | AT Commands defined by GSM 07.07                         |
 * |05h    | AT Commands defined by 3GPP 27.007                       |
 * |06h    | AT Commands defined by TIA for CDMA                      |
 * |07h    | Ethernet Emulation Model                                 |
 * |08h-FDh| RESERVED (future use)                                    |
 * |FEh    | External Protocol: See Command SetFunctional Descriptor  |
 * |FFh    | Vendor-specific                                          |
 */

/**
 * @brief Symbols for Communication Device and Interface Codes
 * 
 */

#define USBCDC_DEVCLASS                                         0x02

#define USBCDC_COMMINTCLASS                                     0x02
#define USBCDC_DATAINTCLASS                                     0x0A


/**
 * @brief CDC Communicationb Class Subclass code
 * 
 */
///@{
#define USBCDC_COMMDEVSUBCLASS_DLCM                             0x01
#define USBCDC_COMMDEVSUBCLASS_ACM                              0x02
#define USBCDC_COMMDEVSUBCLASS_TCM                              0x03
#define USBCDC_COMMDEVSUBCLASS_MCCM                             0x04
#define USBCDC_COMMDEVSUBCLASS_CAPICM                           0x05
#define USBCDC_COMMDEVSUBCLASS_ETHM                             0x06
#define USBCDC_COMMDEVSUBCLASS_ATMM                             0x07
#define USBCDC_COMMDEVSUBCLASS_WHCM                             0x08
#define USBCDC_COMMDEVSUBCLASS_DEVMAN                           0x09
#define USBCDC_COMMDEVSUBCLASS_MDLM                             0x0A
#define USBCDC_COMMDEVSUBCLASS_OBEX                             0x0B
#define USBCDC_COMMDEVSUBCLASS_ETHEM                            0x0C
#define USBCDC_COMMDEVSUBCLASS_NETCM                            0x0D
///@}


/**
 * @brief CDC Interface Class Control Protocol Codes
 */
///@{
#define USBCDC_COMMPROT_USB                                     0x00
#define USBCDC_COMMPROT_V250                                    0x01
#define USBCDC_COMMPROT_PCCA101                                 0x02
#define USBCDC_COMMPROT_PCCA101O                                0x03
#define USBCDC_COMMPROT_GSM                                     0x04
#define USBCDC_COMMPROT_3GPP                                    0x05
#define USBCDC_COMMPROT_CDMA                                    0x06
#define USBCDC_COMMPROT_EEM                                     0x07
#define USBCDC_COMMPROT_EXT                                     0xFE
#define USBCDC_COMMPROT_VENDOR                                  0xFF         
///@}

/*
 * 
 * @note   Data Interface Class SubClass Codes
 *
 */
#define USBCDC_DATAINTSUBCLASS                                  0x00

/**
 * @note Data Interface Class Protocol Codes
 * 
 * | Code  | Description                                      | Reference      |
 * |-------|--------------------------------------------------|----------------|
 * |00h    | No class specific protocol required              |                |
 * |01h    | Network Transfer Block                           | USBNCM 1.0     |
 * |02h–2Fh| RESERVED (future use)          *DELETED!!!!*     |                |
 * |30h    | Physical interface protocol for ISDN BRI         |   I.430        |
 * |31h    | HDLC                                             |ISO/IEC3309-1993|
 * |32h    | Transparent                                      |                |
 * |33h–4Fh| RESERVED (future use)                            |                |
 * |50h    | Management protocol for Q.921 data link protocol |    Q.921M      |
 * |51h    | Data link protocol for Q.931                     |    Q.921       |
 * |52h    | TEI-multiplexor for Q.921 data link protocol     |    Q921TM      |
 * |53h–8Fh| RESERVED (future use)                            |                |
 * |90h    | Data compression procedures                      |     V.42bis    |
 * |91h    | Euro-ISDN protocol control                       | Q931/Euro-ISDN |
 * |92h    | V.24 rate adaption to ISDN                       |     V.120      |
 * |93h    | CAPI Commands                                    |     CAPI2.0    | 
 * |94h-FCh| RESERVED (future use)                            |                |
 * |FDh    | Host based driver (See Note on CDC120 document)  |                |
 * |FEh    | Use Protocol UnitFunctional Descriptors          |      CDC       |
 * |FFh    | Vendor-specific                                  |                |
 * 
 */


/**
 * @brief Data Interface Class and SubClass Codes
 * 
 */
#define USBCDC_DATAINTSUBCLASS                                  0x00

/**
 * @brief Data Interface Class Protocol Code
 */
///@{
#define USBCDC_DATAINTPROT_NONE                                 0x00
#define USBCDC_DATAINTPROT_NTP                                  0x01
#define USBCDC_DATAINTPROT_ISDNBRI                              0x30
#define USBCDC_DATAINTPROT_HDLC                                 0x31
#define USBCDC_DATAINTPROT_TRANSPARENT                          0x32
#define USBCDC_DATAINTPROT_Q921MGMT                             0x50
#define USBCDC_DATAINTPROT_Q921DATA                             0x51
#define USBCDC_DATAINTPROT_W921NYX                              0x52
#define USBCDC_DATAINTPROT_COMPRESSION                          0x90
#define USBCDC_DATAINTPROT_ISDNEU                               0x91
#define USBCDC_DATAINTPROT_V24                                  0x92
#define USBCDC_DATAINTPROT_CAPI                                 0x93 
#define USBCDC_DATAINTPROT_HOST                                 0xFD
#define USBCDC_DATAINTPROT_PUDESC                               0xFE
#define USBCDC_DATAINTPROT_VENDOR                               0xFF
///@}


/**
 * @brief Class Specific (Functional) Descriptors
 * 
 * @note bDescriptorType used in 
 * 
 *      CS_INTERFACE=0x24
 *      CS_ENDPOINT=0x25
 */

#define USBCDC_INTERFACE                                        0x24
#define USBCDC_ENDPOINT                                         0x25

/**
 * 
 * @note  Subtypes in Communication Functional Descriptors
 * 
 * |Subtype|   Functional Descriptor                               |
 * |-------|-------------------------------------------------------|
 * |00h    | Header Functional Descripor                           |
 * |01h    | Call Management Functional Descriptor                 |
 * |02h    | Abstract Control Management Functional Descriptor.    |
 * |03h    | Direct Line Management Functional Descriptor.         |
 * |04h    | Telephone Ringer Functional Descriptor.               |
 * |05h    | Telephone Call and Line State Capabilities Descriptor |
 * |06h    | Union Functional Descriptor                           |
 * |07h    | Country Selection Functional Descriptor               |
 * |08h    | Telephone Operational Modes Functional Descriptor     |
 * |09h    | USB Terminal Functional Descriptor                    |
 * |0Ah    | Network Channel Terminal Descriptor                   |
 * |0Bh    | Protocol Unit Functional Descriptor                   |
 * |0Ch    | Extension Unit Functional Descriptor                  |
 * |0Dh    | Multi-Channel Management Functional Descriptor        |
 * |0Eh    | CAPI Control Management Functional Descriptor         |
 * |0Fh    | Ethernet Networking Functional Descriptor             |
 * |10h    | ATM Networking Functional Descriptor                  |
 * |11h    | Wireless Handset Control Model Functional Descriptor  |
 * |12h    | Mobile Direct Line Model Functional Descriptor        |
 * |13h    | MDLM Detail Functional Descriptor                     |
 * |14h    | Device Management Model Functional Descriptor         |
 * |15h    | OBEX Functional Descriptor                            |
 * |16h    | Command Set Functional Descriptor                     |
 * |17h    | Command Set Detail Functional Descriptor              |
 * |18h    | Telephone Control Model Functional Descriptor         |
 * |19h    | OBEX Service Identifier Functional Descriptor         |
 * |1Ah    | NCM Functional Descriptor                             |
 * |1Bh-7Fh| RESERVED (future use)                                 |
 * |80h-FEh| RESERVED (vendor specific)                            |
  * 
 * OBS: 00 = marks the beginning of the concatenated set of functional 
 *      descriptors for the interface.
 *
 @note  Subtypes in Data Functional Descriptors
 * 
 * |Subtype|   Functional Descriptor                               |
 * |-------|-------------------------------------------------------|
 * |00h    | Header Functional Descriptor                          |
 * |01h-7Fh| RESERVED (future use)                                 |
 * |80h-FEH| RESERVED (vendor specific)                            |
 * 
 *
 * @brief CDC Descriptors requirements
 * 
 * | Field               | Value   | Description                           |
 * |---------------------|---------|---------------------------------------|
 * | <td rowspan="3">Device Class </td>                                    |
 * | bDeviceClass        |  02h    | Communications Device Class           |
 * | bDeviceSubClass     |  00h    | unused at this time                   |
 * | bDeviceProtocol     |  00h    | unused at this time                   |
 * | <td rowspan="3">Communications Class Interface Descriptor </td>       |
 * | bInterfaceClass     |  02h    | ?same as Communications Device Class? |
 * | bInterfaceSubClass  |   *     | See Table Subclass Codes              |
 * | bInterfaceProtocol  |   *     | See Table Protocol Codes              |
 * | <td rowspan="3">Data Class Interface Descriptor </td>                 |
 * | bInterfaceClass     |  0Ah    | Data Interface Class                  |
 * | bInterfaceSubClass  |  00h    | Data Interface Subclass               |
 * | bInterfaceProtocol  |   *     | See Table Data Class Protocol         |
 * 
 */

/**
 * @brief Symbols of Descriptor Subtypes for Functional Descriptors
 */
///@{
/** 
 * @notee Size information. For varying size descriptors, the size
 *        information is the size of the fixed part.
 *  
*/
#define USBCDC_DESCSIZE_HEADER                                  (5)
#define USBCDC_DESCSIZE_CALL_MANAGEMENT                         (5)
#define USBCDC_DESCSIZE_ACM_MANAGEMENT                          (4)
#define USBCDC_DESCSIZE_DIRECT_LINE_MANAGEMENT                  (4)
#define USBCDC_DESCSIZE_TELEPHONE_RINGER                        (5)
#define USBCDC_DESCSIZE_CALL_LINE                               (7)
#define USBCDC_DESCSIZE_UNION                                   (4)
#define USBCDC_DESCSIZE_COUNTRY_SELECTION                       (4)
#define USBCDC_DESCSIZE_TELEPHONE_OPERATIONAL_MODES             (4)
#define USBCDC_DESCSIZE_USB_TERMINAL                            (0)
#define USBCDC_DESCSIZE_NETWORK_CHANNEL_TERMINAL                (0)
#define USBCDC_DESCSIZE_PROTOCOL_UNIT                           (0)
#define USBCDC_DESCSIZE_EXTENSION_UNIT                          (0)
#define USBCDC_DESCSIZE_MULTI_CHANNEL_MANAGEMENT                (0)
#define USBCDC_DESCSIZE_CAPI_CONTROL_ANAGEMENT                  (0)
#define USBCDC_DESCSIZE_ETHERNET_NETWORKING                     (0)
#define USBCDC_DESCSIZE_ATM_NETWORKING                          (0)
#define USBCDC_DESCSIZE_WIRELESS_HANDSET_CONTROL_MODEL          (0)
#define USBCDC_DESCSIZE_MOBILE_DIRECT_LINE_MODEL                (0)
#define USBCDC_DESCSIZE_MDLM_DETAIL                             (0)
#define USBCDC_DESCSIZE_DEVICE_MANAGEMENT_MODEL                 (0)
#define USBCDC_DESCSIZE_OBEX                                    (0)
#define USBCDC_DESCSIZE_COMMAND_SET                             (0)
#define USBCDC_DESCSIZE_COMMAND_SET_DETAIL                      (0)
#define USBCDC_DESCSIZE_TELEPHONE_CONTROL_MODEL                 (0)
#define USBCDC_DESCSIZE_OBEX_SERVICE_IDENTIFIER                 (0)
#define USBCDC_DESCSIZE_NCM                                     (0)
///@}

///@{
/** Subtype */
#define USBCDC_DESCSUBTYPE_HEADER                               0x00
#define USBCDC_DESCSUBTYPE_CALL_MANAGEMENT                      0x01
#define USBCDC_DESCSUBTYPE_ACM_MANAGEMENT                       0x02
#define USBCDC_DESCSUBTYPE_DIRECT_LINE_MANAGEMENT               0x03
#define USBCDC_DESCSUBTYPE_TELEPHONE_RINGER                     0x04
#define USBCDC_DESCSUBTYPE_CALL_LINE                            0x05
#define USBCDC_DESCSUBTYPE_UNION                                0X06
#define USBCDC_DESCSUBTYPE_COUNTRY_SELECTION                    0x07
#define USBCDC_DESCSUBTYPE_TELEPHONE_OPERATIONAL_MODES          0x08
#define USBCDC_DESCSUBTYPE_USB_TERMINAL                         0x09
#define USBCDC_DESCSUBTYPE_NETWORK_CHANNEL_TERMINAL             0x0A
#define USBCDC_DESCSUBTYPE_PROTOCOL_UNIT                        0x0B
#define USBCDC_DESCSUBTYPE_EXTENSION_UNIT                       0x0C
#define USBCDC_DESCSUBTYPE_MULTI_CHANNEL_MANAGEMENT             0x0D
#define USBCDC_DESCSUBTYPE_CAPI_CONTROL_ANAGEMENT               0x0E
#define USBCDC_DESCSUBTYPE_ETHERNET_NETWORKING                  0x0F
#define USBCDC_DESCSUBTYPE_ATM_NETWORKING                       0x10
#define USBCDC_DESCSUBTYPE_WIRELESS_HANDSET_CONTROL_MODEL       0x11
#define USBCDC_DESCSUBTYPE_MOBILE_DIRECT_LINE_MODEL             0x12
#define USBCDC_DESCSUBTYPE_MDLM_DETAIL                          0x13
#define USBCDC_DESCSUBTYPE_DEVICE_MANAGEMENT_MODEL              0x14
#define USBCDC_DESCSUBTYPE_OBEX                                 0X15
#define USBCDC_DESCSUBTYPE_COMMAND_SET                          0x16
#define USBCDC_DESCSUBTYPE_COMMAND_SET_DETAIL                   0x17
#define USBCDC_DESCSUBTYPE_TELEPHONE_CONTROL_MODEL              0x18
#define USBCDC_DESCSUBTYPE_OBEX_SERVICE_IDENTIFIER              0x19
#define USBCDC_DESCSUBTYPE_NCM                                  0x1A
///@}


/**
 * @brief CDC Version
 * 
 */
#define USBCDC_VERSION_11                                       0x0101
#define USBCDC_VERSION_12                                       0x0102

/**
 * @brief General format Functional Descriptors
 * 
 */
struct PACKED USBCDC_GeneralDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubtype;     // SeeFunctional Descriptor's Subtypes
    //....
};


/**
 * @brief USB CDC Header Functional Descriptor
 * 
 * @note The class-specific descriptor starts with this header. 
 *       It marks the beginning of the concatenated set of functional
 *       descriptors for the interface.
 * 
 */
struct PACKED USBCDC_HeaderDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (5).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_DESCSUBTYPE_HEADER=0x00
    uint16_t    bcdDCD;                 // CDC Spec = 0x120
};


/**
 * @brief Union Functional Descriptor
 * 
 * @note It describes the relationship between a group of interfaces that
 *       can be considered to form a functional unit.
 */
struct PACKED USBCDC_UnionDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4+N).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubtype;     // USBCDC_DESCSUBTYPE_UNION=0x06
    uint8_t     bControlInterface;      // Interface #
    uint8_t     bSubordinateInterface[];// N Interface numbers [0..N-1]
};

/**
 * @brief USB CDC Country Selection Functional Descriptor
 * 
 */
struct PACKED USBCDC_CountrySelectionDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4+2*N).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubtype;     // USBCDC_COUNTRY_SELECTION_DESCRIPTOR=0x07
    uint8_t     iCountryCodeRelDate;    // Index of string: DDMMYYYY
    uint16_t    wCountryCode[];         // N Country codes (ISO3166)
};


/**
 * @brief USB CDC Call Management Functional Descriptor
 * 
 * @note  bmCapabilities
 *        D1 = 0 : Device sends/receives call management information only over
 *                 the Communications Class interface.
 *        D1 = 1 : Device can send/receive call management information over a 
 *                 Data Class interface.
 *        D1 = 1 : Device does handle call management itself.
 */
struct PACKED USBCDC_CallManagementDescriptor { // From PSTN
    uint8_t     bFunctionLength;        // Descriptor size in bytes (5).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_CALL_MANAGEMENT_DESCRIPTOR=0x01
    uint8_t     bmCapabilities;         // Capabilities (bit mask)
    uint8_t     bDataInterface;         // Interface # of Data Class interface
};


/**
 * @brief USB CDC Abstract Control Management Functional Descriptor
 * 
 * @note  bmCapabilities
 *        D3 = 1 : Device supports the notification Network_Connection
 *        D2 = 1 : Device supports the request Send_Break
 *        D1 = 1 : Device supports the request combination of Set_Line_Coding, 
 *                 Set_Control_Line_State, Get_Line_Coding, and the notification
 *                 Serial_State
 *        D0 = 1   Device supports the request combination of Set_Comm_Feature,
 *                 Clear_Comm_Feature, and Get_Comm_Feature.
 */
struct PACKED USBCDC_ACManagementDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_AC_MANAGEMENT_DESCRIPTOR=0x02
    uint8_t     bmCapabilities;         // Capabilities (bit mask?)
};


/**
 * @brief Telephone Ringer Functional Descriptor
 * 
 * @note  bRingerVolSteps
 * 
 *      0: 256
 *      1: 1 discrete volume step
 *      2: 2 discrete volume steps
 *      3: 3 discrete volume steps
 * 
 * @note  A general formula for defining ranges, based on X=bRingerVolSteps 
 *        and values [1 to Y] defining the first volume range is:
 *          $$ Y = (256/X) - 1 $$
 *        where X<>0 and X<=128.  
 *        Second volume range is: 
 *          $$ [ (Y+1) to (Y+Y) ] $$
 * 
 * @note The maximum value in the last range must always be 255
 */
struct PACKED USBCDC_TelephoneRingerDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_TELEPHONE_RINGER_DESCRIPTOR=0x04
    uint8_t     bRingerVolSteps;        // # discrete steps in volume
    uint8_t     bNumRingerPatterns;     // # Ringer Patterns [1.255]

};


/**
 * @brief Telephone Operational Modes Functional Descriptor
 * 
 * @note  bmCapabilities
 *      D2 = 1 : Supports Computer Centric mode
 *      D1 = 1 : Supports Standalone mode
 *      D0 = 1 : Supports Simple mode
 * 
 */
struct PACKED USBCDC_TelephoneOperationalModesDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_TELEPHONE_OPERATIONAL_MODES_DESCRIPTOR=0x08
    uint8_t     bmCapabilities;         // See above
};

/**
 * @brief Telephone Call and Line State Reporting Capabilities Descriptor
 * 
 * @note bmCapabilities
 * 
 *      D5 = 1 : Does support line state change notification
 *      D4 = 1 : Can report DTMF digits input remotely over the telephone line
 *      D3 = 1 : Reports incoming distinctive ringing patterns
 *      D2 = 1 : Reports caller ID information
 *      D1 = 1 : Reports ringback, busy, and fast busy states
 *      D0 = 1 : Reports interrupted dialtone in addition to normal dialtone
 */
struct PACKED USBCDC_CallLineStateReportingDescriptor {
    uint8_t     bFunctionLength;        // Descriptor size in bytes (4).
    uint8_t     bDescriptorType;        // CS_INTERFACE=0x24
    uint8_t     bDescriptorSubType;     // USBCDC_CALL_LINE_DESCRIPTOR=0x05
    uint8_t     bmCapabilities;         // See above
};

/**
 * @brief Type definitions for Functional Descriptors
 * 
 */

///@{
typedef struct PACKED USBCDC_FunctionalDescriptor *pUSBCDC_FunctionalDescriptor;
typedef struct PACKED USBCDC_HeaderDescriptor USBCDC_HeaderDescriptor;
typedef struct PACKED USBCDC_UnionDescriptor USBCDC_UnionDescriptor;
typedef struct PACKED USBCDC_CountrySelectionDescriptor USBCDC_CountrySelectionDescriptor;
typedef struct PACKED USBCDC_CallManagementDescriptor USBCDC_CallManagementDescriptor;
typedef struct PACKED USBCDC_ACManagementDescriptor USBCDC_ACManagementDescriptor;
typedef struct PACKED USBCDC_TelephoneRingerDescriptor USBCDC_TelephoneRingerDescriptor;
typedef struct PACKED USBCDC_TelephoneOperationalModesDescriptor USBCDC_TelephoneOperationalModesDescriptor;
typedef struct PACKED USBCDC_CallLineStateReportingDescriptor USBCDC_CallLineStateReportingDescriptor;
///@}


#endif // USBCDC_DESCRIPTORS_H
