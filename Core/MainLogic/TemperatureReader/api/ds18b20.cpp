#include "DS18B20.hpp"
#include "../../fn/fn.h"
#include "../../print/print.h"

/*
Constructor for temperature sensor object
@param tim Pointer to hardware timer handle. The timer has to tick every microsecond!
@param port GPIO port of the sensor pin, e.g. GPIOB
@param pin GPIO pin number of the sensor pin
*/
DS18B20::DS18B20(GPIO_TypeDef *port, uint16_t pin) :  _port(port), _pin(pin)
{	
}

/*
Block for given time in microseconds by waiting for the htim ticks
*/
void DS18B20::delay_us(uint16_t us)
{
	fn::DelayMicro(us);
}

void DS18B20::set_data_pin(bool on)
{
	return HAL_GPIO_WritePin(_port, _pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void DS18B20::toggle_data_pin()
{
	return HAL_GPIO_TogglePin(_port, _pin);
}

GPIO_PinState DS18B20::read_data_pin()
{
	return HAL_GPIO_ReadPin(_port, _pin);
}

void DS18B20::set_pin_output()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = _pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	return HAL_GPIO_Init(_port, &GPIO_InitStruct);
}

void DS18B20::set_pin_input()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = _pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	return HAL_GPIO_Init(_port, &GPIO_InitStruct);
}

bool DS18B20::start_sensor()
{
    set_pin_output();
    set_data_pin(false);

    delay_us(480);
    set_pin_input();
    delay_us(80);

    // Check for presence pulse
    if (read_data_pin() == GPIO_PIN_SET) {
        error("DS18B20::start_sensor: sensor not detected\n");
        return false;
    }

    delay_us(400);
    return true;  
}


void DS18B20::writeData(uint8_t data)
{
	set_pin_output();

	for (uint8_t i = 0; i < 8; i++)
	{

		if (data & (1 << i))
		{
			set_pin_output();
			set_data_pin(false);
			delay_us(1);

			set_pin_input();
			delay_us(60);
			continue;
		}

		set_pin_output();
		set_data_pin(false);
		delay_us(60);

		set_pin_input();
	}
}

uint8_t DS18B20::read_data()
{
	uint8_t value = 0;
	set_pin_input();

	for (uint8_t i = 0; i < 8; i++)
	{
		set_pin_output();

		set_data_pin(false);
		delay_us(2);
		set_pin_input();

		if (read_data_pin())
		{
			value |= 1 << i;
		}

		delay_us(60);
	}
	return value;
}

/*
Read the current temperature from the sensor.
This functions blocks for around 800ms as it waits for the conversion time!
@return Temperature in degrees Celsius
*/
bool DS18B20::readCelciusBegin()
{
	if(!start_sensor())	
		return false;
	HAL_Delay(1);
	writeData(0xCC);
	writeData(0x44);
	
	return true;
}

std::optional<float> DS18B20::readCelciusEnd()
{
    if(!start_sensor())	
		return std::nullopt;
	writeData(0xCC);
	writeData(0xBE);

	uint8_t temp1 = read_data();
	uint8_t temp2 = read_data();

	uint16_t temp_com = (temp2 << 8) | temp1;

	float tempRet = (float)(temp_com / 16.0);

	if(!fn::isInRange(tempRet, 10, 50))
	{
		return std::nullopt;
	}

	return tempRet;
}

/*
Read the current temperature from the sensor.
This functions blocks for around 800ms as it waits for the conversion time!
@return Temperature in degrees Fahrenheit
*/
float DS18B20::read_temp_fahrenheit()
{
	return readCelciusBegin() * 1.8 + 32.0;
}