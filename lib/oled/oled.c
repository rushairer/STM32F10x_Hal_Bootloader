#include "oled.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define SSD1306_I2C_ADDR 0x78

void OLED_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t Command);
void OLED_WriteData(I2C_HandleTypeDef *hi2c, uint8_t Data);

uint8_t OLED_Buffer[8][128];

void OLED_Int(I2C_HandleTypeDef *hi2c)
{
    OLED_WriteCommand(hi2c, 0xAE); // 关闭显示
    OLED_WriteCommand(hi2c, 0x00); // set low column address
    OLED_WriteCommand(hi2c, 0X10); // set high column address

    OLED_WriteCommand(hi2c, 0x40); // 设置显示开始行
    OLED_WriteCommand(hi2c, 0xB0); // set page address

    OLED_WriteCommand(hi2c, 0x81); // 设置对比度控制
    OLED_WriteCommand(hi2c, 0x60);

    OLED_WriteCommand(hi2c, 0xA1); // 设置左右方向，0xA1正常 0xA0左右反置
    OLED_WriteCommand(hi2c, 0xA6); // 设置正常/倒转显示
    OLED_WriteCommand(hi2c, 0xA8); // 设置多路复用率
    OLED_WriteCommand(hi2c, 0x3F); // 1/32 duty
    OLED_WriteCommand(hi2c, 0xC8); // 设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteCommand(hi2c, 0xD3); // 设置显示偏移
    OLED_WriteCommand(hi2c, 0x00);

    OLED_WriteCommand(hi2c, 0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(hi2c, 0x80);

    OLED_WriteCommand(hi2c, 0xD8); // set area Color mode off
    OLED_WriteCommand(hi2c, 0x05);

    OLED_WriteCommand(hi2c, 0xD9); // 设置预充电周期
    OLED_WriteCommand(hi2c, 0xF1);

    OLED_WriteCommand(hi2c, 0xDA); // 设置COM引脚硬件配置
    OLED_WriteCommand(hi2c, 0X12);

    OLED_WriteCommand(hi2c, 0xDB); // 设置VCOMH取消选择级别
    OLED_WriteCommand(hi2c, 0x30);

    OLED_WriteCommand(hi2c, 0x8D); // 设置充电泵
    OLED_WriteCommand(hi2c, 0X14);

    OLED_WriteCommand(hi2c, 0xAF); // 开启显示

    OLED_CleanBuffer(hi2c);
    OLED_RefreshScreen(hi2c);
}

void OLED_Display_On(I2C_HandleTypeDef *hi2c)
{
    OLED_WriteCommand(hi2c, 0x8D);
    OLED_WriteCommand(hi2c, 0X14);
    OLED_WriteCommand(hi2c, 0xAF);
}

void OLED_Display_Off(I2C_HandleTypeDef *hi2c)
{
    OLED_WriteCommand(hi2c, 0x8D);
    OLED_WriteCommand(hi2c, 0X10);
    OLED_WriteCommand(hi2c, 0xAE);
}

void OLED_CleanBuffer(I2C_HandleTypeDef *hi2c)
{
    memset(OLED_Buffer, 0, sizeof(OLED_Buffer));
}

void OLED_RefreshScreen(I2C_HandleTypeDef *hi2c)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++)
    {
        OLED_SetCursor(hi2c, 0, i);
        for (j = 0; j < 128; j++)
        {
            OLED_WriteData(hi2c, OLED_Buffer[i][j]);
        }
    }
}

void OLED_SetPixel(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint8_t Color)
{
    if (X >= 128 || Y >= 64)
    {
        return;
    }

    if (Color)
    {
        OLED_Buffer[Y / 8][X] &= ~(1 << (Y % 8));
    }
    else
    {
        OLED_Buffer[Y / 8][X] |= 1 << (Y % 8);
    }
}

void OLED_SetByte(I2C_HandleTypeDef *hi2c, uint8_t Line, uint8_t Column, uint8_t Data, uint8_t Color)
{
    if (Line >= 8 || Column >= 128)
    {
        return;
    }

    if (Color)
    {
        Data = ~Data;
    }

    OLED_Buffer[Line][Column] = Data;
}

