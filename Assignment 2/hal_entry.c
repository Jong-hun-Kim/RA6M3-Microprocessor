#include "Library.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

extern operation_state state;
extern volatile servo_state rotate_mode;
extern volatile const uint8_t state_mask;

extern volatile uint8_t degree;
extern const double SERVO_MINIMUM_DUTY;

volatile uint8_t dutyRate = 0;
volatile uint32_t agt0_counter = 0U;
volatile uint32_t agt1_counter = 0U;
volatile const uint8_t state_mask = 0x01;

////////////// YOU MUST EDIT ONLY HERE ABOUT GLOBAL VARIABLE //////////////
volatile uint8_t led_state = 0;
volatile const uint32_t Timer_Period_2ms = 0x3A980; // 2[ms] Duty Cycle (500[Hz])
volatile const uint32_t Timer_Period_20ms = 0x249F00; // 20[ms] Duty Cycle (50[Hz])
///////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    Initial_Setting();

    while(true);

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

void R_External_Interrupt(external_irq_callback_args_t *p_args) // Interrupt Service Routine
{
    switch(p_args->channel)
    {
        case EXTERNAL_INTERRUPT_11:
            state.task_mode_11 ^= state_mask;
            break;
        case EXTERNAL_INTERRUPT_12:
            state.task_mode_12 ^= state_mask;
            break;
        case EXTERNAL_INTERRUPT_13:
            state.task_mode_13 ^= state_mask;
            break;
        case EXTERNAL_INTERRUPT_14:
            state.task_mode_14 = state_mask;
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_00, BSP_IO_LEVEL_LOW);
            break;
    }
}
void R_AGT0_Interrupt(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt0_counter++;

    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-1 ///////////////
    if(state.task_mode_11)
    {
        led_state ^= 0x01;
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_08, led_state);
    }
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_08, BSP_IO_LEVEL_HIGH);
    ///////////////////////////////////////////////////////////////////////////

    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-2 ///////////////
    if(state.task_mode_13)
    {
        dutyRate = 0;
        state.task_mode_13 = 0;
    }
    else if (state.task_mode_12 == 0 && dutyRate != 0)
    {
        if(agt0_counter % AGT_DECE_PERIOD == 0)
            dutyRate -= DUTY_RATE_UNIT;
    }
    else if (state.task_mode_12 && dutyRate != 100)
    {
        if(agt0_counter % AGT_ACCE_PERIOD == 0)
            dutyRate += DUTY_RATE_UNIT;
    }

    R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
    ///////////////////////////////////////////////////////////////////////////
}
void R_AGT1_Interrupt(void)
{
    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-3 ///////////////
    if (state.task_mode_14)
    {
        agt1_counter++;

        if(agt1_counter <= AGT_SERVO_PERIOD)
            rotate_mode = SERVO_ROTATE_RIGHT;
        else if(agt1_counter > AGT_SERVO_PERIOD)
            rotate_mode = SERVO_ROTATE_LEFT;

        switch(rotate_mode)
        {
            case SERVO_ROTATE_RIGHT:
                if(degree != 180)
                    degree += ANGLE_UNIT;
                break;
            case SERVO_ROTATE_LEFT:
                if(degree != 0)
                    degree -= ANGLE_UNIT;
                if(degree == 0)
                {
                    agt1_counter = 0;
                    state.task_mode_14 = 0;
                    rotate_mode = SERVO_ROTATE_RIGHT;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_00, BSP_IO_LEVEL_HIGH);
                }
                break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////

    R_ICU->IELSR_b[5].IR = 0U; // Interrupt Controller Unit Flag Bit Clear
    Rotate_Servo(); // Servo Motor PWM Control Function
}

void R_GPT0_Setting() // Servo Motor PWM Generator
{
    R_GPT0->GTCR_b.MD = 0U; // GPT32EH0 Count Mode Setting (Saw-wave PWM Mode)
    R_GPT0->GTCR_b.TPCS = 0U; // GPT32EH0 Clock Source Prescale Setting (PCLKD/1)

    R_GPT0->GTPR = Timer_Period_20ms - 1; // GPT32EH0 Counting Maximum Cycle Setting
    R_GPT0->GTCNT = 0; // GPT32EH0 Counter Initial Value Setting

    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-3 ///////////////
    R_GPT0->GTIOR_b.GTIOA = 9U; // Compare Matching Output Control Setting
    R_GPT0->GTIOR_b.OAE = 1U; // GPIOCA Output Pin Enable

    R_GPT0->GTCCR[0] = (uint32_t)(Timer_Period_20ms * SERVO_MINIMUM_DUTY); // GTCCR Initial Setting (Angle = 0[degree])

    R_GPT0->GTCR_b.CST = 1U; // GPT Counting Start
    ///////////////////////////////////////////////////////////////////////////
}
void R_GPT3_Setting() // DC Motor PWM Generator
{
    R_GPT3->GTCR_b.MD = 0U; // GPT32EH3 Count Mode Setting (Saw-wave PWM Mode)
    R_GPT3->GTCR_b.TPCS = 0U; // GPT32EH3 Clock Source Prescale Setting (PCLKD/1)

    R_GPT3->GTPR = Timer_Period_2ms - 1; // GPT32EH3 Counting Maximum Cycle Setting
    R_GPT3->GTCNT = 0; // GPT32EH3 Counter Initial Value Setting

    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-2 ///////////////
    R_GPT3->GTIOR_b.GTIOA = 9U; // Compare Matching Output Control Setting
    R_GPT3->GTIOR_b.OAE = 1U; // GPIOCA Output Pin Enable

    R_GPT3->GTCCR[0] = 0U; // 0% Duty Rate (Compare Matching Value Setting)

    R_GPT3->GTCR_b.CST = 1U; // GPT Counting Start
    ///////////////////////////////////////////////////////////////////////////
}

void R_AGT1_Setting()
{
    R_MSTP->MSTPCRD_b.MSTPD2 = 0U; // AGT1 Module Stop State Cancel

    R_AGT1->AGTMR1 |= 0x41; // AGT1 Mode Register 1 Setting
    R_AGT1->AGTMR2 = 0x03; // AGT1 Mode Register 2 Setting

    ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 2-3 ///////////////
    R_AGT1->AGT = 0x334; // 200[ms] Counting

    NVIC->ISER[0] |= 0x20U;
    NVIC->IP[5] = 0x0B;

    R_ICU->IELSR_b[5].IELS = 0x43U;
    R_ICU->IELSR_b[5].IR = 0U;

    R_AGT1->AGTCR_b.TSTART = 0x01;
    ///////////////////////////////////////////////////////////////////////////
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
