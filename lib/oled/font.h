#ifndef __STM32F10X_ABL_HAL_OLED_FONT_H
#define __STM32F10X_ABL_HAL_OLED_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"

    typedef enum
    {
        OLED_FONT_SIZE_12 = 12,
        OLED_FONT_SIZE_16 = 16
    } OLED_Font_Size;

    extern const uint8_t OLED_F6x12[][12];
    extern const uint8_t OLED_F8x16[][16];

#ifdef __cplusplus
}
#endif

#endif