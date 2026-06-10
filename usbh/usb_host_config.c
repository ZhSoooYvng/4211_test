/**
 * @file usb_host_config.c
 * @author Product application department
 * @brief usb host config file
 * @version V1.0
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

// 头文件包含
#include "usb_host_setup.h"

// 全局变量定义
// standard Device Descriptor
uint8_t USB_Device_Descriptor[18];

// Device Qualifier Descriptor
uint8_t USB_Device_Qualifier_Descriptor[10];

// configuration descriptor
uint8_t USB_Configuration_Descriptor[64];

// standard string Descriptor,serial number
uint8_t USB_String_Descriptor[64];

/**
 * @brief String language ID
 */
const uint8_t USB_LANGID[4] =
    {
        0x04,
        0x03,
        0x09,
        0x04,
};

/**
 * @brief get device descriptor command
 */
const uint8_t USBReq_GetDescriptor[8] =
    {
        0x80, // bmRequest
        0x06, // bRequest
        0x00,
        0x01,
        0x00,
        0x00,
        0x12,
        0x00};

/**
 * @brief send set address comamnd to device
 */
const uint8_t USBReq_SetAddress[8] =
    {
        0x00,                      // bmRequest
        0x05,                      // bRequest
        USB_MASS_STORAGE_DEV_ADDR, // Address
        0x00,                      // wIndexLow
        0x00,                      // wIndexHigh
        0x00,                      // wLengthLow
        0x00,                      // wLengthHigh
        0x00                       // none data
};

/**
 * @brief set configuration command to device
 */
const uint8_t USBReq_SetConfiguration[8] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * @brief send set interface command to device
 */
