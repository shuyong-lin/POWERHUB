/*
    The MIT License
    Copyright (c) 2025 ElmueSoft / Nakanishi Kiyomaro / Normadotcom
    https://netcult.ch/elmue/CANable Firmware Update
*/

#include "buffer_dual.h"
#include "can.h"
#include "led.h"
#include "usb_interface_dual.h"
#include "control.h"

// Buffer structures
cdc1_tx_buf             buf_cdc1_tx;
cdc1_rx_buf             buf_cdc1_rx;
cdc2_tx_buf             buf_cdc2_tx;
cdc2_rx_buf             buf_cdc2_rx;
can_tx_buf              buf_can_tx;

void buf_init()
{
    // Initialize CDC1 buffers
    buf_cdc1_tx.head = 0;
    buf_cdc1_tx.tail = 0;
    buf_cdc1_rx.head = 0;
    buf_cdc1_rx.tail = 0;

    // Initialize CDC2 buffers
    buf_cdc2_tx.head = 0;
    buf_cdc2_tx.tail = 0;
    buf_cdc2_rx.head = 0;
    buf_cdc2_rx.tail = 0;

    // Initialize CAN buffers
    buf_can_tx.head = 0;
    buf_can_tx.send = 0;
    buf_can_tx.tail = 0;
    buf_can_tx.full = false;
}

void buf_process(uint8_t channel, uint32_t tick_now)
{
    // Process CDC1 RX buffer
    while (buf_cdc1_rx.head != buf_cdc1_rx.tail)
    {
        // Process message from buffer
        control_parse_command(channel, (uint8_t*)buf_cdc1_rx.data[buf_cdc1_rx.tail], buf_cdc1_rx.msglen[buf_cdc1_rx.tail], tick_now);

        // Move tail pointer
        buf_cdc1_rx.tail = (buf_cdc1_rx.tail + 1) % BUF_CDC1_RX_NUM_BUFS;
    }

    // Process CDC2 RX buffer
    while (buf_cdc2_rx.head != buf_cdc2_rx.tail)
    {
        // Process message from buffer
        control_parse_command(channel, (uint8_t*)buf_cdc2_rx.data[buf_cdc2_rx.tail], buf_cdc2_rx.msglen[buf_cdc2_rx.tail], tick_now);

        // Move tail pointer
        buf_cdc2_rx.tail = (buf_cdc2_rx.tail + 1) % BUF_CDC2_RX_NUM_BUFS;
    }

    // Process CAN TX buffer
    while (buf_can_tx.head != buf_can_tx.tail || buf_can_tx.full)
    {
        // Send frame from buffer
        if (can_send_frame(channel, &buf_can_tx.header[buf_can_tx.tail], buf_can_tx.data[buf_can_tx.tail]) == FBK_Success)
        {
            // Move tail pointer
            buf_can_tx.tail = (buf_can_tx.tail + 1) % BUF_CAN_TXQUEUE_LEN;
            buf_can_tx.full = false;
        }
        else
        {
            // Failed to send, break out of loop
            break;
        }
    }

    // Process CDC1 TX buffer
    while (buf_cdc1_tx.head != buf_cdc1_tx.tail)
    {
        // Send message to host
        if (CDC1_Transmit_FS((uint8_t*)buf_cdc1_tx.data[buf_cdc1_tx.tail], buf_cdc1_tx.msglen[buf_cdc1_tx.tail]) == USBD_OK)
        {
            // Move tail pointer
            buf_cdc1_tx.tail = (buf_cdc1_tx.tail + 1) % BUF_CDC1_TX_NUM_BUFS;
        }
        else
        {
            // Failed to send, break out of loop
            break;
        }
    }

    // Process CDC2 TX buffer
    while (buf_cdc2_tx.head != buf_cdc2_tx.tail)
    {
        // Send message to host
        if (CDC2_Transmit_FS((uint8_t*)buf_cdc2_tx.data[buf_cdc2_tx.tail], buf_cdc2_tx.msglen[buf_cdc2_tx.tail]) == USBD_OK)
        {
            // Move tail pointer
            buf_cdc2_tx.tail = (buf_cdc2_tx.tail + 1) % BUF_CDC2_TX_NUM_BUFS;
        }
        else
        {
            // Failed to send, break out of loop
            break;
        }
    }
}

void buf_enqueue_cdc1(uint8_t channel, char* buf, uint16_t len)
{
    // Check for overflow!
    uint32_t new_head = (buf_cdc1_tx.head + 1) % BUF_CDC1_TX_NUM_BUFS;
    if (new_head == buf_cdc1_tx.tail)
    {
        // Buffer overflow! This should never happen.
        error_assert(channel, APP_CanTxOverflow, false);
        return;
    }

    // Copy data to buffer
    memcpy(buf_cdc1_tx.data[buf_cdc1_tx.head], buf, len);
    buf_cdc1_tx.msglen[buf_cdc1_tx.head] = len;
    buf_cdc1_tx.head = new_head;
}

void buf_enqueue_cdc2(uint8_t channel, char* buf, uint16_t len)
{
    // Check for overflow!
    uint32_t new_head = (buf_cdc2_tx.head + 1) % BUF_CDC2_TX_NUM_BUFS;
    if (new_head == buf_cdc2_tx.tail)
    {
        // Buffer overflow! This should never happen.
        error_assert(channel, APP_CanTxOverflow, false);
        return;
    }

    // Copy data to buffer
    memcpy(buf_cdc2_tx.data[buf_cdc2_tx.head], buf, len);
    buf_cdc2_tx.msglen[buf_cdc2_tx.head] = len;
    buf_cdc2_tx.head = new_head;
}

void buf_clear_can_buffer(uint8_t channel)
{
    // Clear CAN TX buffer
    buf_can_tx.head = 0;
    buf_can_tx.send = 0;
    buf_can_tx.tail = 0;
    buf_can_tx.full = false;
}

void buf_store_tx_echo(uint8_t channel, FDCAN_TxEventFifoTypeDef* tx_event)
{
    // This function is not used in Slcan mode
}

eFeedback buf_store_tx_packet(uint8_t channel, FDCAN_TxHeaderTypeDef* tx_header, uint8_t* tx_data)
{
    // Check for overflow!
    if (((buf_can_tx.head + 1) % BUF_CAN_TXQUEUE_LEN) == buf_can_tx.tail)
    {
        // Buffer full!
        return FBK_TxBufferFull;
    }

    // Copy header to buffer
    memcpy(&buf_can_tx.header[buf_can_tx.head], tx_header, sizeof(FDCAN_TxHeaderTypeDef));

    // Copy data to buffer
    memcpy(buf_can_tx.data[buf_can_tx.head], tx_data, tx_header->DataLength);

    // Move head pointer
    buf_can_tx.head = (buf_can_tx.head + 1) % BUF_CAN_TXQUEUE_LEN;
    buf_can_tx.full = (buf_can_tx.head == buf_can_tx.tail);

    return FBK_Success;
}

void buf_store_rx_packet(uint8_t channel, FDCAN_RxHeaderTypeDef* rx_header, uint8_t* rx_data)
{
    // This function is not used in Slcan mode
}
