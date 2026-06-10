
#include "usb_drv.h"
#include "usb_protocol.h"
#include "usb_hid_protocol.h"

// standard Device Descriptor
const uint8_t usb_hid_Device_Descriptor[18] =
    {
        0x12,                    // bLength: 0x12
        DT_DEVICE,               // bDescriptorType:
        0x00, 0x02,              // bcdusb_hid: version2.0
        0x00,                    // bDeviceClass:
        0x00,                    // bDeviceSubClass:
        0x00,                    // bDeviceProtocol:
        USB_MAX_PACKET_SIZE_EP0, // bMaxPacketSize0: 64
        0x48, 0x54,              // idVendor:
        0x02, 0x00,              // idProduct:
        0x00, 0x03,              // bcdDevice:
        0x01,                    // iManufacturer:
        0x02,                    // iProduct:
        0x00,                    // iSerialNumber:
        0x01,                    // bNumConfigurations:
};
// Device Qualifier Descriptor
const uint8_t usb_hid_Device_Qualifier_Descriptor[10] =
    {

        // Device Qualifier(0x0A bytes)
        0x0A,                    // bLength: 0x12 bytes
        DT_DEVICE_QUALIFIER,     // bDescriptorType: DEVICE
        0x00, 0x02,              // bcdusb_hid: version 2.0
        0x00,                    // bDeviceClass: independent interfaces
        0x00,                    // bDeviceSubClass: 0
        0x00,                    // bDeviceProtocol: class specific protocols NOT used on device basis
        USB_MAX_PACKET_SIZE_EP0, // bMaxPacketSize0 for other speed
        0x01,                    // bNumConfigurations for other-speed configuration
        0x00,                    // Reserved,must be 0
};

// configuration descriptor
const uint8_t usb_hid_Configuration_Descriptor[] =
    {
        0x09,                           // bLength
        DT_CONFIGURATION,               // bDescriptor
        _byte_low(USB_HID_CONFIG_LEN),  // wTotalLength  该集合的总长度  低字节
        _byte_high(USB_HID_CONFIG_LEN), // wTotalLength  该集合的总长度  高字节
        0x01,                           // bNumInterface
        0x01,                           // bConfiguration Value
        0x00,                           // iConfiguration
        0xa0,                           // bmAttributes
        0x32,                           // bMaxPower   100mA
        // usb_hid
        0x09,         // bLength:
        DT_INTERFACE, // bDescriptorType
        0x00,         // bInterfaceNumber
        0x00,         // bAlternateSetting
        0x02,         // bNumEndpoints
        0x03,         // bInterfaceClass:
        0x00,         // bInterfaceSubClass
        0x00,         // bInterfaceProtocol
        0x00,         // iInterface
        // HID
        0x09, // bLength:
        0x21, // bDescriptorType:    HID
        0x10, // BCDHID:             HID1.1
        0x01,
        0x00, // bCountryCode:
        0x01, // bNumDescriptor:
        0x22, // bDescriptorType:
        0x36, // WDescriptorLength:
        0x00,
        // USB接口描述符端点描述符
        0x07,                                // bLength:
        DT_ENDPOINT,                         // bDescriptorType:
        0x80 + BULKIN_EP,                    // EndpointAddress
        TRANS_INT,                           // bmAttributes
        _byte_low(USB_MAX_PACKET_SIZE_V20),  // wMaxPacketSize字段。该端点的最大包长 低字节
        _byte_high(USB_MAX_PACKET_SIZE_V20), // wMaxPacketSize字段。该端点的最大包长 高字节
        0x01,                                // bInterval

        0x07,                                // bLength:
        DT_ENDPOINT,                         // bDescriptorType:
        BULKOUT_EP,                          // EndpointAddress
        TRANS_INT,                           // bmAttributes
        _byte_low(USB_MAX_PACKET_SIZE_V20),  // wMaxPacketSize字段。该端点的最大包长 低字节
        _byte_high(USB_MAX_PACKET_SIZE_V20), // wMaxPacketSize字段。该端点的最大包长 高字节
        0x01,                                // bInterval
};

