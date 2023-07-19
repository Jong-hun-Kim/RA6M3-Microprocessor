/*
 * Library.c
 *
 *  Created on: 2023. 02. 09.
 *      Author: Jonghun Kim
 */


/* Don't edit any code in this file(Library.c). */


#include <Library.h>

/* GLOBAL VARIABLE AREA */
volatile operation_state state;
volatile uint8_t state_mask = 0x01;
uint8_t number[10] = {0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8D, 0x80, 0X84};

extern volatile const uint32_t Timer_Period_2ms;

void IRQ_Initial_Setting()
{
    // Interrupt ICU Setting
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);

    IRQ_Enable();
}
void IRQ_Enable()
{
    // Interrupt Set Enable Register Setting (Enable)
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}
void IRQ_Disable()
{
    // Interrupt Set Disable Register Setting (Disable)
    R_ICU_ExternalIrqDisable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq14_ctrl);
}

void Initial_Setting()
{
    state.task_mode_11 = 0;
    state.task_mode_12 = 0;
    state.task_mode_13 = 0;
    state.task_mode_14 = 0;

    srand((unsigned)time(NULL));

    FND_Reset();
    LED_Reset();

    IRQ_Initial_Setting();
    R_AGT0_Setting();
    R_AGT1_Setting();
    R_GPT3_Setting();
    R_UART0_Setting();
    R_ADC_Setting();
    R_DAC_Setting();
}

void R_AGT0_Setting()
{
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Start(&g_timer0_ctrl);
}
void R_AGT1_Setting()
{
    R_AGT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_AGT_Start(&g_timer1_ctrl);
}
void R_GPT3_Setting() // DC Motor PWM Generator
{
    R_MSTP->MSTPCRD_b.MSTPD5 = 0U; // GPT32EHx (x=0 to 3) Module Stop State Cancel

    R_GPT3->GTCR_b.MD = 0U; // GPT32EH3 Count Mode Setting (Saw-wave PWM Mode)
    R_GPT3->GTCR_b.TPCS = 0U; // GPT32EH3 Clock Source Prescale Setting (PCLKD/1)

    R_PMISC->PWPR_b.B0WI = (uint8_t)0U; // PFSWE Bit Write Protection Disable
    R_PMISC->PWPR_b.PFSWE = (uint8_t)1U; // PmnPFS Register Write Protection Disable

    R_PFS->PORT[4].PIN[3].PmnPFS_b.PMR = 1U; // GTIOC3A Pin Peripheral Function Enable
    R_PFS->PORT[4].PIN[3].PmnPFS_b.PSEL = 3U; // Port m/n Pin Function Select: GTIOC3A (GPT3 PWM Output)

    R_PORT9->PCNTR1 |= IO_PCNTR1_MASK;

    R_PORT10->PCNTR1_b.PDR |= 0x0700;
    R_PORT11->PCNTR1_b.PDR |= 0x0001;

    R_GPT3->GTPR = Timer_Period_2ms - 1; // GPT32EH3 Counting Maximum Cycle Setting
    R_GPT3->GTCNT = 0; // GPT32EH3 Counter Initial Value Setting

    R_GPT3->GTIOR_b.GTIOA = 9U; // Compare Matching Output Control Setting
    R_GPT3->GTIOR_b.OAE = 1U; // GPIOCA Output Pin Enable

    R_GPT3->GTCCR[0] = 0U; // 0% Duty Rate (Compare Matching Value Setting)

    R_GPT3->GTCR_b.CST = 1U; // GPT Counting Start
}
void R_UART0_Setting()
{
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
}
void R_ADC_Setting()
{
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
}
void R_DAC_Setting()
{
    R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    R_DAC_Start(&g_dac0_ctrl);
}

void FND_Display_Data(uint8_t digit, uint8_t data)
{
    uint8_t key = 0, idx = 0;

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);
    FND_Reset();

    /* 7-Segment Digit Selection */
    R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[digit], BSP_IO_LEVEL_HIGH);

    /* 7-Segment LED Pin Setting */
    for(idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        key = (uint8_t)(data >> (7 - idx) & 0x01);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], key);
    }
}

void FND_Print_Data(uint8_t digit, uint8_t num)
{
    if(num >= 10)
    {
        FND_Display_Data(digit - 1, number[num / 10]);
        FND_Display_Data(digit, number[num % 10]);
    }
    else
        FND_Display_Data(digit, number[num]);
}

void FND_Reset()
{
    uint8_t idx = 0;

    /* 7-Segment Initialization */
    for (idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        if (idx < DIGIT_INDEX)
            R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[idx], BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], BSP_IO_LEVEL_HIGH);
    }
}

void LED_Reset()
{
    uint8_t idx = 0;

    /* LED Initialization */
    for (idx = 0; idx < LED_INDEX; idx++)
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[idx], BSP_IO_LEVEL_HIGH);
}
