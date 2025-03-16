/*
 * liquidcrystal_i2c.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#include "liquidcrystal_i2c.h"
#include "../../fn/fn.h"

#include "../../print/print.h"

/* Command */
constexpr int8_t LCD_CLEARDISPLAY =  0x01;
constexpr int8_t LCD_RETURNHOME =  0x02;
constexpr int8_t LCD_ENTRYMODESET =  0x04;
constexpr int8_t LCD_DISPLAYCONTROL =  0x08;
constexpr int8_t LCD_CURSORSHIFT =  0x10;
constexpr int8_t LCD_FUNCTIONSET =  0x20;
constexpr int8_t LCD_SETCGRAMADDR =  0x40;
constexpr int8_t LCD_SETDDRAMADDR =  0x80;

/* Entry Mode */
constexpr int8_t LCD_ENTRYRIGHT = 0x00;
constexpr int8_t LCD_ENTRYLEFT = 0x02;
constexpr int8_t LCD_ENTRYSHIFTINCREMENT = 0x01;
constexpr int8_t LCD_ENTRYSHIFTDECREMENT = 0x00;

/* Display On/Off */
constexpr int8_t LCD_DISPLAYON = 0x04;
constexpr int8_t LCD_DISPLAYOFF = 0x00;
constexpr int8_t LCD_CURSORON = 0x02;
constexpr int8_t LCD_CURSOROFF = 0x00;
constexpr int8_t LCD_BLINKON = 0x01;
constexpr int8_t LCD_BLINKOFF = 0x00;

/* Cursor Shift */
constexpr int8_t LCD_DISPLAYMOVE = 0x08;
constexpr int8_t LCD_CURSORMOVE = 0x00;
constexpr int8_t LCD_MOVERIGHT = 0x04;
constexpr int8_t LCD_MOVELEFT = 0x00;

/* Function Set */
constexpr int8_t LCD_8BITMODE = 0x10;
constexpr int8_t LCD_4BITMODE = 0x00;
constexpr int8_t LCD_2LINE = 0x08;
constexpr int8_t LCD_1LINE = 0x00;
constexpr int8_t LCD_5x10DOTS = 0x04;
constexpr int8_t LCD_5x8DOTS = 0x00;

/* Backlight */
constexpr int8_t LCD_BACKLIGHT = 0x08;
constexpr int8_t LCD_NOBACKLIGHT = 0x00;

/* Enable Bit */
constexpr int8_t ENABLE_CMD = 0x04;

/* Read Write Bit */
//#define RW 0x0

/* Register Select Bit */
constexpr int8_t RS = 0x01;

/* Device I2C Address */
constexpr int8_t DEVICE_ADDR = 0x4E;



uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);
static void DelayInit(void);

static I2C_HandleTypeDef* hi2c1 = nullptr;

uint8_t special1[8] = { 0b00000, 0b11001, 0b11011, 0b00110,
                        0b01100, 0b11011, 0b10011, 0b00000 };

uint8_t special2[8] = { 0b11000, 0b11000, 0b00110, 0b01001,
                        0b01000, 0b01001, 0b00110, 0b00000 };

void HD44780_Init(uint8_t rows, I2C_HandleTypeDef* hi2c1Ptr)
{
    hi2c1 = hi2c1Ptr;
    dpRows = rows;

    dpBacklight = LCD_BACKLIGHT;

    dpFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

    if (dpRows > 1)
    {
        dpFunction |= LCD_2LINE;
    }
    else
    {
        dpFunction |= LCD_5x10DOTS;
    }

    /* Wait for initialization */
    DelayInit();
    HAL_Delay(50);

    ExpanderWrite(dpBacklight);
    HAL_Delay(1000);

    /* 4bit Mode */
    Write4Bits(0x03 << 4);
    fn::DelayMicro(4500);

    Write4Bits(0x03 << 4);
    fn::DelayMicro(4500);

    Write4Bits(0x03 << 4);
    fn::DelayMicro(4500);

    Write4Bits(0x02 << 4);
    fn::DelayMicro(100);

    /* Display Control */
    SendCommand(LCD_FUNCTIONSET | dpFunction);

    dpControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    HD44780_Display();
    HD44780_Clear();

    /* Display Mode */
    dpMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    SendCommand(LCD_ENTRYMODESET | dpMode);
    fn::DelayMicro(4500);

    HD44780_CreateSpecialChar(0, special1);
    HD44780_CreateSpecialChar(1, special2);

    HD44780_Home();
}