const uint8_t usb_hid_ReportDescriptor_V20[] = // USB报告描述符
    {
        0x06, 0xA0, 0xFF, // 全局，将用途页选择为?
        0x09, 0x01,       // 局部，将用途选择为01
        0xA1, 0x01,       // 主条目，COLLECTION开集合，为应用集合
        0x09, 0x02,       // 局部，将用途选择为02
        0xA1, 0x00,       // 主条目，COLLECTION开集合，为?
        0x06, 0xA1, 0xFF, // 全局，将用途页选择为?

        0x09, 0x03,                                                                    // 局部，将用途选择为03
        0x09, 0x04,                                                                    // 局部，将用途选择为04
        0x15, 0x80,                                                                    // 全局，逻辑值最小为80
        0x25, 0x7F,                                                                    // 全局，逻辑值最大为7F
        0x35, 0x00,                                                                    // 全局，物理值最小为00
        0x45, 0xFF,                                                                    // 全局，物理值最大为FF
        0x75, 0x08,                                                                    // 全局，每个数据域长度为8位
        0x96, _byte_low(USB_MAX_PACKET_SIZE_V20), _byte_high(USB_MAX_PACKET_SIZE_V20), // 全局，数据域的数量为512个
        0x81, 0x02,                                                                    // 主条目，数据位输入

        0x09, 0x05, // 局部，将用途选择为05
        0x09, 0x06,
        0x15, 0x80,
        0x25, 0x7F,
        0x35, 0x00,
        0x45, 0xFF,
        0x75, 0x08,
        0x96, _byte_low(USB_MAX_PACKET_SIZE_V20), _byte_high(USB_MAX_PACKET_SIZE_V20), // 全局，数据域的数量为512个
        0x91, 0x02,                                                                    // 主条目，数据位输出

        0xC0, // END COLLECTION
        0xC0  // END COLLECTION
};

const uint8_t usb_hid_ReportDescriptor_V11[] = // USB报告描述符
    {
        0x06, 0xA0, 0xFF, // 全局，将用途页选择为?
        0x09, 0x01,       // 局部，将用途选择为01
        0xA1, 0x01,       // 主条目，COLLECTION开集合，为应用集合
        0x09, 0x02,       // 局部，将用途选择为02
        0xA1, 0x00,       // 主条目，COLLECTION开集合，为?
        0x06, 0xA1, 0xFF, // 全局，将用途页选择为?

        0x09, 0x03,                               // 局部，将用途选择为03
        0x09, 0x04,                               // 局部，将用途选择为04
        0x15, 0x80,                               // 全局，逻辑值最小为80
        0x25, 0x7F,                               // 全局，逻辑值最大为7F
        0x35, 0x00,                               // 全局，物理值最小为00
        0x45, 0xFF,                               // 全局，物理值最大为FF
        0x75, 0x08,                               // 全局，每个数据域长度为8位
        0x95, _byte_low(USB_MAX_PACKET_SIZE_V11), // 全局，数据域的数量为64个
        0x81, 0x02,                               // 主条目，数据位输入

        0x09, 0x05, // 局部，将用途选择为05
        0x09, 0x06,
        0x15, 0x80,
        0x25, 0x7F,
        0x35, 0x00,
        0x45, 0xFF,
        0x75, 0x08,
        0x95, _byte_low(USB_MAX_PACKET_SIZE_V11), // 全局，数据域的数量为64个
        0x91, 0x02,                               // 主条目，数据位输出

        0xC0, // END COLLECTION
        0xC0  // END COLLECTION
};

// USB Language ID
const uint8_t usb_hid_LanguageID[4] =
    {
        4,         // Num bytes of this descriptor
        DT_STRING, // String descriptor
        0x09,      // Language ID LSB
        0x04       // Language ID
};

// Manufacturer String
const uint8_t usb_hid_StrDescManufacturer[12] =
    {
        12,        // Num bytes of this descriptor
        DT_STRING, // String descriptor
        'C', 0,
        'C', 0,
        'o', 0,
        'r', 0,
        'e', 0};

const uint8_t usb_hid_StrDescProduct[30] =
    {
        30,        // Num bytes of this descriptor
        DT_STRING, // String descriptor
        'U', 0,
        'S', 0,
        'B', 0,
        ' ', 0,
        'H', 0,
        'I', 0,
        'D', 0,
        ' ', 0,
        'D', 0,
        'E', 0,
        'V', 0,
        'I', 0,
        'C', 0,
        'E', 0};

// Serial Number
const uint8_t usb_hid_StrDescSerialNumber[34] =
    {
        34,        // Num bytes of this descriptor
        DT_STRING, // String descriptor
        '1', 0,
        '2', 0,
        '3', 0,
        '4', 0,
        '5', 0,
        '6', 0,
        '7', 0,
        '8', 0,
        '9', 0,
        '0', 0,
        'A', 0,
        'B', 0,
        'C', 0,
        'D', 0,
        'E', 0,
        'F', 0};
