/*
 * print.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_PRINT_PRINT_H_
#define MAINLOGIC_PRINT_PRINT_H_


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../../Inc/main.h"

extern UART_HandleTypeDef huart1;

#include <sstream>
#include "main.h" // Include HAL and peripherals

void print(const char *format, ...);


#endif /* MAINLOGIC_PRINT_PRINT_H_ */
