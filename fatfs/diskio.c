/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdio_hal.h"
#include "ssi_drv.h"
#include "chipconfig.h"
#include "nandflash_test.h"
#include "usb_host_includes.h"
#include "debug.h"


/* Definitions of physical drive number for each drive */

#define DEV_SDC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_FLH		1	/* Example: Map NORFLASH card to physical drive 1 */
#define DEV_NAN		2	/* Example: Map NANDFLASH card to physical drive 1 */
#define DEV_USBH	3	/* Example: Map USBHost disk to physical drive 1 */
uint8_t nand_init = 0;
/* used by user's api file. */
extern sdio_sd_cardinfo_t SDCardInfo;
uint8_t SDIO_DATA_BUFFER[512] = {0};

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_OKAY;

    switch (pdrv) {
    case DEV_SDC :
        return stat;

    case DEV_FLH :
        return stat;

    case DEV_NAN :
        return stat;
    case DEV_USBH :
        return stat;
    }
    
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_OKAY;
    sdio_sd_error_t status;
    nand_status_t nanstatus;
    uint8_t res = 0;
    uint8_t reg = 0;
    USBC_Host_HandleTypeDef usbHostInitStructure;
    switch (pdrv) {
    case DEV_SDC :
        status = hal_sd_enumcard();
        if (status != SD_OK)
        {
            stat = STA_NOINIT;
            printf("SD/TF disk_initialize error.\r\n");
        }
        else
        {
            printf("SD/TF disk_initialize ok.\r\n");
        }

        return stat;

    case DEV_FLH :
        printf("Attention: Norflash set to be xip-read, so ignore this option...\r\n");

        return stat;

    case DEV_NAN :
        if(nand_init ==0)
        nanstatus = nand_flash_init(&g_nand_info);
        else
        return STA_OKAY;
        nand_init = 1;
        if (nanstatus != NAND_OK)
        {
            stat = STA_NOINIT;
            printf("NAND disk_initialize error.\r\n");
        }
        else
        {
            printf("NAND disk_initialize ok.\r\n");
        }
        return stat;
    case DEV_USBH :
    printf("USB Host Test Start:");
    /* 初始化 */
    usbHostInitStructure.osc = USBHOSTPHY_EXTER_OSC;
    usbHostInitStructure.version = USB_VERSION_20;
    usbHostInitStructure.CallBackIT = NULL;

    USBHost_Init(&usbHostInitStructure);
    printf("Init success");
    USB_IRQHandler_Callback = USBDev_ISR;
    __disable_irq();
    NVIC_Init(6, 0, USBC_Irqn, 3);
    __enable_irq();
    printf("Wait Device connect");
    res = waitDeviceConnect(0xFFFFFF);
        if (res != 0)
        {
            printf("Time Out");
        }
        else
        {
            printf("Device insert");
            delayms(500);

            // Enumerate a removal hard disk,then enable USB DEV.
            reg = USBHost_InitUsbDevInsert();
            if (reg == false) // can't enumerate device which didn't support mass-storage-only and bulk-only protocol
            {
                printf("This Device can not support mass-storage-only and bulk-only protocol");
                gUSBC_ComReg->EINDEX = INDEX_EP1;
            }
            else
            {	
                printf("Usbhost fatfs init...\r\n");
            }
            return stat;
        }
    }
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    DRESULT res = RES_OK;
    sdio_sd_error_t status;
    nand_status_t nanstatus;

    BYTE bufftemp[2048];
    UINT sectors_per_page = 4;
    BYTE count_remaining, count_page_read;
    BYTE sector_offset;
    DWORD current_page_addr; // 映射
    BYTE* current_buff_ptr; 


    switch (pdrv) {
    case DEV_SDC :
        INTX_DISABLE();
        if((uint32_t)buff % 4 != 0)
        {
            for (int i = 0; i < count; i++)
            {
                status = hal_sd_readblock(sector, (uint32_t *)SDIO_DATA_BUFFER, BLOCK_SIZE, 1);
                memcpy(buff, SDIO_DATA_BUFFER, BLOCK_SIZE);
                buff += 512;
            }
            
        }
        else
        {
            status = hal_sd_readblock(sector, (uint32_t *)buff, BLOCK_SIZE, count);
        }
        INTX_ENABLE();
        if (status != SD_OK)
        {
            res = RES_ERROR;
            printf("SD/TF disk_read error.\r\n");
        }
        else
        {
            // printf("disk_read ok.\r\n");
        }

        return res;

    case DEV_FLH :
        memcpy(buff, (void*)(FATFS_NORFLASH_ADDR + sector * 512), count * 512);

        return res;

    case DEV_NAN :
        count_page_read = count/sectors_per_page;
        count_remaining = count%sectors_per_page;
        // printf("count_page_read = %d, count_remaining = %d\r\n", count_page_read, count_remaining);
        
        //  读取完整的page
        if(count_page_read > 0)
        {
            for (UINT i = 0; i < count_page_read; i++) {
                // printf("Sector = %d\r\n", sector);
                current_page_addr =  sector/sectors_per_page + i; // 映射
                current_buff_ptr = buff + (i * sectors_per_page * FF_MIN_SS); 

                nanstatus = nand_flash_page_read(current_page_addr, current_buff_ptr, NULL);
                if (nanstatus != NAND_OK) {
                    if (nanstatus == NAND_ERROR_ECC_UNCORRECTABLE) {
                        printf("NAND Read: ECC uncorrectable for page %x\r\n", current_page_addr);
                        return RES_ERROR; // ECC无法纠正，读取失败
                    } else {
                        printf("NAND Read: Error %d for page %x\r\n", nanstatus, current_page_addr);
                        return RES_ERROR; // 其他NAND错误
                    }
                }else{
                    // printf("NAND Read: Page %lu read successfully\r\n", current_page_addr);
                }
            }
        }

        // 处理剩余的部分
        if (count_remaining > 0) {
            // printf("Sector = %d\r\n", sector);
            current_page_addr = sector/sectors_per_page + count_page_read; // 映射
            current_buff_ptr = buff + (count_page_read * 4 * FF_MIN_SS);

            nanstatus = nand_flash_page_read(current_page_addr, bufftemp, NULL);
            if (nanstatus != NAND_OK) {
                if (nanstatus == NAND_ERROR_ECC_UNCORRECTABLE) {
                    printf("NAND Read: ECC uncorrectable for page %x\r\n", current_page_addr);
                    return RES_ERROR; // ECC无法纠正，读取失败
                } else {
                    printf("NAND Read: Error %d for page %x\r\n", nanstatus, current_page_addr);
                    return RES_ERROR; // 其他NAND错误
                }
            }else{
                // printf("NAND Read: Page %lu read successfully\r\n", current_page_addr);
            }

            // 复制剩余部分
            sector_offset = sector % sectors_per_page;
            memcpy(current_buff_ptr, bufftemp + sector_offset * FF_MIN_SS, count_remaining * FF_MIN_SS);

        }
        return res;
    case DEV_USBH :
         USBHost_Read10(sector, count, (uint8_t *)buff);
    return res;
}

    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    DRESULT res = RES_OK;
    sdio_sd_error_t status;
    uint8_t nanstatus;

    switch (pdrv) {
    case DEV_SDC :
        status = hal_sd_writeblock(sector, (uint32_t *)buff, BLOCK_SIZE, count);
        if (status != SD_OK)
        {
            res = RES_ERROR;
            printf("SD/TF disk_write error.\r\n");
        }
        else
        {
            // printf("disk_write ok.\r\n");
        }

        return res;

    case DEV_FLH :
        printf("Attention: Norflash set to be xip-read, so ignore this option...");

        return res;

    case DEV_NAN :
        // TO DO: 默认读写成功, 根据是否需要添加写入功能
        printf("Attention: No Nandflash write opt...");
        // nand_flash_page_program 具体操作参考NAN READ
        return res;
    case DEV_USBH :
        USBHost_Write10(sector, count, (uint8_t *)buff);
        return res;
    }

    return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_OK;
    int result;
    UINT sectors_per_page = 4;

    switch (pdrv) {
    case DEV_SDC :
        switch (cmd) {
            //（SD卡容量 = sector_count * 512）
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = SDCardInfo.CardCapacity;
                break;
            case GET_SECTOR_SIZE:
                *(WORD*)buff = BLOCK_SIZE;
                break;
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = 1;
                break;
            case CTRL_SYNC:
                break;
            default:
                return RES_PARERR;
        }
        return res;

    case DEV_FLH :
        // printf("Attention: Norflash set to be xip-read, so ignore this setting...");
        switch (cmd) {
            //（FLASH容量 = sector_count * 512）
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = 49152; // TO DO: 根据实际分配的镜像大小匹配，这里设置为 12M/256=49152
                break;
            case GET_SECTOR_SIZE:
                *(WORD*)buff = 512; // Attention: 若兼容读写，可能需要设置为4K，这里配置为 page 大小
                break;
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = 1;
                break;
            case CTRL_SYNC:
                break;
            default:
                return RES_PARERR;
        }
        return res;

    case DEV_NAN :
        // printf("Attention: Norflash set to be xip-read, so ignore this setting...");
        switch (cmd) {
            //（FLASH容量 = sector_count * 512）
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = g_nand_info.total_blocks * g_nand_info.pages_per_block * 4; // TO DO: 自定义
                break;
            case GET_SECTOR_SIZE:
                *(WORD*)buff = g_nand_info.page_size_bytes / sectors_per_page;  //2K / 4
                break;
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = g_nand_info.pages_per_block;
                break;
            case CTRL_SYNC:
                break;
            default:
                return RES_PARERR;
        }
        return res;
    case DEV_USBH :
        switch (cmd)
        {
            case CTRL_SYNC:
            res = RES_OK;
            break;
            case GET_SECTOR_COUNT :
            *(DWORD*)buff = 1024;
            break;
            case GET_SECTOR_SIZE :
            *(DWORD*)buff = 512;
            break;
            case GET_BLOCK_SIZE :
            *(DWORD*)buff = 1;
            break;
            default:
            res = RES_PARERR;
        }
        return res;
    }

    return RES_PARERR;
}

