/*
 * g_pDevice_Driver.c
 *
 *  Created on: 2023. 3. 9.
 *      Author: Jonghun Kim
 */

#include <g_DeviceDriver.h>
#include <g_ethernet.h>
#include <g_SCI_UART.h>
#include <g_CAN.h>

void R_IRQ_Initial_Setting()
{
    /* Interrupt ICU Setting */
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);

    /* Interrupt Set Enable Register Setting (Enable) */
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}
void HW_Initial_Setting()
{
    /* Invoke Interrupt Setting Library */
    R_IRQ_Initial_Setting();
    /* Invoke UART Setting Library */
    R_UART_Initial_Setting();
    /* Invoke Ethernet Setting Library */
    R_Eth_Initial_Setting();
    /* Invoke CAN Setting Library */
    R_CAN_Initial_Setting();
}
