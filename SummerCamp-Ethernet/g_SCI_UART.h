/*
 * g_sci_uart.h
 *
 *  Created on: 2023. 6. 14.
 *      Author: Jonghun Kim
 */

#include <hal_data.h>
#include <g_Ethernet.h>

#ifndef G_SCI_UART_H_
#define G_SCI_UART_H_

#define STX                     0x02
#define ETX                     0x03
#define CARRIAGE_RETURN         0x0D
#define LINE_FEED               0x0A

#define ETH_START_DEL           '<'

#define ETH_TYPE_DEL            'T'
#define ETH_LEN_DEL             'S'

#define ETH_DATA_DEL1           'M'
#define ETH_DATA_DEL2           'S'
#define ETH_DATA_DEL3           'G'

#define ETH_END_DEL             '>'

#define MAX_DATA_SIZE           4

#define ASCII_NUM_BASE          48
#define ASCII_ALPHA_BASE        87

#define MESSAGE_MAX_SIZE        100

#define ETH_DST_ADDR_IDX        0
#define ETH_SRC_ADDR_IDX        6

typedef enum _ConvertMode ConvertMode;

void R_UART_Initial_Setting();
void R_UART_Mode_Change(char text);
void R_UART_Data_Split(char value);
void R_UART_Data_Size();
char R_UART_Data_Convert(ConvertMode mode, char value);

typedef struct _uartFrameStr {
    uint8_t MAC[ETH_MAC_ADDR_SIZE * 2];
    uint8_t EtherType[ETH_TYPE_SIZE];
    uint8_t DataLen[MAX_DATA_SIZE];
    uint8_t Payload[ETH_MTU_SIZE];
} uartFrameStr;

typedef enum _ConvertMode {
    ASCIItoHEX = 0,
    HEXtoASCII
} ConvertMode;

typedef enum _SplitMode {
    FrameStart = 0,
    EtherType,
    DataLen,
    Payload,
    FrameEnd
} SplitMode;

#endif /* G_SCI_UART_H_ */
