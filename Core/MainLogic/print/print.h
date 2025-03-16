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
#include <cstdint>
#include <stdarg.h>

#include "../Inc/main.h"

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
    #define debug(format, ...) print(format, ##__VA_ARGS__)
#else
    #define debug(format, ...) do {} while (0)
#endif

#define error(format, ...) print(format, ##__VA_ARGS__)

void print(const char *format, ...);


#endif /* MAINLOGIC_PRINT_PRINT_H_ */
