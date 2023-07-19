/*
 * g_sci_uart.c
 *
 *  Created on: 2023. 6. 14.
 *      Author: Jonghun Kim
 */

#include <g_SCI_UART.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

extern ethFrameStr          TxFrameBuffer;

uint32_t                    index = 0;
uint8_t                     data[1500] = {0};

uint8_t                     length_count = 0;
uint16_t                    total_length = 0;
uint32_t                    uart_buffer_idx = 0;
char                        temp = 0;

SplitMode                   splitmode;
uartFrameStr                EthFrame;

void R_UART_Initial_Setting()
{
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
}

void R_UART_Mode_Change(char text)
{
    switch(text)
    {
        case ETH_START_DEL: // UART Ethernet Frame Start Delimiter: '<'
            length_count = 0;
            total_length = 0;
            memset(&TxFrameBuffer, 0, sizeof(ethFrameStr));
            memset(&EthFrame, 0, sizeof(uartFrameStr));
            uart_buffer_idx = 0;
            splitmode = FrameStart;
            break;
        case ETH_TYPE_DEL: // UART Ethernet Frame Type Delimiter: 'T'
            uart_buffer_idx = 0;
            splitmode = EtherType;
            break;
        case ETH_LEN_DEL: // UART Ethernet Frame Payload Length Delimiter: 'S'
            uart_buffer_idx = 0;
            splitmode = DataLen;
            break;
        case ETH_DATA_DEL1: case ETH_DATA_DEL3: // UART Ethernet Frame Payload Delimiter: "MSG"
            uart_buffer_idx = 0;
            splitmode = Payload;
            break;
        case ETH_END_DEL: // UART Ethernet Frame End Delimiter: '>'
            uart_buffer_idx = 0;
            splitmode = FrameEnd;
            R_UART_Data_Split(R_UART_Data_Convert(ASCIItoHEX, text));
            break;
        default:
            if (splitmode != Payload)
                R_UART_Data_Split(R_UART_Data_Convert(ASCIItoHEX, text));
            else
                R_UART_Data_Split(text);
            break;
    }

    data[index++] = text;
}

void R_UART_Data_Split(char value)
{
    switch(splitmode)
    {
        case FrameStart:
            if (++uart_buffer_idx % 2)
                EthFrame.MAC[(uart_buffer_idx - 1) / 2] = (uint8_t)((temp << 4) | value);
            else
                temp = value;
            break;
        case EtherType:
            if (uart_buffer_idx++ % 2)
                EthFrame.EtherType[(uart_buffer_idx - 1) / 2] = (uint8_t)((temp << 4 | value));
            else
                temp = value;
            break;
        case DataLen:
            EthFrame.DataLen[length_count++] = (uint8_t)value;
            break;
        case Payload:
            EthFrame.Payload[uart_buffer_idx++] = (uint8_t)value;
            break;
        case FrameEnd:
            R_UART_Data_Size();
            R_Eth_Write(&(EthFrame.MAC[ETH_DST_ADDR_IDX]), &(EthFrame.MAC[ETH_SRC_ADDR_IDX]));
            break;
    }
}

void R_UART_Data_Size()
{
    for (uint8_t i = 0; i < length_count; i++)
        total_length += (uint16_t)(EthFrame.DataLen[i] * pow(10, length_count - 1 - i));

    TxFrameBuffer.payload[0] = (uint8_t)((total_length >> STRUCT_SHIFT_SIZE) & 0xFF);
    TxFrameBuffer.payload[1] = (uint8_t)(total_length & 0xFF);
}

char R_UART_Data_Convert(ConvertMode mode, char value)
{
    switch(mode)
    {
        case ASCIItoHEX:
            if (value <= '9')
                return value - ASCII_NUM_BASE;
            else
                return value - ASCII_ALPHA_BASE;
            break;
        case HEXtoASCII:
            if (value <= '9')
                return value + ASCII_NUM_BASE;
            else
                return value + ASCII_ALPHA_BASE;
            break;
    }
    return 0;
}
