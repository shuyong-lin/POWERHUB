/*
    The MIT License
    Copyright (c) 2025 ElmueSoft / Nakanishi Kiyomaro / Normadotcom
    https://netcult.ch/elmue/CANable Firmware Update
*/

#pragma once

#include "usb_class_dual.h"

extern USBD_CDC_ItfTypeDef USBD_CDC1_InterfaceCallbacks;
extern USBD_CDC_ItfTypeDef USBD_CDC2_InterfaceCallbacks;

uint8_t CDC1_Transmit_FS(uint8_t* Buf, uint16_t Len);
uint8_t CDC2_Transmit_FS(uint8_t* Buf, uint16_t Len);
