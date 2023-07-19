/*
 * Library.h
 *
 *  Created on: 2023. 1. 20.
 *      Author: J.H.Kim
 */

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "hal_data.h"
#include "stdlib.h"

void IRQ_Initial_Setting();
void IRQ_Enable();
void IRQ_Disable();
void Initial_Setting();
void LED_Reset();
void R_AGT0_Setting();
void R_AGT1_Setting();
void R_GPT_Setting();
void R_GPT0_Setting();
void R_GPT3_Setting();
void R_GPIO_Setting();
void Rotate_Servo();

#define SEGMENT_INDEX               8
#define DIGIT_INDEX                 4
#define LED_INDEX                   4

#define AGT_ACCE_PERIOD             5
#define AGT_DECE_PERIOD             4
#define DUTY_RATE_UNIT              5

#define IO_PCNTR1_MASK              0x00010003

#define ANGLE_UNIT                  30
#define MAX_SERVO_DEGREE            180
#define AGT_SERVO_PERIOD            (MAX_SERVO_DEGREE / ANGLE_UNIT)

typedef const enum _irq_number {
    EXTERNAL_INTERRUPT_11 = 11,
    EXTERNAL_INTERRUPT_12,
    EXTERNAL_INTERRUPT_13,
    EXTERNAL_INTERRUPT_14
} irq_number;

typedef enum _servo_state {
    SERVO_ROTATE_RIGHT = 0,
    SERVO_ROTATE_LEFT
} servo_state;

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
