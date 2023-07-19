/*
 * Library.c
 *
 *  Created on: 2023. 1. 20.
 *      Author: J.H.Kim
 */


/* Don't edit any code in this file(Library.c). */

#include "Library.h"

/* GLOBAL VARIABLE AREA */
volatile operation_state state;
volatile servo_state rotate_mode;
extern volatile const uint32_t Timer_Period_20ms;

volatile uint8_t degree = 0;

const double SERVO_MINIMUM_DUTY = 0.03;
const double SERVO_MAXIMUM_DUTY = 0.12;
const double SERVO_EACH_DEGREE = (SERVO_MAXIMUM_DUTY - SERVO_MINIMUM_DUTY) / 180;

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
void R_AGT0_Setting()
{
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Start(&g_timer0_ctrl);
}
void R_GPT_Setting()
{
    R_MSTP->MSTPCRD_b.MSTPD5 = 0U; // GPT32EHx (x=0 to 3) Module Stop State Cancel

    R_PMISC->PWPR_b.B0WI = (uint8_t)0U; // PFSWE Bit Write Protection Disable
    R_PMISC->PWPR_b.PFSWE = (uint8_t)1U; // PmnPFS Register Write Protection Disable

    R_PFS->PORT[4].PIN[3].PmnPFS_b.PMR = 1U; // GTIOC3A Pin Peripheral Function Enable
    R_PFS->PORT[4].PIN[3].PmnPFS_b.PSEL = 3U; // Port m/n Pin Function Select: GTIOC3A (GPT3 PWM Output)

    R_PFS->PORT[4].PIN[15].PmnPFS_b.PMR = 1U; // GTIOC0A Pin Peripheral Function Enable
    R_PFS->PORT[4].PIN[15].PmnPFS_b.PSEL = 3U; // Port m/n Pin Function Select: GTIOC0A (GPT0 PWM Output)

    R_GPT0_Setting();
    R_GPT3_Setting();
}
void Initial_Setting()
{
    state.task_mode_11 = 0;
    state.task_mode_12 = 0;
    state.task_mode_13 = 0;
    state.task_mode_14 = 0;

    R_GPIO_Setting();

    IRQ_Initial_Setting();

    R_AGT0_Setting();
    R_AGT1_Setting();
    R_GPT_Setting();
}
void Rotate_Servo()
{
    double temp_calc = (SERVO_MINIMUM_DUTY + SERVO_EACH_DEGREE * (float)degree);

    R_GPT0->GTCCR[0] = (uint32_t)(Timer_Period_20ms * temp_calc);
}
void R_GPIO_Setting()
{
    R_PORT9->PCNTR1 |= IO_PCNTR1_MASK;

    R_PORT10->PCNTR1_b.PDR |= 0x0700;
    R_PORT11->PCNTR1_b.PDR |= 0x0001;

    LED_Reset();
}
void LED_Reset()
{
    uint8_t idx = 0;

    /* LED Initialization */
    for (idx = 0; idx < LED_INDEX; idx++)
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[idx], BSP_IO_LEVEL_HIGH);
}
