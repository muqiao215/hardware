    .syntax unified
    .cpu cortex-m3
    .fpu softvfp
    .thumb

/* Vector table placed at start of FLASH */
    .section .isr_vector, "a", %progbits
    .type   g_pfnVectors, %object
    .size   g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
    .word   _estack             /* Initial stack pointer */
    .word   Reset_Handler       /* Reset */
    .word   NMI_Handler
    .word   HardFault_Handler
    .word   MemManage_Handler
    .word   BusFault_Handler
    .word   UsageFault_Handler
    .word   0
    .word   0
    .word   0
    .word   0
    .word   SVC_Handler
    .word   DebugMon_Handler
    .word   0
    .word   PendSV_Handler
    .word   SysTick_Handler

    /* External interrupts */
    .word   WWDG_IRQHandler
    .word   PVD_IRQHandler
    .word   TAMPER_IRQHandler
    .word   RTC_IRQHandler
    .word   FLASH_IRQHandler
    .word   RCC_IRQHandler
    .word   EXTI0_IRQHandler
    .word   EXTI1_IRQHandler
    .word   EXTI2_IRQHandler
    .word   EXTI3_IRQHandler
    .word   EXTI4_IRQHandler
    .word   DMA1_Channel1_IRQHandler
    .word   DMA1_Channel2_IRQHandler
    .word   DMA1_Channel3_IRQHandler
    .word   DMA1_Channel4_IRQHandler
    .word   DMA1_Channel5_IRQHandler
    .word   DMA1_Channel6_IRQHandler
    .word   DMA1_Channel7_IRQHandler
    .word   ADC1_2_IRQHandler
    .word   USB_HP_CAN1_TX_IRQHandler
    .word   USB_LP_CAN1_RX0_IRQHandler
    .word   CAN1_RX1_IRQHandler
    .word   CAN1_SCE_IRQHandler
    .word   EXTI9_5_IRQHandler
    .word   TIM1_BRK_IRQHandler
    .word   TIM1_UP_IRQHandler
    .word   TIM1_TRG_COM_IRQHandler
    .word   TIM1_CC_IRQHandler
    .word   TIM2_IRQHandler
    .word   TIM3_IRQHandler
    .word   TIM4_IRQHandler
    .word   I2C1_EV_IRQHandler
    .word   I2C1_ER_IRQHandler
    .word   I2C2_EV_IRQHandler
    .word   I2C2_ER_IRQHandler
    .word   SPI1_IRQHandler
    .word   SPI2_IRQHandler
    .word   USART1_IRQHandler
    .word   USART2_IRQHandler
    .word   USART3_IRQHandler
    .word   EXTI15_10_IRQHandler
    .word   RTCAlarm_IRQHandler
    .word   USBWakeUp_IRQHandler

/* Weak default handlers */
    .section .text.Reset_Handler
    .weak   Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    /* Copy data section from FLASH to RAM */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
1:
    cmp r1, r2
    bcc 2f
    b   3f
2:
    ldr r3, [r0], #4
    str r3, [r1], #4
    b   1b

3:
    /* Zero fill the .bss section */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
4:
    cmp r0, r1
    bcc 5f
    b   6f
5:
    str r2, [r0], #4
    b   4b

6:
    bl  SystemInit
    bl  main
    b   .

.size Reset_Handler, .-Reset_Handler

/* Weak aliases for exception handlers */
    .macro DEF_DEFAULT name
    .weak \name
    .type \name, %function
\name:
    b .
    .endm

    DEF_DEFAULT NMI_Handler
    DEF_DEFAULT HardFault_Handler
    DEF_DEFAULT MemManage_Handler
    DEF_DEFAULT BusFault_Handler
    DEF_DEFAULT UsageFault_Handler
    DEF_DEFAULT SVC_Handler
    DEF_DEFAULT DebugMon_Handler
    DEF_DEFAULT PendSV_Handler
    DEF_DEFAULT SysTick_Handler

    DEF_DEFAULT WWDG_IRQHandler
    DEF_DEFAULT PVD_IRQHandler
    DEF_DEFAULT TAMPER_IRQHandler
    DEF_DEFAULT RTC_IRQHandler
    DEF_DEFAULT FLASH_IRQHandler
    DEF_DEFAULT RCC_IRQHandler
    DEF_DEFAULT EXTI0_IRQHandler
    DEF_DEFAULT EXTI1_IRQHandler
    DEF_DEFAULT EXTI2_IRQHandler
    DEF_DEFAULT EXTI3_IRQHandler
    DEF_DEFAULT EXTI4_IRQHandler
    DEF_DEFAULT DMA1_Channel1_IRQHandler
    DEF_DEFAULT DMA1_Channel2_IRQHandler
    DEF_DEFAULT DMA1_Channel3_IRQHandler
    DEF_DEFAULT DMA1_Channel4_IRQHandler
    DEF_DEFAULT DMA1_Channel5_IRQHandler
    DEF_DEFAULT DMA1_Channel6_IRQHandler
    DEF_DEFAULT DMA1_Channel7_IRQHandler
    DEF_DEFAULT ADC1_2_IRQHandler
    DEF_DEFAULT USB_HP_CAN1_TX_IRQHandler
    DEF_DEFAULT USB_LP_CAN1_RX0_IRQHandler
    DEF_DEFAULT CAN1_RX1_IRQHandler
    DEF_DEFAULT CAN1_SCE_IRQHandler
    DEF_DEFAULT EXTI9_5_IRQHandler
    DEF_DEFAULT TIM1_BRK_IRQHandler
    DEF_DEFAULT TIM1_UP_IRQHandler
    DEF_DEFAULT TIM1_TRG_COM_IRQHandler
    DEF_DEFAULT TIM1_CC_IRQHandler
    DEF_DEFAULT TIM2_IRQHandler
    DEF_DEFAULT TIM3_IRQHandler
    DEF_DEFAULT TIM4_IRQHandler
    DEF_DEFAULT I2C1_EV_IRQHandler
    DEF_DEFAULT I2C1_ER_IRQHandler
    DEF_DEFAULT I2C2_EV_IRQHandler
    DEF_DEFAULT I2C2_ER_IRQHandler
    DEF_DEFAULT SPI1_IRQHandler
    DEF_DEFAULT SPI2_IRQHandler
    DEF_DEFAULT USART1_IRQHandler
    DEF_DEFAULT USART2_IRQHandler
    DEF_DEFAULT USART3_IRQHandler
    DEF_DEFAULT EXTI15_10_IRQHandler
    DEF_DEFAULT RTCAlarm_IRQHandler
    DEF_DEFAULT USBWakeUp_IRQHandler

/* Initial stack */
    .section .stack, "aw", %progbits
    .word   0x20005000     /* _estack, matches RAM top (20KB) */
