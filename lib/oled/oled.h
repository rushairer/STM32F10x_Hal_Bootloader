#ifndef __STM32F10X_ABL_HAL_OLED_H
#define __STM32F10X_ABL_HAL_OLED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"
#include "font.h"

    typedef enum
    {
        OLED_COLOR_NORMAL = 0,
        OlED_COLOR_REVERSED = 1,
    } OLED_Color;

    void OLED_Int(I2C_HandleTypeDef *hi2c);
    void OLED_SetCursor(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y);
    void OLED_CleanBuffer(I2C_HandleTypeDef *hi2c);
    void OLED_RefreshScreen(I2C_HandleTypeDef *hi2c);
    void OLED_SetPixel(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint8_t Color);
    void OLED_SetByte(I2C_HandleTypeDef *hi2c, uint8_t Line, uint8_t Column, uint8_t Data, uint8_t Color);
    void OLED_DrawLine(I2C_HandleTypeDef *hi2c, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Color);
    void OLED_ShowChar(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, char Char, OLED_Font_Size Size, uint8_t Color);
    void OLED_ShowString(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, char *String, OLED_Font_Size Size, uint8_t Color);
    void OLED_ShowNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color);
    void OLED_ShowHexNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color);
    void OLED_ShowBinNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color);
    void OLED_DrawImage(I2C_HandleTypeDef *hi2c, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, const uint8_t BMP[]);

#ifdef __cplusplus
}
#endif

#endif
