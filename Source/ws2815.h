/*
    The MIT License
    Copyright (c) 2025 ElmueSoft / Nakanishi Kiyomaro / Normadotcom
    https://netcult.ch/elmue/CANable Firmware Update
*/

#pragma once

#include "settings.h"
#include "led.h"


#if (LED_WS2815_ENABLE > 0)

typedef struct
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} ws28xx_class;

void ws2815_init(void);
void ws2815_set_rx(uint8_t channel, bool status);
void ws2815_set_tx(uint8_t channel, bool status);
void ws2815_set_pwr(uint8_t status);
void ws2815_update(void);
#else
static inline void ws2815_init(void) {}
static inline void ws2815_set_rx(uint8_t channel, bool status) { (void)channel; (void)status; }
static inline void ws2815_set_tx(uint8_t channel, bool status) { (void)channel; (void)status; }
static inline void ws2815_set_pwr(uint8_t status) { (void)status; }
static inline void ws2815_update(void) {}
#endif
