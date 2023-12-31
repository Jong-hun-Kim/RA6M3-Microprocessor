#define _CRT_SECURE_NO_WARNINGS
#include "Library.h"
#include "vocabulary.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
void random();
void compare_string();
void DAC_Sound_Answer();
void DAC_Sound_Warning();
char convert_string(uint32_t data);
void ADC_Read_and_Convert();
void Potentiometer_Read();
void Cds_Sensor_Read();
FSP_CPP_FOOTER

#define CARRIAGE_RETURN      0x0D
#define LINE_FEED            0x0A
#define CDS_VALUE_COND        400

extern volatile operation_state state;
extern volatile uint8_t state_mask;

int select = 0;
uint8_t string[50] = {0}; char temp[50] = {0};
unsigned char line_change[2] = {CARRIAGE_RETURN, LINE_FEED};
volatile uint8_t idx = 0, dataLen = 40, correct = 0, total = 0, dac_state = 0, stop_state = 0;
volatile uint16_t potentiometer_mV = 0, potentiometer_Rb = 0, cds_value = 0;

volatile uint16_t dutyRate = 0;
uint32_t agt1_counter = 0;

extern unsigned char AnswerSound[155616];
extern unsigned char WarningSound[106080];

volatile const uint32_t Timer_Period_2ms = 0x3A980; // 2[ms] Duty Cycle (500[Hz])

const float ADC_CONST = (float)(3.3/4096);

typedef enum {
    CDS_light = 0,
    CDS_dark = 1,
}CDS_t;

CDS_t brightness;

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    Initial_Setting();

    while(true)
    {
        if (state.task_mode_13)
        {
            FND_Print_Data(1, total);
            FND_Print_Data(3, correct);
        }
        else
        {
            FND_Reset();
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}
void DAC_Sound_Answer()
{
    uint16_t value = 0;

    for(uint32_t i = 0; i < sizeof(AnswerSound); i += 2)
    {
        value = (uint16_t)(AnswerSound[i] | (AnswerSound[i + 1] << 8));
        R_DAC_Write(&g_dac0_ctrl, value);
        R_BSP_SoftwareDelay(23, BSP_DELAY_UNITS_MICROSECONDS);
    }
}
void DAC_Sound_Warning()
{
    uint16_t value = 0;

    for(uint32_t i = 0; i < sizeof(WarningSound); i += 2)
    {
        value = (uint16_t)(WarningSound[i] | (WarningSound[i + 1] << 8));
        R_DAC_Write(&g_dac0_ctrl, value);
        R_BSP_SoftwareDelay(42, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

void ADC_Read_and_Convert()
{
    adc_status_t status;

    R_ADC_ScanStart(&g_adc0_ctrl);
    status.state = ADC_STATE_SCAN_IN_PROGRESS;

    while(ADC_STATE_SCAN_IN_PROGRESS == status.state){
        R_ADC_StatusGet(&g_adc0_ctrl, &status);
    }

    Potentiometer_Read();
    Cds_Sensor_Read();
}

void Potentiometer_Read()
{
    uint16_t ch0_adc_result;

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &ch0_adc_result);
    potentiometer_mV = (uint16_t)(ch0_adc_result * ADC_CONST * 1000);
    potentiometer_Rb = (uint16_t)(potentiometer_mV * 3.0303);

    dutyRate = potentiometer_Rb / 100;
    R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
}

void Cds_Sensor_Read()
{
    uint16_t ch2_adc_result;

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_2, &ch2_adc_result);
    cds_value = ch2_adc_result;
}

void External_Interrupt(external_irq_callback_args_t *p_args) // Interrupt Service Routine
{
    switch(p_args->channel)
    {
        case EXTERNAL_INTERRUPT_11:
            ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 1-1 //////////////
            state.task_mode_11 ^= state_mask;
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_08, state.task_mode_11 ^ state_mask);
            //////////////////////////////////////////////////////////////////////////
            break;
        case EXTERNAL_INTERRUPT_12:
            ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 1-2 //////////////
            state.task_mode_12 ^= state_mask;
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_09, state.task_mode_12 ^ state_mask);
            //////////////////////////////////////////////////////////////////////////
            break;
        case EXTERNAL_INTERRUPT_13:
            ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 1-2 //////////////
            state.task_mode_13 ^= state_mask;
            if (state.task_mode_13)
            {
                total = 0;
                correct = 0;
            }
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_10, state.task_mode_13 ^ state_mask);
            //////////////////////////////////////////////////////////////////////////
            break;
        case EXTERNAL_INTERRUPT_14:
            ////////////// YOU MUST EDIT ONLY HERE ABOUT Assignment 1-3 //////////////
            state.task_mode_14 ^= state_mask;
            //////////////////////////////////////////////////////////////////////////
            break;
    }
}

char convert_string(uint32_t data)
{
    if ((data >= 65 && data <= 90) || (data >= 97 && data <= 122))
        return (char)data - 7;
    else if (data >= 48 && data <= 57)
        return (char)data - 15;
    else
        return (char)data + 12;
}

void Callback_UART_0(uart_callback_args_t *p_args)
{
    if(state.task_mode_11)
    {
        if(p_args->data == LINE_FEED)
        {
            idx = 0;
            unsigned char *message;
            message = (unsigned char*)malloc(strlen(temp) + 2);

            memcpy(message, temp, strlen(temp));
            *(message + strlen(temp)) = CARRIAGE_RETURN;
            *(message + strlen(temp) + 1) = LINE_FEED;
            R_SCI_UART_Write(&g_uart0_ctrl, message, strlen(temp) + 2);
            free(message);
            memset(&temp, 0, strlen(temp));
        }
        else if(p_args->event == UART_EVENT_RX_CHAR)
            temp[idx++] = convert_string(p_args->data);
    }
    else if(state.task_mode_13)
    {
        if(p_args->data == LINE_FEED)
        {
            idx = 0;
            compare_string();
            memset(&string, 0, sizeof(string));
        }
        else if(p_args->event == UART_EVENT_RX_CHAR)
            string[idx++] = (uint8_t)p_args->data;
    }

}

void random()
{
    while(true)
    {
        select = rand() % dataLen;
        if (select >= 0 && select < 40)
            break;
    }
}

void compare_string()
{
    uint8_t i = 0, count = 0, compareLen = (uint8_t)strlen(voca[select]);

    for (i = 0; i < compareLen; i++)
        if (string[i] == voca[select][i])
            count++;

    if (count == compareLen)
    {
        correct++;
        DAC_Sound_Answer();
    }
}

void Callback_AGT0(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if (state.task_mode_13)
    {
        total++;
        random(); // Vocabulary Random Selecting
        unsigned char *message;
        message = (unsigned char*)malloc(strlen(voca[select]) + 2);

        memcpy(message, voca[select], strlen(voca[select]));
        *(message + strlen(voca[select])) = CARRIAGE_RETURN;
        *(message + strlen(voca[select]) + 1) = LINE_FEED;
        R_SCI_UART_Write(&g_uart0_ctrl, message, strlen(voca[select]) + 2);
        free(message);
    }
}
void Callback_AGT1(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    if (state.task_mode_12)
    {
        agt1_counter++;

        if (agt1_counter)
        {
            ADC_Read_and_Convert();

            if (cds_value >= 200 && cds_value < 400)
                DAC_Sound_Warning();
            else if (cds_value >= 400)
                dutyRate = 0;

            R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
        }
    }
    else
        R_GPT3->GTCCR[0] = 0;
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
