#ifndef DS18B20_HPP
#define DS18B20_HPP

#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"
#include <stdint.h>

class DS18B20
{
public:
	DS18B20(GPIO_TypeDef *port, uint16_t pin);

	float read_temp_celsius();
	float read_temp_fahrenheit();

private:
	void set_data_pin(bool on);
	void toggle_data_pin();

	void set_pin_output();
	void set_pin_input();

	GPIO_PinState read_data_pin();

	void start_sensor();

	void writeData(uint8_t data);
	uint8_t read_data();

	void delay_us(uint16_t us);

	GPIO_TypeDef *_port;
	uint16_t _pin;
};

#endif // DS18B20_HPP
