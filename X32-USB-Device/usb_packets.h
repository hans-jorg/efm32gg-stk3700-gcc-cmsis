#ifndef USB_PACKETS_H
#define USB_PACKETS_H
/**
 * @file usb_packets.c
 * 
 * @brief USB packets specially control packets (only LS and HS)
 * 
 * @note  According USB Complete of Jan Axelson
 * 
 * @note Packet Identifier
 *  
 *       | PID Type | PID Name | PID | PID Byte | Packets                      |
 *       |----------|----------|-----|----------|------------------------------|
 *       | Token    | OUT      | 0001|  0x1E    | PID ADDR CRC                 |
 *       |    ^     | IN       | 1001|  0x96    | PID ADDR CRC
 *       |    ^     | SOF      | 0101|  0x5A    | PID FN CRC
 *       |    ^     | SETUP    | 1101|  0xD2    | PID ADDR CRC
 *       | Data     | DATA0    | 0011|  0x3C    | PID DATA CRC
 *       |    ^     | DATA1    | 1011|  0xB4    | PID DATA CRC
 *       |    ^     | DATA2    | 0111|  0x78    | PID DATA CRC
 *       |    ^     | MDATA    | 1111|  0xF0    | PID DATA CRC
 *       | Handshake| ACK      | 0010|  0x2D    | PID
 *       |    ^     | NAK      | 1010|  0xA5    | PID
 *       |    ^     | STALL    | 1110|  0xE1    | PID
 *       |    ^     | NYET     | 0110|  0x69    | PID
 *       | Special  | PRE      | 1100|  0xC3    | 
 *       |    ^     | ERR      | 1100|  0xC3    | 
 *       |    ^     | SPLIT    | 1000|  0x87    | 
 *       |    ^     | PING     | 0100|  0x4B    | 
 *       |    ^     | Reserved | 0000|  0x0F    | 
 * 
 * @note  The structs must be packed. I.e., with internal padding.
 *
 * @note  Maximum Data Packet Size in Transactions
 *        excluding PID and CRC bits
 * 
 *     | Transaction | Bus speed | Maximum Data Packet Size |     Rate      |
 *     |-------------|-----------|--------------------------|---------------|
 *     |   Control   |    Low    |            8             |        -      |
 *     |     ^       |    Full   |   8, 16, 32, or 64       |        -      |
 *     |     ^       |    High   |           64             |        -      |
 *     |     ^       |SuperSpeed |          512             |        -      |
 *     |    Bulk     |    Low    |           -              |        -      |
 *     |     ^       |    Full   |   8, 16, 32, or 64       |        -      |
 *     |     ^       |    High   |          512             |        -      |
 *     |     ^       |SuperSpeed |         1024             |        -      |
 *     | Interrupt   |    Low    |          1-8             |   1/10 frames |
 *     |     ^       |    Full   |         1-64             |   1/frame     |
 *     |     ^       |    High   |       1-1024             |   1/microframe|
 *     |     ^       |     ^     |     513-1024             |   2/microframe|
 *     |     ^       |     ^     |     683-1024             |   3/microframe|
 *     |     ^       |SuperSpeed |       1-1024             | 1/bus interval|
 *     |     ^       |     ^     |   1024-                  | 3/bus interval|
 *     | Isochronous |    Low    |           -              |        -      |
 *     |     ^       |    Full   |       0-1023             |   1/frame     |
 *     |     ^       |    High   |       0-1024             |   1/microframe|
 *     |     ^       |     ^     |     513-1024             |   2/microframe|
 *     |     ^       |     ^     |     683-1024             |   3/microframe|
 *     |     ^       |SuperSpeed |       1-1024             | 1/bus interval|
 *     |     ^       |     ^     |   1024-                  | 3/bus interval|
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

#ifdef __GNUC__
#define PACKED   __attribute__((__packed__))
#define ALIGNED  __attribute__((aligned(4)))
#else
#define PACKED
#define ALIGNED
#endif

/**
 * @brief USB Control Token Packet
 * 
 * @note  Sent from host
 * 
 * @note  Format
 * 
 * |Offset | Field        | Description                                                            |
 * |-------|--------------|------------------------------------------------------------------------|
 * |   0   | bmRequestType| 
 * |       |              |  Bit 7: Request direction (0=Host to device - Out, 1=Device to host - In).
 * |       |              |  Bits 5-6: Request type (0=standard, 1=class, 2=vendor, 3=reserved).
 * |       |              |  Bits 0-4: Recipient (0=device, 1=interface, 2=endpoint,3=other).
 * |   1   | bRequest     | The actual request (see the Standard Device Request Codes)
 * |   2   | wValue       | A word-size value that varies according to the request. For example, in the CLEAR_FEATURE request the value is used to select the feature, in the GET_DESCRIPTOR request the value indicates the descriptor type and in the SET_ADDRESS request the value contains the device address.
 * |   4   | wIndex       | A word-size value that varies according to the request. The index is generally used to specify an endpoint or an interface.
 * |   6   | wLength      | A word-size value that indicates the number of bytes to be transferred if there is a data stage.
 * 
 * @note Standard Device Request Codes
 * 
 * | bRequest         |   Value    |   wValue    |   wIndex   | wLength  |
 * |------------------|------------|-------------|------------|----------|
 * | GET_STATUS       |      0     |
 * | CLEAR_FEATURE    |      1     |
 * | Reserved         |      2     |
 * | SET_FEATURE      |      3     |
 * | Reserved         |      4     |
 * | SET_ADDRESS      |      5     |
 * | GET_DESCRIPTOR   |      6     |  0x0100     |  0x0000    |  0x0012  |
 * | SET_DESCRIPTOR   |      7     |
 * | GET_CONFIGURATION|      8     |
 * | SET_CONFIGURATION|      9     |
 * | GET_INTERFACE    |     10     |
 * | SET_INTERFACE    |     11     |
 * | SYNCH_FRAME      |     12     |
 * 
 * @note Packet formats for Device Requests
 * 
 * | Packet type      | PID | Type |  Req | wValue   | wIndex  | wLength  | Data      |
 * |------------------|-----|------|------|----------|---------|----------|-----------|
 * | GET_STATUS       |     | 0x80 | 0x00 | 0x0000   | 0x0000  | 0x0002   | Status    |
 * | CLEAR_FEATURE    |     | 0x00 | 0x01 | Feature  | 0x0000  | 0x0000   | None      |
 * | SET_FEATURE      |     | 0x00 | 0x03 | Feature  | 0x0000  | 0x0000   | None      |
 * | SET_ADDRESS      |     | 0x00 | 0x05 | Device   | 0x0000  | 0x0000   | None      |
 * | GET_DESCRIPTOR   |     | 0x80 | 0x06 |DescType  | 0x0000  |DescLength|Descriptor |
 * | SET_DESCRIPTOR   |     | 0x00 | 0x07 |DescType  | 0x0000  |DescLength|Descriptor |
 * | GET_CONFIGURATION|     | 0x80 | 0x08 | 0x0000   | 0x0000  | 0x0001   |ConfValue  |
 * | SET_CONFIGURATION|     | 0x00 | 0x09 |ConfValue | 0x0000  | 0x0000   | None      |
 * 
 * OBS:  1 - Set and Get Descriptor can have wIndex as a Language ID
 *       2 - DescType includes an index info (??)
 * 
 * @note Packet formats for interface requests
 * 
 * | Packet type      | Type |  Req | wValue   | wIndex  | wLength  | Data        |
 * |------------------|------|------|----------|---------|----------|-------------|
 * | GET_STATUS       | 0x81 | 0x00 | 0x0000   |Interface| 0x0002   | Status      |
 * | CLEAR_FEATURE    | 0x01 | 0x01 | Feature  |Interface| 0x0000   | None        |
 * | SET_FEATURE      | 0x01 | 0x03 | Feature  |Interface| 0x0000   | None        |
 * | GET_INTERFACE    | 0x81 | 0x0A | 0x0000   |Interface| 0x0001   | AltInterface|
 * | SET_INTERFACE    | 0x01 | 0x0B | AltValue |Interface| 0x0000   | None        |
 * 
 * @note Packet formats for endpoint requests
 * 
 * | Packet type      | Type |  Req | wValue   | wIndex  | wLength  | Data        |
 * |------------------|------|------|----------|---------|----------|-------------|
 * | GET_STATUS       | 0x82 | 0x00 | 0x0000   |Endpoint | 0x0002   | Status      |
 * | CLEAR_FEATURE    | 0x02 | 0x01 | Feature  |Endpoint | 0x0000   | None        |
 * | SET_FEATURE      | 0x02 | 0x03 | Feature  |Endpoint | 0x0000   | None        |
 * | SYNC_FRAME       | 0x82 | 0x12 | 0x0000   |Endpoint | 0x0002   | FrameNumber |
 * 
 * 
 * @
 */

struct USB_ControlPacket_Setup {
   uint8_t     bmRequestType; 
   uint8_t     bRequest;
   uint16_t    wValue;
   uint16_t    wIndex;
   uint16_t    wLength;
};

struct USB_ControlPacket_Data {
   uint8_t     bmRequestType; 
   uint8_t     bRequest;
   uint16_t    wValue;
   uint16_t    wIndex;
   uint16_t    wLength;
};

struct USB_ControlPacket_Handshake {
   uint8_t     bmRequestType; 
   uint8_t     bRequest;
   uint16_t    wValue;
   uint16_t    wIndex;
   uint16_t    wLength;
};


#endif // USB_PACKETS_H
