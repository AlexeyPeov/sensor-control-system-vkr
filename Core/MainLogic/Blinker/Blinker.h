/*
 * Blinker.h
 *
 *  Created on: Mar 8, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_BLINKER_BLINKER_H_
#define MAINLOGIC_BLINKER_BLINKER_H_

#include <cstdint>

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f103xb.h"

class Blinker {
public:
	Blinker();
	
	Blinker(GPIO_TypeDef* GPIO_PIN_LETTER, uint16_t GPIO_PIN_ID);

	~Blinker();

	void set(bool on = true);

	bool isOn() const;
	
private:
	GPIO_TypeDef* m_pinLetter=nullptr;
	uint16_t m_pin = 0;
	bool m_isOn = false;
};

#endif /* MAINLOGIC_BLINKER_BLINKER_H_ */
