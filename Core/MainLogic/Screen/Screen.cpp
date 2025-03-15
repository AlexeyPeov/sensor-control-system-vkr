#include "Screen.h"

void Screen::init(I2C_HandleTypeDef* hi2c1)
{
    HD44780_Init(2, hi2c1);
    HD44780_Display();
}

void Screen::clear() { HD44780_Clear(); }

void Screen::print(uint8_t row, uint8_t column, const std::string& str)
{
    HD44780_SetCursor(row, column);
    HD44780_PrintStr(str);
}
