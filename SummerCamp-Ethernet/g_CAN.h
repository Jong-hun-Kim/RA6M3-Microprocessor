/*
 * g_CAN.h
 *
 *  Created on: 2023. 6. 15.
 *      Author: Jonghun Kim
 */

#include <hal_data.h>

#ifndef G_CAN_H_
#define G_CAN_H_

#define         STANDARD_CAN_FRAME      8

#define         MAILBOX_NUM0            0
#define         MAILBOX_NUM1            1
#define         MAILBOX_NUM2            2
#define         MAILBOX_NUM3            3
#define         MAILBOX_NUM4            4
#define         MAILBOX_NUM5            5
#define         MAILBOX_NUM6            6
#define         MAILBOX_NUM7            7
#define         MAILBOX_NUM8            8
#define         MAILBOX_NUM9            9

void R_CAN_Initial_Setting();
void R_CAN_Frame_Setting(uint32_t id, uint8_t dlc, uint8_t *data);
void R_CAN_Frame_Write(uint32_t id, uint8_t size, uint8_t *ptr);

#endif /* G_CAN_H_ */
