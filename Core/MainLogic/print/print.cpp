/*
 * print.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */
#include "print.h"

extern UART_HandleTypeDef huart1;

void print(const char* format, ...)
{
    constexpr size_t bufferSize = 128; // Define a buffer size
    char buffer[bufferSize];

    // Initialize variable argument list
    va_list args;
    va_start(args, format);

    // Format the input string
    vsnprintf(buffer, bufferSize, format, args);
    va_end(args);

    // Add a newline directly to the buffer
    size_t len = strlen(buffer);
    if (len < bufferSize - 1)
    { // Ensure space for the newline and null terminator
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
    }

    // Use the buffer directly for UART transmission
    HAL_UART_Transmit(
        &huart1,
        reinterpret_cast<uint8_t*>(buffer),
        strlen(buffer),
        HAL_MAX_DELAY
    );

    memset(buffer, 0, bufferSize);
}