void OLED_DrawLine(I2C_HandleTypeDef *hi2c, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Color)
{
    static uint8_t temp = 0;
    if (X1 == X2)
    {
        if (Y1 > Y2)
        {
            temp = Y1;
            Y1 = Y2;
            Y2 = temp;
        }
        for (uint8_t y = Y1; y <= Y2; y++)
        {
            OLED_SetPixel(hi2c, X1, y, Color);
        }
    }
    else if (Y1 == Y2)
    {
        if (X1 > X2)
        {
            temp = X1;
            X1 = X2;
            X2 = temp;
        }
        for (uint8_t x = X1; x <= X2; x++)
        {
            OLED_SetPixel(hi2c, x, Y1, Color);
        }
    }
    else
    {
        // Bresenham直线算法
        int16_t dx = X2 - X1;
        int16_t dy = Y2 - Y1;
        int16_t ux = ((dx > 0) << 1) - 1;
        int16_t uy = ((dy > 0) << 1) - 1;
        int16_t x = X1, y = Y1, eps = 0;
        dx = abs(dx);
        dy = abs(dy);
        if (dx > dy)
        {
            for (x = X1; x != X2; x += ux)
            {
                OLED_SetPixel(hi2c, x, y, Color);
                eps += dy;
                if ((eps << 1) >= dx)
                {
                    y += uy;
                    eps -= dx;
                }
            }
        }
        else
        {
            for (y = Y1; y != Y2; y += uy)
            {
                OLED_SetPixel(hi2c, x, y, Color);
                eps += dx;
                if ((eps << 1) >= dy)
                {
                    x += ux;
                    eps -= dy;
                }
            }
        }
    }
}

void OLED_ShowChar(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, char Char, OLED_Font_Size Size, uint8_t Color)
{
    uint8_t temp, i, j;
    uint8_t y0 = Y;
    Char = Char - ' ';
    for (i = 0; i < Size; i++)
    {
        switch (Size)
        {
        case 12:
            temp = OLED_F6x12[(uint8_t)Char][i];
            break;
        default:
            temp = OLED_F8x16[(uint8_t)Char][i];
            break;
        }

        for (j = 0; j < 8; j++)
        {
            if (temp & 0x80)
            {
                OLED_SetPixel(hi2c, X, Y, Color);
            }
            else
            {
                OLED_SetPixel(hi2c, X, Y, !Color);
            }
            temp <<= 1;
            Y++;
            if ((Y - y0) == Size)
            {
                Y = y0;
                X++;
                break;
            }
        }
    }
}

uint8_t OLED_FontWidth(I2C_HandleTypeDef *hi2c, OLED_Font_Size Size)
{
    uint8_t width;
    switch (Size)
    {
    case 12:
        width = 6;
        break;

    default:
        width = 8;
        break;
    }

    return width;
}

void OLED_ShowString(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, char *String, OLED_Font_Size Size, uint8_t Color)
{
    uint8_t i, width;

    width = OLED_FontWidth(hi2c, Size);

    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(hi2c, X + i * width, Y, String[i], Size, Color);
    }
}

void OLED_ShowNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color)
{
    uint8_t i, width;
    width = OLED_FontWidth(hi2c, Size);
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(hi2c, X + i * width, Y, Number / (uint32_t)pow(10, Length - i - 1) % 10 + '0', Size, Color);
    }
}

void OLED_ShowHexNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color)
{
    uint8_t i, width, SingleNumber;
    width = OLED_FontWidth(hi2c, Size);

    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / (uint32_t)pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(hi2c, X + i * width, Y, SingleNumber + '0', Size, Color);
        }
        else
        {
            OLED_ShowChar(hi2c, X + i * width, Y, SingleNumber - 10 + 'A', Size, Color);
        }
    }
}

void OLED_ShowBinNumber(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, OLED_Font_Size Size, uint8_t Color)
{
    uint8_t i, width;
    width = OLED_FontWidth(hi2c, Size);
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(hi2c, X + i * width, Y, Number / (uint32_t)pow(2, Length - i - 1) % 2 + '0', Size, Color);
    }
}

void OLED_DrawImage(I2C_HandleTypeDef *hi2c, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, const uint8_t BMP[])
{
    uint16_t i = 0, X, Y;
    for (Y = Y1; Y < (Y2 + 1) / 8; Y++)
    {
        for (X = X1; X < X2 + 1; X++)
        {
            OLED_Buffer[Y][X] = BMP[i++];
        }
    }
}

void OLED_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t Command)
{
    HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x00, 1, &Command, 1, 10);
}

void OLED_WriteData(I2C_HandleTypeDef *hi2c, uint8_t Data)
{
    HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &Data, 1, 10);
}

void OLED_SetCursor(I2C_HandleTypeDef *hi2c, uint8_t X, uint8_t Y)
{
    OLED_WriteCommand(hi2c, 0xB0 + Y);                 // 设置Y位置
    OLED_WriteCommand(hi2c, 0X10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    OLED_WriteCommand(hi2c, 0x00 | (X & 0x0F));        // 设置X位置低4位
}