/**
  ******************************************************************************
  * @file    usbd_cdc.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_cdc.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include  "usb_ioreq.h"

// First CDC interface endpoints
#define CDC1_IN_EP                                   0x81U  /* EP1 IN  for Rx data */
#define CDC1_OUT_EP                                  0x01U  /* EP1 OUT for Tx data */
#define CDC1_NOTI_EP                                 0x82U  /* EP2 IN  for notification events */

// Second CDC interface endpoints
#define CDC2_IN_EP                                   0x85U  /* EP5 IN  for Rx data */
#define CDC2_OUT_EP                                  0x05U  /* EP5 OUT for Tx data */
#define CDC2_NOTI_EP                                 0x86U  /* EP6 IN  for notification events */

#ifndef CDC_HS_BINTERVAL
    #define CDC_HS_BINTERVAL                        0x10U // 16 ms interrupt IN packets
#endif

#ifndef CDC_FS_BINTERVAL
    #define CDC_FS_BINTERVAL                        0x10U // 16 ms interrupt IN packets
#endif

// The count of USB interface descriptors declared in the configuration descriptor (also needed for error checking)
// IMPORTANT: All interface descriptors must have an ascending bInterfaceNumber: 0, 1, 2, 3, 4...
// interface 0 = CDC1 Control --> 1 interrupt endpoint
// interface 1 = CDC1 Data    --> 2 bulk endpoints
// interface 2 = CDC2 Control --> 1 interrupt endpoint
// interface 3 = CDC2 Data    --> 2 bulk endpoints
#define USBD_INTERFACES_COUNT                       4

// CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance.
// EMZ this is unused. Set to same as FS. Endpoint IN & OUT Packet size
#define CDC_DATA_FS_MAX_PACKET_SIZE                 64U  // Endpoint IN & OUT Packet size for Full Speed
#define CDC_DATA_HS_MAX_PACKET_SIZE                 64U  // Endpoint IN & OUT Packet size for High Speed
#define CDC_NOTI_PACKET_SIZE                         8U  // Notification Endpoint Packet size

#define USB_CDC_CONFIG_DESC_SIZE                    67U
#define CDC_DATA_HS_IN_PACKET_SIZE                  CDC_DATA_HS_MAX_PACKET_SIZE // High Speed
#define CDC_DATA_HS_OUT_PACKET_SIZE                 CDC_DATA_HS_MAX_PACKET_SIZE

#define CDC_DATA_FS_IN_PACKET_SIZE                  CDC_DATA_FS_MAX_PACKET_SIZE // Full Speed
#define CDC_DATA_FS_OUT_PACKET_SIZE                 CDC_DATA_FS_MAX_PACKET_SIZE

#define CDC_SEND_ENCAPSULATED_COMMAND               0x00U
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01U
#define CDC_SET_COMM_FEATURE                        0x02U
#define CDC_GET_COMM_FEATURE                        0x03U
#define CDC_CLEAR_COMM_FEATURE                      0x04U
#define CDC_SET_LINE_CODING                         0x20U
#define CDC_GET_LINE_CODING                         0x21U
#define CDC_SET_CONTROL_LINE_STATE                  0x22U
#define CDC_SEND_BREAK                              0x23U

typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef;

typedef struct _USBD_CDC_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);

} USBD_CDC_ItfTypeDef;

typedef struct
{
  uint32_t data[CDC_DATA_HS_MAX_PACKET_SIZE / 4U];      // Force 32 bit alignment
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
}
USBD_CDC_HandleTypeDef;

// CDC1 and CDC2 interface handles
extern USBD_CDC_HandleTypeDef CDC1_Handle;
extern USBD_CDC_HandleTypeDef CDC2_Handle;

uint8_t  USBD_CDC1_SetTxBuffer(uint8_t *pbuff, uint16_t length);
uint8_t  USBD_CDC1_SetRxBuffer(uint8_t *pbuff);
uint8_t  USBD_CDC1_ReceivePacket();
uint8_t  USBD_CDC1_TransmitPacket();

// CDC1 functions
uint8_t CDC1_Transmit_FS(uint8_t* Buf, uint16_t Len);

// CDC2 functions
uint8_t USBD_CDC2_SetTxBuffer(uint8_t *pbuff, uint16_t length);
uint8_t USBD_CDC2_SetRxBuffer(uint8_t *pbuff);
uint8_t USBD_CDC2_ReceivePacket();
uint8_t USBD_CDC2_TransmitPacket();
uint8_t CDC2_Transmit_FS(uint8_t* Buf, uint16_t Len);

bool               USBD_SetupStageRequest();
USBD_StatusTypeDef USBD_ConfigureEndpoints();

#ifdef __cplusplus
}
#endif
