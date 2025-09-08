#include "bsp.h"
// 系统时钟频率定义
#define SYSTEM_CORE_CLOCK 64000000  // 64MHz

static const uint8_t seg_code[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

static void TM1723_WriteByte(uint8_t data);

static void Delay_us(uint32_t us)
{
    SysTick->LOAD = us * (SYSTEM_CORE_CLOCK / 1000000) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL = 0;
}

void TM1723_Init(void)
{
    STB_LOW();
    TM1723_WriteByte(0x8F); // 显示开，亮度最大
    STB_HIGH();
}


static void TM1723_Delay(void)
{
    for (volatile int i = 0; i < 50; i++); // 简单延时
}



static void TM1723_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        SCLK_LOW();
        if (data & 0x01)
            DIO_HIGH();
        else
            DIO_LOW();
        TM1723_Delay();
        SCLK_HIGH();
        TM1723_Delay();
        data >>= 1;
    }
}

void TM1723_DisplayDigit(uint8_t pos, uint8_t num)
{
    STB_LOW();
    TM1723_WriteByte(0x44); // 固定地址模式
    STB_HIGH();

    STB_LOW();
    TM1723_WriteByte(0xC0 | pos); // 地址
    TM1723_WriteByte(seg_code[num]);
    STB_HIGH();
}


// 显示数字(0-9999)
void TM1723_DisplayNumber(uint16_t number) 
{
    if (number > 9999) number = 9999;
    
    uint8_t digits[4];
    digits[0] = number / 1000;          // 千位
    digits[1] = (number / 100) % 10;    // 百位
    digits[2] = (number / 10) % 10;     // 十位
    digits[3] = number % 10;            // 个位
    
    // 显示数字，从右到左
    for (uint8_t i = 0; i < 4; i++) {
        TM1723_DisplayDigit(3 - i, digits[i]);
    }
}

