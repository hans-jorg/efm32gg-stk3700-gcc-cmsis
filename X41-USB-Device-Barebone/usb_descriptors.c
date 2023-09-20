/**
 * @file usb_descriptors.c
 * @author Hans
 * @brief 
 * @version 0.1
 * @date 2021-12-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include <stdint.h>
#include "usb_descriptors.h"


/**
 * @brief Initialization of Flexible Member Array
 * 
 * @note  Flexible Member Array (FMA) is an array with undetermined size
 *        as the last member of a struct. It is used when the
 *        the struct is a header of a data structurre.
 * 
 * @note  Standard C does not allow initialization of a struct
 *        with a FMA, but GCC does. When using -pedantic, it will
 *        cause an error or a warning. To silent it, use the
 *        #pragma sequence below.
 */
 





/**
 * @brief Make String Descriptor from a C string
 *
 * @note  It generates a String Descriptor structure on 
 *        area and copies the string to it.
 * 
 * @note  String Descriptors use 16-bit unicode
 *
 * @note  TODO UTF-8 conversion to UTF-16LE
 */ 
struct USB_StringDescriptorEPN *MakeStringDescriptorFromString(uint8_t *s, int max, char *str) {
struct USB_StringDescriptorEPN *p = (struct USB_StringDescriptorEPN *) s;
int len = 0;

    if( max > 255 ) max = 255;
    
    len = 2;   // header uses 2 bytes

    while( (len<max)&&(*str) ) {
        p->wString[len] = *str++;
        len+=2;
    }
    p->bLength = len;
    p->bDescriptorType = 0x03;
    return p;
}


