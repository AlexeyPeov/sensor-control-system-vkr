#ifndef DS18B20_H_
#define DS18B20_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

constexpr uint8_t SKIP_ROM = 0;
constexpr uint8_t NO_SKIP_ROM = 1;

constexpr uint8_t RESOLUTION_9BIT = 0x1F;
constexpr uint8_t RESOLUTION_10BIT = 0x3F;
constexpr uint8_t RESOLUTION_11BIT = 0x5F;
constexpr uint8_t RESOLUTION_12BIT = 0x7F;

void __temperature_reader_port_b11_init(void);
uint8_t ds18b20_init(uint8_t mode);
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum);
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum);
uint8_t ds18b20_GetSign(uint16_t dt);
float ds18b20_Convert(uint16_t dt);

#endif /* DS18B20_H_ */
