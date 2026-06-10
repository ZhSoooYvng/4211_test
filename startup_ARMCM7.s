;/**************************************************************************//**
; * @file     startup_ARMCM7.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM7 Device Series
; * @version  V1.08
; * @date     23. November 2012
; *
; * @note
; *
; ******************************************************************************/
;/* Copyright (c) 2011 - 2012 ARM LIMITED
;
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
;   ---------------------------------------------------------------------------*/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00006000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00006000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     0x20000000+0x0fffc  ;0x20000000+0x0fffc      ;__initial_sp        ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     OTP_IRQHandler            ;  0:  OTP
                DCD     PMU_IRQHandler            ;  1:  PMU
                DCD     TC1_IRQHandler             ;  2:  TC
                DCD     PIT1_IRQHandler           ;  3:  PIT1
                DCD     PIT2_IRQHandler           ;  4:  PIT2
                DCD     TC2_IRQHandler            ;  5:  ENCR
                DCD     TC3_IRQHandler            ;  6:  ENCR1
                DCD     DMA1_IRQHandler            ;  7:  DMA
                DCD     DMA2_IRQHandler           ;  8:  DMA2
                DCD     0                         ;  9 : Reserved
                DCD     TRNG_IRQHandler           ; 10:  TRNG
                DCD     SEC_FD_IRQHandler         ; 11:  SEC_FD
                DCD     0                         ; 12: Reserved
                DCD     0                         ; 13: Reserved
                DCD     CPM_IRQHandler            ; 14:  CPM
                DCD     PMU_RTC_IRQHandler        ; 15:  PMU RTC
                DCD     SD_IRQHandler             ; 16:  SD
                DCD     DMA2D_IRQHandler          ; 17:  DMA2D
                DCD     0                         ; 18: Reserved
                DCD     AESSM4_IRQHandler         ; 19:  AES&SM4
                DCD     QADC_IRQHandler           ; 20:  QADC
                DCD     DAC_IRQHandler            ; 21:  DAC
                DCD     0                         ; 22: Reserved
                DCD     UART5_IRQHandler          ; 23:  UART5
                DCD     USB_IRQHandler            ; 24:  USBC
                DCD     UART6_IRQHandler          ; 25:  UART6
                DCD     SPI1_IRQHandler           ; 26:  SPI1
                DCD     SPI2_IRQHandler           ; 27:  SPI2
                DCD     SPI3_IRQHandler           ; 28:  SPI3
                DCD     SPIM1_IRQHandler          ; 29:  SPIM1
                DCD     SPIM2_IRQHandler          ; 30:  SPIM2
                DCD     SPIM3_IRQHandler          ; 31:  SPIM3
                DCD     UART1_IRQHandler          ; 32:  UART1
                DCD     UART2_IRQHandler          ; 33:  UART2
                DCD     PSRAM_IRQHandler          ; 34:  PSRAM
                DCD     GMAC_IRQHandler           ; 35:  GMCA
                DCD     I2C1_IRQHandler           ; 36: I2C1
                DCD     0                         ; 37: Reserved
                DCD     0                         ; 38: Reserved
                DCD     0                         ; 39: Reserved
                DCD     0                         ; 40: Reserved                                
                DCD     EPORT0_0_IRQHandler          ; 41: EPORT0_0
                DCD     EPORT0_1_IRQHandler          ; 42: EPORT0_1
                DCD     EPORT0_2_IRQHandler       ; 43: EPORT0_2
                DCD     EPORT0_3_IRQHandler       ; 44: EPORT0_3
                DCD     EPORT0_4_IRQHandler       ; 45: EPORT0_4
                DCD     EPORT0_5_IRQHandler       ; 46: EPORT0_5
                DCD     EPORT0_6_IRQHandler       ; 47: EPORT0_6
                DCD     EPORT0_7_IRQHandler       ; 48: EPORT0_7
                DCD     EPORT1_0_IRQHandler          ; 49: EPORT1_0
                DCD     EPORT1_1_IRQHandler          ; 50: EPORT1_1
                DCD     EPORT1_2_IRQHandler       ; 51: EPORT1_2
                DCD     EPORT1_3_IRQHandler       ; 52: EPORT1_3
                DCD     EPORT1_4_IRQHandler       ; 53: EPORT1_4
                DCD     EPORT1_5_IRQHandler       ; 54: EPORT1_5
                DCD     EPORT1_6_IRQHandler       ; 55: EPORT1_6
                DCD     EPORT1_7_IRQHandler          ; 56: EPORT1_7
                DCD     SSISLV1_IRQHandler        ; 57: SSISLV1
                DCD     JPEG_ACCELERATOR_IRQHandler  ; 58: JPEG_ACCELERATOR                
                DCD     I2C2_IRQHandler           ; 59:  I2C2    
                DCD     AXIDMA_IRQHandler         ; 60: AXIDMA                                        
                DCD     UART3_IRQHandler          ; 61:  UART3                
                DCD     UART4_IRQHandler          ; 62:  UART4                
                DCD     LCDC_ROT_ERROR_IRQHandler ; 63:  LCDCROT_ERROR  Interrupt                
                DCD     CAN1_BUF00_03_IRQHandler  ; 64:  CAN1_BUF00_03
                DCD     CAN1_BUF04_07_IRQHandler  ; 65:  CAN1_BUF04_07
                DCD     CAN1_BUF08_11_IRQHandler  ; 66:  CAN1_BUF08_11
                DCD     CAN1_BUF12_15_IRQHandler  ; 67:  CAN1_BUF12_15
                DCD     CAN1_BUF16_31_IRQHandler  ; 68:  CAN1_BUF16_31
                DCD     CAN1_BUF32_63_IRQHandler  ; 69:  CAN1_BUF32_63                
                DCD     BOFF_TRWARN_IRQHandler    ; 70:  BOFF_TRWARN
                DCD     CAN1_ERR_IRQHandler       ; 71:  CAN1_ERR
                DCD     CAN2_BUF00_03_IRQHandler  ; 72:  CAN1_BUF00_03
                DCD     CAN2_BUF04_07_IRQHandler  ; 73:  CAN1_BUF04_07
                DCD     CAN2_BUF08_11_IRQHandler  ; 74:  CAN1_BUF08_11
                DCD     CAN2_BUF12_15_IRQHandler  ; 75:  CAN1_BUF12_15
                DCD     CAN2_BUF16_31_IRQHandler  ; 76:  CAN1_BUF16_31
                DCD     CAN2_BUF32_63_IRQHandler  ; 77:  CAN1_BUF32_63                
                DCD     BOFF2_TRWARN_IRQHandler   ; 78:  BOFF_TRWARN
                DCD     CAN2_ERR_IRQHandler       ; 79:  CAN1_ERR                
                DCD     I2S1_IRQHandler           ; 80:  I2S1
                DCD     I2S2_IRQHandler           ; 81:  I2S2
                DCD     LCDC_CRC_ERROR_IRQHandler ; 82:  LCDC_CRC_ERROR
                DCD     LCDC_REG_RELOAD_IRQHandler; 83:  LCDC_REG_RELOAD
                DCD     PWMT1_IRQHandler          ; 84:  PWMT1                
                DCD     PWMT2_IRQHandler          ; 85:  PWMT2
                DCD     PWMT3_IRQHandler          ; 86:  PWMT3
                DCD     LCDC_BUS_ERROR_IRQHandler ; 87:  LCDC_BUS_ERROR
                DCD     LCDC_FIFO_UNDERRUN_IRQHandler;88:LCDC_FIFO_UNDERRUN
                DCD     LCDC_FIFO_WARN_IRQHandler ;89:   LCDC_FIFO_WARN
                DCD     LCDC_IRQ_SCANLINE_IRQHandler; 90:LCDC_IRQ_SCANLINE
                DCD     IMGAGE_DECODER_IRQHandler; ; 91: image decoder
                DCD     EPORT2_0_IRQHandler          ; 92: EPORT2_0
                DCD     EPORT2_1_IRQHandler          ; 93: EPORT2_1                
                DCD     EPORT2_2_IRQHandler          ; 94: EPORT2_2
                DCD     EPORT2_3_IRQHandler          ; 95: EPORT2_3
                DCD     EPORT2_4_IRQHandler          ; 96: EPORT2_4
                DCD     EPORT2_5_IRQHandler          ; 97: EPORT2_5
                DCD     EPORT2_6_IRQHandler          ; 98: EPORT2_6
                DCD     EPORT2_7_IRQHandler          ; 99: EPORT2_7
                DCD     EPORT3_0_IRQHandler          ; 100: EPORT3_0
                DCD     EPORT3_1_IRQHandler          ; 101: EPORT3_1                
                DCD     EPORT3_2_IRQHandler          ; 102: EPORT3_2
                DCD     EPORT3_3_IRQHandler          ; 103: EPORT3_3
                DCD     EPORT3_4_IRQHandler          ; 104: EPORT3_4
                DCD     EPORT3_5_IRQHandler          ; 105: EPORT3_5
                DCD     EPORT3_6_IRQHandler          ; 106: EPORT3_6
                DCD     EPORT3_7_IRQHandler          ; 107: EPORT3_7                
                DCD     EPORT4_0_IRQHandler          ; 108: EPORT4_0
                DCD     EPORT4_1_IRQHandler          ; 109: EPORT4_1                
                DCD     EPORT4_2_IRQHandler          ; 110: EPORT4_2
                DCD     EPORT4_3_IRQHandler          ; 111: EPORT4_3
                DCD     EPORT4_4_IRQHandler          ; 112: EPORT4_4
                DCD     EPORT4_5_IRQHandler          ; 113: EPORT4_5
                DCD     EPORT4_6_IRQHandler          ; 114: EPORT4_6
                DCD     EPORT4_7_IRQHandler          ; 115: EPORT4_7
                DCD     EPORT5_0_IRQHandler          ; 116: EPORT5_0
                DCD     EPORT5_1_IRQHandler          ; 117: EPORT5_1                
                DCD     EPORT5_2_IRQHandler          ; 118: EPORT5_2
                DCD     EPORT5_3_IRQHandler          ; 119: EPORT5_3
                DCD     EPORT5_4_IRQHandler          ; 120: EPORT5_4
                DCD     EPORT5_5_IRQHandler          ; 121: EPORT5_5
                DCD     EPORT5_6_IRQHandler          ; 122: EPORT5_6
                DCD     EPORT5_7_IRQHandler          ; 123: EPORT5_7                
                DCD     EPORT6_0_IRQHandler          ; 124: EPORT6_0
                DCD     EPORT6_1_IRQHandler          ; 125: EPORT6_1                
                DCD     EPORT6_2_IRQHandler          ; 126: EPORT6_2
                DCD     EPORT6_3_IRQHandler          ; 127: EPORT6_3
                DCD     EPORT6_4_IRQHandler          ; 128: EPORT6_4
                DCD     EPORT6_5_IRQHandler          ; 129: EPORT6_5
                DCD     EPORT6_6_IRQHandler          ; 130: EPORT6_6
                DCD     EPORT6_7_IRQHandler          ; 131: EPORT6_7
                DCD     EPORT7_0_IRQHandler          ; 132: EPORT7_0
                DCD     EPORT7_1_IRQHandler          ; 133: EPORT7_1                
                DCD     EPORT7_2_IRQHandler          ; 134: EPORT7_2
                DCD     EPORT7_3_IRQHandler          ; 135: EPORT7_3
                DCD     EPORT7_4_IRQHandler          ; 136: EPORT7_4
                DCD     EPORT7_5_IRQHandler          ; 137: EPORT7_5
                DCD     EPORT7_6_IRQHandler          ; 138: EPORT7_6
                DCD     EPORT7_7_IRQHandler          ; 139: EPORT7_7
                DCD     EPORT8_0_IRQHandler          ; 140: EPORT8_0
                DCD     EPORT8_1_IRQHandler          ; 141: EPORT8_1                
                DCD     EPORT8_2_IRQHandler          ; 142: EPORT8_2
                DCD     EPORT8_3_IRQHandler          ; 143: EPORT8_3
                DCD     EPORT8_4_IRQHandler          ; 144: EPORT8_4
                DCD     EPORT8_5_IRQHandler          ; 145: EPORT8_5
                DCD     EPORT8_6_IRQHandler          ; 146: EPORT8_6
                DCD     EPORT8_7_IRQHandler          ; 147: EPORT8_7
                DCD     EPORT9_0_IRQHandler          ; 148: EPORT9_0
                DCD     EPORT9_1_IRQHandler          ; 149: EPORT9_1                
                DCD     EPORT9_2_IRQHandler          ; 150: EPORT9_2
                DCD     EPORT9_3_IRQHandler          ; 151: EPORT9_3
                DCD     EPORT9_4_IRQHandler          ; 152: EPORT9_4
                DCD     EPORT9_5_IRQHandler          ; 153: EPORT9_5
                DCD     EPORT9_6_IRQHandler          ; 154: EPORT9_6
                DCD     EPORT9_7_IRQHandler          ; 155: EPORT9_7

                
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler
                
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]

                IMPORT  LVD_EN
                LDR     R0, =LVD_EN
                BLX     R0
               
