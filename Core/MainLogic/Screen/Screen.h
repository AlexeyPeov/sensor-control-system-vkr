/*
 * Screen.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_SCREEN_H_
#define MAINLOGIC_SCREEN_H_

#include <string>

#include "screen_i2c/liquidcrystal_i2c.h"

class Screen
{
public:
	static void init(I2C_HandleTypeDef* hi2c1);

private:

	friend class MainLogic;

private:
	static void clear();

	static void print(uint8_t row, uint8_t column, const std::string& data);
};


#endif /* MAINLOGIC_SCREEN_H_ */
