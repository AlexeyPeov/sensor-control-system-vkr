#ifndef LIQUIDCRYSTAL_I2C_H_
#define LIQUIDCRYSTAL_I2C_H_

#include "stm32f1xx_hal.h"
#include <string>

void HD44780_Init(uint8_t rows,I2C_HandleTypeDef* hi2c1);
void HD44780_Clear();
void HD44780_Home();
void HD44780_NoDisplay();
void HD44780_Display();
void HD44780_NoBlink();
void HD44780_Blink();
void HD44780_NoCursor();
void HD44780_Cursor();
void HD44780_ScrollDisplayLeft();
void HD44780_ScrollDisplayRight();
void HD44780_PrintLeft();
void HD44780_PrintRight();
void HD44780_LeftToRight();
void HD44780_RightToLeft();
void HD44780_ShiftIncrement();
void HD44780_ShiftDecrement();
void HD44780_NoBacklight();
void HD44780_Backlight();
void HD44780_AutoScroll();
void HD44780_NoAutoScroll();
void HD44780_CreateSpecialChar(uint8_t, uint8_t[]);
void HD44780_PrintSpecialChar(uint8_t);
void HD44780_SetCursor(uint8_t, uint8_t);
void HD44780_SetBacklight(uint8_t new_val);
void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows);
void HD44780_PrintStr(const std::string& str);

#endif /* LIQUIDCRYSTAL_I2C_H_ */
