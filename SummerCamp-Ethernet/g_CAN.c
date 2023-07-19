/*
 * g_CAN.c
 *
 *  Created on: 2023. 6. 15.
 *      Author: Jonghun Kim
 */

#include <g_CAN.h>

can_frame_t         g_can_tx_frame;

void R_CAN_Initial_Setting()
{
    R_CAN_Open(&g_can0_ctrl, &g_can0_cfg);
}

void R_CAN_Frame_Setting(uint32_t id, uint8_t dlc, uint8_t *data)
{
    g_can_tx_frame.id = id;
    g_can_tx_frame.id_mode = CAN_ID_MODE_STANDARD;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = dlc;

    memcpy(g_can_tx_frame.data, data, dlc);
}

void R_CAN_Frame_Write(uint32_t id, uint8_t size, uint8_t *ptr)
{
    memset(&g_can_tx_frame, 0, sizeof(can_frame_t));
    R_CAN_Frame_Setting(id, size, ptr);

    // To guarantee the CAN Transmission Delay, you must use the multi-transmit-mailbox.
    // If you use single-mailbox, only first CAN frame is transmitted because of hardware delay.

    // Please check the FSP Configuration Properties about CAN HAL Stack.
    R_CAN_Write(&g_can0_ctrl, id, &g_can_tx_frame);
}
