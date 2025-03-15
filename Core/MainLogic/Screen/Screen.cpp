#include "Screen.h"

void Screen::init()
{
	lcd_init();
}

void Screen::print(uint8_t row, uint8_t column, const std::string& data)
{
	lcd_clear();
	lcd_put_cur(row, column);
	lcd_send_string(data.c_str());
}
