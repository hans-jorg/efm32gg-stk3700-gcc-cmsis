
In cdc.c
USBD_Callbacks_TypeDef defined in em_usb.h
USBD_Init_TypeDef defined in em_usb.h



    USBD_Init(&usbInitStruct);
        USBD_Init_TypeDef usbInitStruct 
            deviceDescriptor    -> USBDESC_deviceDesc
            configDescriptor    -> USBDESC_configDesc
            stringDescriptors   -> USBDESC_strings
            numberOfStrings      3
            bufferingMultiplier -> array with endpoint buffer sizes 1 or 2
            callbacks           ->  callbacks (below)
            reserved



        USBD_Callbacks_TypeDef callbacks
            usbReset                -> 0
            usbStateChange          -> CDC_StateChangeEvent
            setupCmd                -> CDC_SetupCmd
            isSelfPowered           -> 0
            sofInt                  -> 0


USBDESC_deviceDesc in descriptors.c
    .bLength            = USB_DEVICE_DESCSIZE,
    .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_CDC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
    .idVendor           = 0x10C4,
    .idProduct          = 0x0003,
    .bcdDevice          = 0x0000,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 0,
    .bNumConfigurations = 1

USBDESC_configDesc in descriptors.c
    .bLength                = USB_CONFIG_DESCSIZE,
    .bDescriptorType        = USB_CONFIG_DESCRIPTOR,
    .wTotalLength           = CONFIG_DESCSIZE (16 bits),
    .bNumInterfaces         = 2,
    .bConfigurationValue    = 1,
    .iConfiguration         = 0,
    .bmAttributes           = Selfpowered|D7,
    .bMaxPower              = 100 mA,
    ----------------- Interface Descriptor #0 -----------------------------
            .bLength;                    =   USB_INTERFACE_DESCSIZE,
            .bDescriptorType;            =   USB_INTERFACE_DESCRIPTOR,
            .bInterfaceNumber;           =   0,
            .bAlternateSetting;          =   0,
            .bNumEndpoints;              =   1,
            .bInterfaceClass;            =   USB_CLASS_CDC,
            .bInterfaceSubClass;         =   USB_CLASS_CDC_ACM,
            .bInterfaceProtocol;         =   0,
            .iInterface;                 =   0,

            .bLength                     =   USB_ENDPOINT_DESCSIZE,
            .bDescriptorType             =   USB_ENDPOINT_DESCRIPTOR,
            .bEndpointAddress            =   CDC_EP_NOTIFY,  // 0x82 usbconfig.h
            .bmAttributes                =   USB_EPTYPE_INTR, // = 3
            .wMaxPacketSize              =   USB_FS_BULK_EP_MAXSIZE, // = 64
            .bInterval                   =   0,

            /*** Endpoint  ***/
            USB_ENDPOINT_DESCSIZE,  /* bLength               */
            USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
            CDC_EP_NOTIFY,          /* bEndpointAddress (IN) */
            USB_EPTYPE_INTR,        /* bmAttributes          */
            USB_FS_INTR_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
            0,                      /* wMaxPacketSize (MSB)  */
            0xFF,                   /* bInterval             */
    ----------------- Interface Descriptor #1 -----------------------------
            .bLength;                    =   USB_INTERFACE_DESCSIZE,
            .bDescriptorType;            =   USB_INTERFACE_DESCRIPTOR,
            .bInterfaceNumber;           =   1,
            .bAlternateSetting;          =   0,
            .bNumEndpoints;              =   2,
            .bInterfaceClass;            =   USB_CLASS_CDC,
            .bInterfaceSubClass;         =   0,
            .bInterfaceProtocol;         =   0,
            .iInterface;                 =   0,
            // Endpoint 1
            .bLength                     =   USB_ENDPOINT_DESCSIZE,
            .bDescriptorType             =   USB_ENDPOINT_DESCRIPTOR,
            .bEndpointAddress            =   CDC_EP_DATA_IN,  // 0x81 usbconfig.h
            .bmAttributes                =   USB_EPTYPE_BULK, // = 2
            .wMaxPacketSize              =   USB_FS_BULK_EP_MAXSIZE, // = 64
            .bInterval                   =   0,
            // Endpoint 2
            .bLength                     =   USB_ENDPOINT_DESCSIZE,
            .bDescriptorType             =   USB_ENDPOINT_DESCRIPTOR,
            .bEndpointAddress            =   CDC_EP_DATA_OUT,  // 0x01 usbconfig.h
            .bmAttributes                =   USB_EPTYPE_BULK, // = 2
            .wMaxPacketSize              =   USB_FS_BULK_EP_MAXSIZE, // = 64
            .bInterval                   =   0,

