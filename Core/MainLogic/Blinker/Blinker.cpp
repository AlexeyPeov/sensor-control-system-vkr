/*
 * Blinker.cpp
 *
 *  Created on: Mar 8, 2025
 *      Author: alex
 */

#include "Blinker.h"
#include "../print/print.h"

Blinker::Blinker()
{
}

Blinker::Blinker(GPIO_TypeDef* GPIO_PIN_LETTER, uint16_t GPIO_PIN_ID) :
    m_pinLetter(GPIO_PIN_LETTER), m_pin(GPIO_PIN_ID), m_isOn(false)
{
    HAL_GPIO_WritePin(
        GPIO_PIN_LETTER,
        GPIO_PIN_ID,
        GPIO_PinState::GPIO_PIN_RESET
    );
}

void Blinker::set(bool on)
{
    if (m_pin == -1)
    {
        error("blinker pin -1");
        return;
    }

    m_isOn = on;

    GPIO_PinState state = on ? GPIO_PIN_SET : GPIO_PIN_RESET;

    HAL_GPIO_WritePin(m_pinLetter, m_pin, state);
}

bool Blinker::isOn() const
{
    return m_isOn;
}

Blinker::~Blinker()
{
    // TODO Auto-generated destructor stub
}
