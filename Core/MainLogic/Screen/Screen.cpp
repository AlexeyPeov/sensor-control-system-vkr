#include "Screen.h"

void Screen::init()
{
	HD44780_Init(2);
}

void Screen::print(uint8_t row, uint8_t column, const std::string& str)
{
	HD44780_Clear();
	HD44780_SetCursor(row,column);
	HD44780_PrintStr(str.c_str());
}
