
#include "usbh_core.h"
#include "usb_musb_reg.h"
#include "interrupt.h"
#include "cpm_drv.h"
#include "debug.h"

#define USB_DP_CAPABILITY_VALUE (0xF)
#define USB_DN_CAPABILITY_VALUE (0xF)

#if CONFIG_USBDEV_EP_NUM != 8
#error beken chips only support 8 endpoints
#endif

#if CONFIG_USBHOST_PIPE_NUM != 8
#error beken chips only support 8 pipes
#endif
extern void drv_cpm_usbphyinit(uint8_t src_type);
extern void drv_cpm_usbphydeinit(void);
// clang-format off
static struct musb_fifo_cfg musb_device_table[] = {
{ .ep_num =  0, .style = FIFO_TXRX, .maxpacket = 64, },
{ .ep_num =  1, .style = FIFO_TX,   .maxpacket = 1024, },
{ .ep_num =  1, .style = FIFO_RX,   .maxpacket = 1024, },
{ .ep_num =  2, .style = FIFO_TX,   .maxpacket = 512, },
{ .ep_num =  2, .style = FIFO_RX,   .maxpacket = 512, },
{ .ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, },
{ .ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, },
{ .ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, },
{ .ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, },
{ .ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, },
{ .ep_num =  5, .style = FIFO_RX,   .maxpacket = 512, },
{ .ep_num =  6, .style = FIFO_TXRX, .maxpacket = 512, },
{ .ep_num =  7, .style = FIFO_TXRX, .maxpacket = 512, }
};

static struct musb_fifo_cfg musb_host_table[] = {
{ .ep_num =  0, .style = FIFO_TXRX, .maxpacket = 64, },
{ .ep_num =  1, .style = FIFO_RX,   .maxpacket = 1024, },    

 { .ep_num =  1, .style = FIFO_TX,   .maxpacket = 1024, },
 { .ep_num =  2, .style = FIFO_TX,   .maxpacket = 1024, },
 { .ep_num =  2, .style = FIFO_RX,   .maxpacket = 1024, },

// { .ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, },
// { .ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, },

// { .ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, },
// { .ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, },

// { .ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, },
// { .ep_num =  5, .style = FIFO_RX,   .maxpacket = 512, },

// { .ep_num =  6, .style = FIFO_TX,   .maxpacket = 512, },
// { .ep_num =  6, .style = FIFO_RX,   .maxpacket = 512, },

// { .ep_num =  7, .style = FIFO_TX,   .maxpacket = 512, },
// { .ep_num =  7, .style = FIFO_RX,   .maxpacket = 512, },
};
// clang-format on

uint8_t usbd_get_musb_fifo_cfg(struct musb_fifo_cfg **cfg)
{
    *cfg = musb_device_table;
    return sizeof(musb_device_table) / sizeof(musb_device_table[0]);
}

uint8_t usbh_get_musb_fifo_cfg(struct musb_fifo_cfg **cfg)
{
    *cfg = musb_host_table;
    return sizeof(musb_host_table) / sizeof(musb_host_table[0]);
}

uint32_t usb_get_musb_ram_size(void)
{
    return 8192;
}

//extern void USBD_IRQHandler(uint8_t busid);

void USBD_IRQ(void)
{
//    USBD_IRQHandler(0);
}

void usb_dc_low_level_init(void)
{

}

void usb_dc_low_level_deinit(void)
{
	
}

void usbd_musb_delay_ms(uint8_t ms)
{
    /* implement later */
}

extern void USBH_IRQHandler(uint8_t busid);

static inline  void USBH_IRQ(void)
{

    return USBH_IRQHandler(0);
}

//NOTEHERE 01
extern void (*USB_IRQHandler_Callback)(void);
void usb_hc_low_level_init(struct usbh_bus *bus)
{
	USB_LOG_ERR("usb_hc_low_level_init\r\n");
			
	NVIC_DisableIRQ(USBC_Irqn);
	INTX_DISABLE();
	drv_cpm_usbphydeinit();
	// when bit 7 set 1 means OTG_ID pull up to usb host mode
    (*(volatile unsigned char *)(0x40000040)) = 0x80;
    delayms(10);
	

	drv_cpm_usbphyinit(0x03);
	
	USB_IRQHandler_Callback = USBH_IRQ;
	NVIC_Init(6, 0, USBC_Irqn, 3);
	INTX_ENABLE();
}

void usb_hc_low_level_deinit(struct usbh_bus *bus)
{
}
