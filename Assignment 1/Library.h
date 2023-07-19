/*
 * Library.h
 *
 *  Created on: 2022. 12. 22.
 *      Author: Jonghun Kim
 */

/* Don't edit any code in this file(Library.h). */

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "hal_data.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

void IRQ_Initial_Setting();
void IRQ_Enable();
void IRQ_Disable();
void Initial_Setting();
void FND_Display_Data(uint8_t digit, uint8_t data);
void FND_Print_Data(uint8_t *string);
void FND_Reset();
void LED_Reset();

#define SEGMENT_INDEX         8
#define DIGIT_INDEX           4
#define LED_INDEX             4
#define FIRST                 1
#define SECOND                2
#define THIRD                 3

#define ID_LENGTH     10

#define PODR_INDEX_HIGH       7
#define PODR_INDEX_LOW        4
#define PODR_DIGIT_MASK       0x01E0
#define PODR_HIGH_MASK        0x7800
#define PODR_LOW_MASK         0x00F0

static const enum _irq_number {
    EXTERNAL_INTERRUPT_11 = 11,
    EXTERNAL_INTERRUPT_12,
    EXTERNAL_INTERRUPT_13,
    EXTERNAL_INTERRUPT_14
} irq_number;

typedef struct _operation_state {
    volatile uint8_t task_mode_11;
    volatile uint8_t task_mode_12;
    volatile uint8_t task_mode_13;
    volatile uint8_t task_mode_14;
} operation_state;


static const bsp_io_port_pin_t led_pin[4] = {
                                BSP_IO_PORT_10_PIN_08,
                                BSP_IO_PORT_10_PIN_09,
                                BSP_IO_PORT_10_PIN_10,
                                BSP_IO_PORT_11_PIN_00
};
static const bsp_io_port_pin_t segment_digit[4] = {
                                      BSP_IO_PORT_03_PIN_05,
                                      BSP_IO_PORT_03_PIN_06,
                                      BSP_IO_PORT_03_PIN_07,
                                      BSP_IO_PORT_03_PIN_08
};
static const bsp_io_port_pin_t segment_led_pin[8] = {
                                           BSP_IO_PORT_06_PIN_14,
                                           BSP_IO_PORT_06_PIN_04,
                                           BSP_IO_PORT_06_PIN_05,
                                           BSP_IO_PORT_06_PIN_06,
                                           BSP_IO_PORT_06_PIN_07,
                                           BSP_IO_PORT_06_PIN_11,
                                           BSP_IO_PORT_06_PIN_12,
                                           BSP_IO_PORT_06_PIN_13
};

#endif /* LIBRARY_H_ */
