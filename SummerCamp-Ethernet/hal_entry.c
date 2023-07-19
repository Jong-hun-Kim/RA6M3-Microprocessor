#include <g_DeviceDriver.h>
#include <g_Ethernet.h>
#include <g_SCI_UART.h>

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

ethFrameStr     TxFrameBuffer;                 // Set Transmit Ethernet Frame (VLAN Security Assist)
ethFrameStr     RxFrameBuffer;                 // Get Receive Ethernet Frame (VLAN Security Assist)
uint32_t        RxFrameSize = 0;               // Length(Size) of Received Ethernet Message

void hal_entry(void)
{
    HW_Initial_Setting();

    while(true);
}

/* Receive External Interrupt Signal from Outside */
void R_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    switch(p_args->channel)
    {
        case EXTERNAL_INTERRUPT_11:
            break;
        case EXTERNAL_INTERRUPT_12:
            break;
        case EXTERNAL_INTERRUPT_13:
            break;
        case EXTERNAL_INTERRUPT_14:
            break;
    }
}

/* Receive UART Frame from PC */
void R_SCI_UART_Callback(uart_callback_args_t *p_args)
{
    // If SDR (Serial Data Register) is charged, this event(UART_EVENT_RX_CHAR) is occurred.
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            R_UART_Mode_Change((char)p_args->data);
            break;
        default:
            break;
    }
}

/* Receive Ethernet Frame from PC */
void R_Eth_Callback(ether_callback_args_t *p_args)
{
    uint16_t type = 0;

    switch(p_args->event)
    {
        case ETHER_EVENT_INTERRUPT:
            // You must set "RACT" in the Receive Descriptor to 1 after occur Ethernet Handler.
            // If you use "R_ETHER_Read" HAL Function, Receive Descriptor is automatically set.
            R_ETHER_Read(&g_ether0_ctrl, &RxFrameBuffer, &RxFrameSize);
            type = (uint16_t)((RxFrameBuffer.ethType[0] << 8) | RxFrameBuffer.ethType[1]);

            R_Eth_Exercise(type);
            break;
        default:
            break;
    }
}

/* Receive CAN Frame from other Devices */
void R_CAN_Callback(can_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case CAN_EVENT_RX_COMPLETE:
            break;
        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