void HD44780_Clear()
{
    SendCommand(LCD_CLEARDISPLAY);
    fn::DelayMicro(2000);
}

void HD44780_Home()
{
    SendCommand(LCD_RETURNHOME);
    fn::DelayMicro(2000);
}

void HD44780_SetCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row >= dpRows)
    {
        row = dpRows - 1;
    }
    SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_NoDisplay()
{
    dpControl &= ~LCD_DISPLAYON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Display()
{
    dpControl |= LCD_DISPLAYON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_NoCursor()
{
    dpControl &= ~LCD_CURSORON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Cursor()
{
    dpControl |= LCD_CURSORON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_NoBlink()
{
    dpControl &= ~LCD_BLINKON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Blink()
{
    dpControl |= LCD_BLINKON;
    SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_ScrollDisplayLeft(void)
{
    SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void HD44780_ScrollDisplayRight(void)
{
    SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void HD44780_LeftToRight(void)
{
    dpMode |= LCD_ENTRYLEFT;
    SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_RightToLeft(void)
{
    dpMode &= ~LCD_ENTRYLEFT;
    SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_AutoScroll(void)
{
    dpMode |= LCD_ENTRYSHIFTINCREMENT;
    SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_NoAutoScroll(void)
{
    dpMode &= ~LCD_ENTRYSHIFTINCREMENT;
    SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x7;
    SendCommand(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
    {
        SendChar(charmap[i]);
    }
}

void HD44780_PrintSpecialChar(uint8_t index) { SendChar(index); }

void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t* rows)
{
    HD44780_CreateSpecialChar(char_num, rows);
}

void HD44780_PrintStr(const std::string& str)
{
    // debug("print string: %s, size: %d", str.c_str(), str.size());

    for (auto c : str)
    {
        SendChar(c);
    }
}

void HD44780_SetBacklight(uint8_t new_val)
{
    if (new_val)
        HD44780_Backlight();
    else
        HD44780_NoBacklight();
}

void HD44780_NoBacklight(void)
{
    dpBacklight = LCD_NOBACKLIGHT;
    ExpanderWrite(0);
}

void HD44780_Backlight(void)
{
    dpBacklight = LCD_BACKLIGHT;
    ExpanderWrite(0);
}

static void SendCommand(uint8_t cmd) { Send(cmd, 0); }

static void SendChar(uint8_t ch)
{
    // debug("send char %c", ch);
    Send(ch, RS);
}

static void Send(uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    Write4Bits((highnib) | mode);
    Write4Bits((lownib) | mode);
}

static void Write4Bits(uint8_t value)
{
    ExpanderWrite(value);
    PulseEnable(value);
}

static void ExpanderWrite(uint8_t _data)
{
    uint8_t data = _data | dpBacklight;
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(
        hi2c1,
        DEVICE_ADDR,
        (uint8_t*)&data,
        1,
        10
    );

    if (ret != HAL_OK)
    {
        debug("HAL_I2C_Master_Transmit not ok, %d, %d", ret, data);
    }
    //  else
    //  {
    //	  debug("HAL_I2C_Master_Transmit OK, %d, %d", ret, data);
    //  }
}

static void PulseEnable(uint8_t _data)
{
    ExpanderWrite(_data | ENABLE_CMD);
    fn::DelayMicro(20);

    ExpanderWrite(_data & ~ENABLE_CMD);
    fn::DelayMicro(20);
}

static void DelayInit(void)
{
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 0x00000001;

    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile("NOP");
    __ASM volatile("NOP");
    __ASM volatile("NOP");
}
