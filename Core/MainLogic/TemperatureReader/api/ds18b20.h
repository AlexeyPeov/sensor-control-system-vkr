#ifndef DS18B20_H_
#define DS18B20_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

constexpr uint8_t SKIP_ROM = 0;
constexpr uint8_t NO_SKIP_ROM = 1;

constexpr uint8_t RESOLUTION_9BIT = 0x1F;
constexpr uint8_t RESOLUTION_10BIT = 0x3F;
constexpr uint8_t RESOLUTION_11BIT = 0x5F;
constexpr uint8_t RESOLUTION_12BIT = 0x7F;

enum class DS18B20_Status
{
    DS18B20_OK = 0,
    DS18B20_ERROR_RESET = 1,
    DS18B20_ERROR_COMMUNICATION = 2,
    DS18B20_ERROR_TIMEOUT = 3
};

void __temperature_reader_port_b11_init(void);

DS18B20_Status ds18b20_init(uint8_t mode);

DS18B20_Status ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum);

DS18B20_Status ds18b20_ReadStratcpad(
    uint8_t mode, 
    uint8_t* Data, 
    uint8_t DevNum
);

uint8_t ds18b20_GetSign(uint16_t dt);

float ds18b20_Convert(uint16_t dt);

std::string toString(DS18B20_Status status);

#endif /* DS18B20_H_ */