const uint8_t USBReq_SetInterface[8] = {0x01, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * @brief clear feature request
 */
uint8_t USBReq_ClearFeature[8] = {0x2, 0x1, 0, 0, 0, 0, 0, 0};

/**
 * @brief get max LUN request
 */
const uint8_t USBReq_GetMaxLUN[8] = {0xa1, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};

/**
 * @brief capacity  data
 */
const uint8_t SCSIDATA_ReadCapacities[] =
    {

        0x00, // number of block   MSB
        0x00, // number of block    |
        0x01, // number of block    |
        0x80, // number of block   LSB // 192KBYTE

        0x00, // no cartridge in drive//02
        0x00, // length of block
        0x02,
        0x00};

/**
 * @brief capacity and format capacity data
 */
const uint8_t SCSIDATA_ReadFormatCapacities[] =
    {
        0x00, // reserved
        0x00, // reserved
        0x00, // reserved
        0x08, // Capacity List Length

        0x00, // number of block   MSB
        0x00, // number of block    |
        0x01, // number of block    |
        0x80, // number of block   LSB // 192KBYTE

        0x02, // formatted media
        0x00, // length of block
        0x02,
        0x00};

/**
 * @brief SCSI read capacity command
 */
const uint8_t SCSICmd_Packet[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x88, 0x44, 0x77, 0x80, // dCBWTag
        0x08, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0a,                   // bCBWCBLength 3:0
        0x25,                   // SCSI command code
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief CSI read capacity command23
 */
const uint8_t SCSICmd_ReadCapacity23[] =
    {
        0x55,
        0x53,
        0x42,
        0x43, // dCBWSignature
        0x08,
        0x20,
        0xd6,
        0x81, // dCBWTag
        0xfc,
        0x00,
        0x00,
        0x00, // dCBWDataTransferLength
        0x80, // bmCBWFlag
        0x00, // bCBWLUN 3:0
        0x0a, // bCBWCBLength 3:0
        0x23, // SCSI command code
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xfc,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
};

/**
 * @brief CSI read capacity command10
 */
const uint8_t SCSICmd_ReadCapacity10[] =
    {
        0x55,
        0x53,
        0x42,
        0x43, // dCBWSignature
        0x1e,
        0x00,
        0x00,
        0x00, // dCBWTag
        0x08,
        0x00,
        0x00,
        0x00, // dCBWDataTransferLength
        0x80, // bmCBWFlag
        0x00, // bCBWLUN 3:0
        0x0a, // bCBWCBLength 3:0
        0x25, // SCSI command code
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
};

/**
 * @brief CSI read capacity command25
 */
const uint8_t SCSICmd_ReadCapacity25[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x08, 0x20, 0xd6, 0x81, // dCBWTag
        0x08, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0a,                   // bCBWCBLength 3:0
        0x25,                   // SCSI command code
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief SCSI read format capacity command
 */
const uint8_t SCSICmd_ReadFormatCapacity23[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x08, 0x20, 0xd6, 0x81, // dCBWTag
        0xFC, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0a,                   // bCBWCBLength 3:0
        0x23,                   // SCSI command code
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFC, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief SCSICmd_RequestSense
 */
const uint8_t SCSICmd_RequestSense[] =
    {
        0x55,
        0x53,
        0x42,
        0x43, // dCBWSignature
        0x08,
        0x20,
        0xd6,
        0x81, // dCBWTag
        0x12,
        0x00,
        0x00,
        0x00, // dCBWDataTransferLength
        0x80, // bmCBWFlag
        0x00, // bCBWLUN 3:0
        0x0c, // bCBWCBLength 3:0
        0x03, // SCSI command code
        0x00,
        0x00,
        0x00,
        0x12,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
};

// below for HOST command reference

/**
 * @brief Control Medium Removal 1e command
 */
const uint8_t SCSICmd_MediumRemoval[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0xc8, 0x6b, 0x83, 0x87, // dCBWTag
        0xc0, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0c,                   // bCBWCBLength 3:0
        0x1e,                   // Operation Code
        0x00,                   // Lun Number
        0x00, 0x00,             // Reserved
        0x00,                   // Prevent or Allow  0->Allow Removal   1->Prevent Removal
        0x00, 0x00, 0x00, 0x00, // reserved
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief mode sense 1a command
 */
const uint8_t SCSICmd_ModeSense06[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0xc8, 0x6b, 0x83, 0x87, // dCBWTag
        0xc0, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x06,                   // bCBWCBLength 3:0
        0x1a,                   // SCSI command code
        0x00, 0x1c, 0x00, 0xc0,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

const uint8_t SCSICmd_Inquiry12[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x08, 0x20, 0xd6, 0x81, // dCBWTag
        0x68, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x06,                   // bCBWCBLength 3:0
        0x12,                   // SCSI command code
        0x00, 0x00, 0x55, 0x68,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief scsi test unit ready command
 */
const uint8_t SCSICmd_TestReady[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x09, 0x00, 0x00, 0x00, // dCBWTag
        0x00, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x00,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x06,                   // bCBWCBLength 3:0
        0x00,                   // SCSI command code
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

const uint8_t SCSICmd_Read_cap10[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x09, 0x00, 0x00, 0x00, // dCBWTag
        0x08, 0x00, 0x00, 0x00, // dCBWDataTransferLength
        0x80,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0a,                   // bCBWCBLength 3:0
        0x25,                   // SCSI command code
        0x00, 0x00, 0x0a, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00};

/**
 * @brief SCSICmd_Read28
 */
const uint8_t SCSICmd_Read28[] =
    {
        0x55,
        0x53,
        0x42,
        0x43, // dCBWSignature
        0x16,
        0x00,
        0x00,
        0x00, // dCBWTag   history:0xe8,0x03,0x0e,0x85,// 20161128 fix by zxb
        0x00,
        0x02,
        0x00,
        0x00, // dCBWDataTransferLength
        0x80, // bmCBWFlag
        0x00, // bCBWLUN 3:0
        0x0a, // bCBWCBLength 3:0
        0x28, // SCSI command code
        0x00, // LogicalUnitNumber 7:5
        0x00,
        0x00,
        0x00,
        0x00, // Logical Block Address
        0x00, // Reserved
        0x00,
        0x01, // Transfer Length
        0x00,
        0x00,
        0x00, // Reserved
        0x00,
        0x00,
        0x00,
        0x00,
};

/**
 * @brief scsicmd write command
 */
const uint8_t SCSICmd_Write2a[] =
    {
        0x55, 0x53, 0x42, 0x43, // dCBWSignature
        0x68, 0x9e, 0x00, 0x83, // dCBWTag
        0x00, 0x02, 0x00, 0x00, // dCBWDataTransferLength
        0x00,                   // bmCBWFlag
        0x00,                   // bCBWLUN 3:0
        0x0a,                   // bCBWCBLength 3:0
        0x2a,                   // SCSI command code
        0x00,                   // LogicalUnitNumber 7:5
        0x00, 0x00, 0x00, 0x00, // Logical Block Address
        0x00,                   // Reserved
        0x00, 0x00,             // Transfer Length
        0x00, 0x00, 0x00,       // Reserved
        0x00, 0x00, 0x00, 0x00};