;set lvd level at 2.0V 
                MOV        r0,#0x40000000
                MOV        r1,#0x40004000
                STR        r0,[r1,#0x5c]
                LSLS    r0,r0,#1
                STR        r0,[r1,#0x5c]
                ASRS    r0,r0,#1
                STR        r0,[r1,#0x5c]
                MOV        r0,r1
                LDR        r0,[r0,#0x5c]
                ORR        r0,r0,#0x400000
                STR        r0,[r1,#0x5c]
                
                LDR        r0,=0x01001314
                MOV        r1,#0x40004000
                STR        r0,[r1,#0x54]
                
                MOV        r1,#0x40004000
                LDR        r0,[r1,#0x5c]
                BIC        r0,r0,#0xc0000000
                STR        r0,[r1,#0x5c]
;set lvd level at 2.0V end

                IMPORT  IO_Latch_Clr
                LDR     R0, =IO_Latch_Clr
                BLX     R0
                IMPORT  pre_copy_code_init
                IMPORT  SystemInit
                IMPORT  __main    
                MOV     r0,#0xe
                LDR     R1,=0x40005000
                STRH    r0,[r1,#0]       ;Close WDT
                MOV     r0,#0
                LDR     r1,=0x40006000
                STRH    r0,[r1,#0]       ;Close TC    1
                
                MOV     r0,#0
                LDR     r1,=0x4000a000
                STRH    r0,[r1,#0]       ;Close TC    2
                
                MOV     r0,#0
                LDR     r1,=0x4000b000
                STRH    r0,[r1,#0]       ;Close TC    3
               
                LDR     R0, =pre_copy_code_init
                BLX     R0
                LDR     R0, =SystemInit
                BLX     R0
                ISB
                ISB
                LDR     R0, =__initial_sp  
                MSR     MSP, R0             ; 
                ISB
                ISB
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  OTP_IRQHandler                 [WEAK]
                EXPORT  PMU_IRQHandler                 [WEAK]
                EXPORT  TC1_IRQHandler                  [WEAK]
                EXPORT  PIT1_IRQHandler                [WEAK]
                EXPORT  PIT2_IRQHandler                [WEAK]
                EXPORT  TC2_IRQHandler                 [WEAK]
                EXPORT  TC3_IRQHandler                 [WEAK]
                EXPORT  DMA1_IRQHandler                 [WEAK]
                EXPORT  DMA2_IRQHandler                [WEAK]
                EXPORT  TRNG_IRQHandler                [WEAK]
                EXPORT  SEC_FD_IRQHandler              [WEAK]
                EXPORT  CPM_IRQHandler                 [WEAK]
                EXPORT  PMU_RTC_IRQHandler             [WEAK]
                EXPORT  SD_IRQHandler                  [WEAK]
                EXPORT  DMA2D_IRQHandler               [WEAK]
                EXPORT  AESSM4_IRQHandler              [WEAK]
                EXPORT  QADC_IRQHandler                [WEAK]
                EXPORT  DAC_IRQHandler                 [WEAK]
                EXPORT  UART5_IRQHandler               [WEAK]                    
                EXPORT  USB_IRQHandler                 [WEAK]                    
                EXPORT  UART6_IRQHandler               [WEAK]
                EXPORT  SPI1_IRQHandler                [WEAK]                    
                EXPORT  SPI2_IRQHandler                [WEAK]                    
                EXPORT  SPI3_IRQHandler                [WEAK]                    
                EXPORT  SPIM1_IRQHandler               [WEAK]                    
                EXPORT  SPIM2_IRQHandler               [WEAK]
                EXPORT  SPIM3_IRQHandler               [WEAK]
                EXPORT  UART1_IRQHandler               [WEAK]
                EXPORT  UART2_IRQHandler               [WEAK]
                EXPORT  PSRAM_IRQHandler               [WEAK]                    
                EXPORT  GMAC_IRQHandler                [WEAK]
                EXPORT  I2C1_IRQHandler                [WEAK]                    
                EXPORT  EPORT0_0_IRQHandler            [WEAK]
                EXPORT  EPORT0_1_IRQHandler            [WEAK]
                EXPORT  EPORT0_2_IRQHandler            [WEAK]
                EXPORT  EPORT0_3_IRQHandler            [WEAK]
                EXPORT  EPORT0_4_IRQHandler            [WEAK]
                EXPORT  EPORT0_5_IRQHandler            [WEAK]
                EXPORT  EPORT0_6_IRQHandler            [WEAK]
                EXPORT  EPORT0_7_IRQHandler            [WEAK]
                EXPORT  EPORT1_0_IRQHandler            [WEAK]
                EXPORT  EPORT1_1_IRQHandler            [WEAK]
                EXPORT  EPORT1_2_IRQHandler            [WEAK]
                EXPORT  EPORT1_3_IRQHandler            [WEAK]
                EXPORT  EPORT1_4_IRQHandler            [WEAK]
                EXPORT  EPORT1_5_IRQHandler            [WEAK]
                EXPORT  EPORT1_6_IRQHandler            [WEAK]
                EXPORT  EPORT1_7_IRQHandler            [WEAK]
                EXPORT  SSISLV1_IRQHandler             [WEAK]
                EXPORT  JPEG_ACCELERATOR_IRQHandler    [WEAK]
                EXPORT  I2C2_IRQHandler                [WEAK]    
                EXPORT  AXIDMA_IRQHandler              [WEAK]
                EXPORT  UART3_IRQHandler               [WEAK]                                                      
                EXPORT  UART4_IRQHandler               [WEAK]                                                      
                EXPORT  LCDC_ROT_ERROR_IRQHandler     [WEAK]                       
                EXPORT  CAN1_BUF00_03_IRQHandler       [WEAK]  
                EXPORT  CAN1_BUF04_07_IRQHandler       [WEAK]  
                EXPORT  CAN1_BUF08_11_IRQHandler       [WEAK]  
                EXPORT  CAN1_BUF12_15_IRQHandler       [WEAK]  
                EXPORT  CAN1_BUF16_31_IRQHandler       [WEAK]  
                EXPORT  CAN1_BUF32_63_IRQHandler       [WEAK]              
                EXPORT  BOFF_TRWARN_IRQHandler         [WEAK]    
                EXPORT  CAN1_ERR_IRQHandler            [WEAK]
                EXPORT  CAN2_BUF00_03_IRQHandler       [WEAK]  
                EXPORT  CAN2_BUF04_07_IRQHandler       [WEAK]  
                EXPORT  CAN2_BUF08_11_IRQHandler       [WEAK]  
                EXPORT  CAN2_BUF12_15_IRQHandler       [WEAK]  
                EXPORT  CAN2_BUF16_31_IRQHandler       [WEAK]  
                EXPORT  CAN2_BUF32_63_IRQHandler       [WEAK]              
                EXPORT  BOFF2_TRWARN_IRQHandler        [WEAK]   
                EXPORT  CAN2_ERR_IRQHandler            [WEAK]                       
                EXPORT  I2S1_IRQHandler                [WEAK]           
                EXPORT  I2S2_IRQHandler                [WEAK]          
                EXPORT  LCDC_CRC_ERROR_IRQHandler      [WEAK]         
                EXPORT  LCDC_REG_RELOAD_IRQHandler     [WEAK]        
                EXPORT  PWMT1_IRQHandler               [WEAK]                     
                EXPORT  PWMT2_IRQHandler               [WEAK]          
                EXPORT  PWMT3_IRQHandler               [WEAK]          
                EXPORT  LCDC_BUS_ERROR_IRQHandler      [WEAK]          
                EXPORT  LCDC_FIFO_UNDERRUN_IRQHandler  [WEAK]            
                EXPORT  LCDC_FIFO_WARN_IRQHandler      [WEAK]            
                EXPORT  LCDC_IRQ_SCANLINE_IRQHandler   [WEAK] 
        EXPORT    IMGAGE_DECODER_IRQHandler       [WEAK]
                EXPORT  EPORT2_0_IRQHandler            [WEAK]           
                EXPORT  EPORT2_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT2_2_IRQHandler            [WEAK]          
                EXPORT  EPORT2_3_IRQHandler            [WEAK]          
                EXPORT  EPORT2_4_IRQHandler            [WEAK]          
                EXPORT  EPORT2_5_IRQHandler            [WEAK]          
                EXPORT  EPORT2_6_IRQHandler            [WEAK]          
                EXPORT  EPORT2_7_IRQHandler            [WEAK]          
                EXPORT  EPORT3_0_IRQHandler            [WEAK]          
                EXPORT  EPORT3_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT3_2_IRQHandler            [WEAK]          
                EXPORT  EPORT3_3_IRQHandler            [WEAK]          
                EXPORT  EPORT3_4_IRQHandler            [WEAK]          
                EXPORT  EPORT3_5_IRQHandler            [WEAK]          
                EXPORT  EPORT3_6_IRQHandler            [WEAK]          
                EXPORT  EPORT3_7_IRQHandler            [WEAK]                          
                EXPORT  EPORT4_0_IRQHandler            [WEAK]          
                EXPORT  EPORT4_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT4_2_IRQHandler            [WEAK]          
                EXPORT  EPORT4_3_IRQHandler            [WEAK]          
                EXPORT  EPORT4_4_IRQHandler            [WEAK]          
                EXPORT  EPORT4_5_IRQHandler            [WEAK]          
                EXPORT  EPORT4_6_IRQHandler            [WEAK]          
                EXPORT  EPORT4_7_IRQHandler            [WEAK]          
                EXPORT  EPORT5_0_IRQHandler            [WEAK]          
                EXPORT  EPORT5_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT5_2_IRQHandler            [WEAK]          
                EXPORT  EPORT5_3_IRQHandler            [WEAK]          
                EXPORT  EPORT5_4_IRQHandler            [WEAK]          
                EXPORT  EPORT5_5_IRQHandler            [WEAK]          
                EXPORT  EPORT5_6_IRQHandler            [WEAK]          
                EXPORT  EPORT5_7_IRQHandler            [WEAK]                          
                EXPORT  EPORT6_0_IRQHandler            [WEAK]          
                EXPORT  EPORT6_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT6_2_IRQHandler            [WEAK]          
                EXPORT  EPORT6_3_IRQHandler            [WEAK]          
                EXPORT  EPORT6_4_IRQHandler            [WEAK]          
                EXPORT  EPORT6_5_IRQHandler            [WEAK]          
                EXPORT  EPORT6_6_IRQHandler            [WEAK]          
                EXPORT  EPORT6_7_IRQHandler            [WEAK]          
                EXPORT  EPORT7_0_IRQHandler            [WEAK]          
                EXPORT  EPORT7_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT7_2_IRQHandler            [WEAK]          
                EXPORT  EPORT7_3_IRQHandler            [WEAK]          
                EXPORT  EPORT7_4_IRQHandler            [WEAK]          
                EXPORT  EPORT7_5_IRQHandler            [WEAK]          
                EXPORT  EPORT7_6_IRQHandler            [WEAK]          
                EXPORT  EPORT7_7_IRQHandler            [WEAK]      
                EXPORT  EPORT8_0_IRQHandler            [WEAK]          
                EXPORT  EPORT8_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT8_2_IRQHandler            [WEAK]          
                EXPORT  EPORT8_3_IRQHandler            [WEAK]          
                EXPORT  EPORT8_4_IRQHandler            [WEAK]          
                EXPORT  EPORT8_5_IRQHandler            [WEAK]          
                EXPORT  EPORT8_6_IRQHandler            [WEAK]          
                EXPORT  EPORT8_7_IRQHandler            [WEAK]          
                EXPORT  EPORT9_0_IRQHandler            [WEAK]          
                EXPORT  EPORT9_1_IRQHandler            [WEAK]                          
                EXPORT  EPORT9_2_IRQHandler            [WEAK]          
                EXPORT  EPORT9_3_IRQHandler            [WEAK]          
                EXPORT  EPORT9_4_IRQHandler            [WEAK]          
                EXPORT  EPORT9_5_IRQHandler            [WEAK]          
                EXPORT  EPORT9_6_IRQHandler            [WEAK]          
                EXPORT  EPORT9_7_IRQHandler            [WEAK]          
                       

OTP_IRQHandler                
PMU_IRQHandler                
TC1_IRQHandler                 
PIT1_IRQHandler               
PIT2_IRQHandler               
TC2_IRQHandler                
TC3_IRQHandler                
DMA1_IRQHandler                
DMA2_IRQHandler               
TRNG_IRQHandler               
SEC_FD_IRQHandler             
CPM_IRQHandler                
PMU_RTC_IRQHandler            
SD_IRQHandler                 
DMA2D_IRQHandler              
AESSM4_IRQHandler             
QADC_IRQHandler               
DAC_IRQHandler                
UART5_IRQHandler                      
USB_IRQHandler                        
UART6_IRQHandler              
SPI1_IRQHandler                       
SPI2_IRQHandler                       
SPI3_IRQHandler                       
SPIM1_IRQHandler                      
SPIM2_IRQHandler              
SPIM3_IRQHandler              
UART1_IRQHandler              
UART2_IRQHandler              
PSRAM_IRQHandler                      
GMAC_IRQHandler               
I2C1_IRQHandler                       
EPORT0_0_IRQHandler           
EPORT0_1_IRQHandler           
EPORT0_2_IRQHandler           
EPORT0_3_IRQHandler           
EPORT0_4_IRQHandler           
EPORT0_5_IRQHandler           
EPORT0_6_IRQHandler           
EPORT0_7_IRQHandler           
EPORT1_0_IRQHandler           
EPORT1_1_IRQHandler           
EPORT1_2_IRQHandler           
EPORT1_3_IRQHandler           
EPORT1_4_IRQHandler           
EPORT1_5_IRQHandler           
EPORT1_6_IRQHandler           
EPORT1_7_IRQHandler           
SSISLV1_IRQHandler
JPEG_ACCELERATOR_IRQHandler            
I2C2_IRQHandler           
AXIDMA_IRQHandler    
UART3_IRQHandler                                                        
UART4_IRQHandler                                                        
LCDC_ROT_ERROR_IRQHandler                  
CAN1_BUF00_03_IRQHandler      
CAN1_BUF04_07_IRQHandler      
CAN1_BUF08_11_IRQHandler      
CAN1_BUF12_15_IRQHandler      
CAN1_BUF16_31_IRQHandler      
CAN1_BUF32_63_IRQHandler          
BOFF_TRWARN_IRQHandler        
CAN1_ERR_IRQHandler           
CAN2_BUF00_03_IRQHandler      
CAN2_BUF04_07_IRQHandler      
CAN2_BUF08_11_IRQHandler      
CAN2_BUF12_15_IRQHandler      
CAN2_BUF16_31_IRQHandler      
CAN2_BUF32_63_IRQHandler          
BOFF2_TRWARN_IRQHandler       
CAN2_ERR_IRQHandler                       
I2S1_IRQHandler                 
I2S2_IRQHandler                
LCDC_CRC_ERROR_IRQHandler                     
LCDC_REG_RELOAD_IRQHandler    
PWMT1_IRQHandler                          
PWMT2_IRQHandler               
PWMT3_IRQHandler               
LCDC_BUS_ERROR_IRQHandler      
LCDC_FIFO_UNDERRUN_IRQHandler    
LCDC_FIFO_WARN_IRQHandler        
LCDC_IRQ_SCANLINE_IRQHandler 
IMGAGE_DECODER_IRQHandler
EPORT2_0_IRQHandler           
EPORT2_1_IRQHandler                           
EPORT2_2_IRQHandler           
EPORT2_3_IRQHandler           
EPORT2_4_IRQHandler           
EPORT2_5_IRQHandler           
EPORT2_6_IRQHandler           
EPORT2_7_IRQHandler           
EPORT3_0_IRQHandler           
EPORT3_1_IRQHandler                           
EPORT3_2_IRQHandler           
EPORT3_3_IRQHandler           
EPORT3_4_IRQHandler           
EPORT3_5_IRQHandler           
EPORT3_6_IRQHandler           
EPORT3_7_IRQHandler                           
EPORT4_0_IRQHandler           
EPORT4_1_IRQHandler                           
EPORT4_2_IRQHandler           
EPORT4_3_IRQHandler           
EPORT4_4_IRQHandler           
EPORT4_5_IRQHandler           
EPORT4_6_IRQHandler           
EPORT4_7_IRQHandler           
EPORT5_0_IRQHandler           
EPORT5_1_IRQHandler                           
EPORT5_2_IRQHandler           
EPORT5_3_IRQHandler           
EPORT5_4_IRQHandler           
EPORT5_5_IRQHandler           
EPORT5_6_IRQHandler           
EPORT5_7_IRQHandler                           
EPORT6_0_IRQHandler           
EPORT6_1_IRQHandler                           
EPORT6_2_IRQHandler           
EPORT6_3_IRQHandler           
EPORT6_4_IRQHandler           
EPORT6_5_IRQHandler           
EPORT6_6_IRQHandler           
EPORT6_7_IRQHandler           
EPORT7_0_IRQHandler           
EPORT7_1_IRQHandler                           
EPORT7_2_IRQHandler           
EPORT7_3_IRQHandler           
EPORT7_4_IRQHandler           
EPORT7_5_IRQHandler           
EPORT7_6_IRQHandler           
EPORT7_7_IRQHandler           
EPORT8_0_IRQHandler           
EPORT8_1_IRQHandler                           
EPORT8_2_IRQHandler           
EPORT8_3_IRQHandler           
EPORT8_4_IRQHandler           
EPORT8_5_IRQHandler           
EPORT8_6_IRQHandler           
EPORT8_7_IRQHandler           
EPORT9_0_IRQHandler           
EPORT9_1_IRQHandler                           
EPORT9_2_IRQHandler           
EPORT9_3_IRQHandler           
EPORT9_4_IRQHandler           
EPORT9_5_IRQHandler           
EPORT9_6_IRQHandler           
EPORT9_7_IRQHandler           


                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
