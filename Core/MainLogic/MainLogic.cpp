/*
 * MainLogic.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#include "MainLogic.h"
#include "fn/fn.h"

MainLogic::MainLogic() :
    m_displayLine1("Current temp: 15c"), m_displayLine2("Desir   temp: 15c"),
    m_temperatureReader(2000, [this](int16_t t) { onTemperatureMeasured(t); }),
    m_button(Button_GPIO_Port, Button_Pin)
{

    setDesiredTemperature(constants::initialDesiredTemp);

    updateDisplay();

    Network::instance().init([this](Network::TemperatureMessage msg){ 
        setDesiredTemperature(msg.desiredTemp); 
    });

    m_button.setOnClick(nullptr, [this] { onButtonPressed(); });
}

MainLogic::~MainLogic() { }

void MainLogic::update(int dtInMs)
{
    m_temperatureReader.update(dtInMs);
    m_button.update(dtInMs);

    if(m_currentTemperature > m_desiredTemperature)
    {
        // start refrigerating
    }
}

void MainLogic::onTemperatureMeasured(int16_t t)
{
    if (m_currentTemperature != t)
    {
        m_currentTemperature = t;

        updateDisplay();
    }
}

void MainLogic::onButtonPressed()
{
    int16_t potentiometerValue = m_potentiometer.getValueMapped(
        constants::tempMin,
        constants::tempMax
    );

    if (potentiometerValue == constants::tempMin - 1)
    {
        const char* errorV = "Err potentiom";

        Screen::print(0, 0, errorV);

        error(errorV);

        return;
    }

    setDesiredTemperature(potentiometerValue);
}

void MainLogic::setDesiredTemperature(int16_t temp)
{
    if (m_desiredTemperature == temp)
    {
        return;
    }

    if (!fn::isInRange(temp, constants::tempMin, constants::tempMax))
    {
        error(
            "setDesiredTemperature out of range %d-%d, rec: %d",
            constants::tempMin,
            constants::tempMax,
            temp
        );

        return;
        // temp = fn::fitInRange(temp,constants::tempMin, constants::tempMax);
    }

    debug("set desired temp: %d", temp);

    m_desiredTemperature = temp;
    updateDisplay();
}

void MainLogic::updateDisplay()
{
    snprintf(
        &m_displayLine1[0],
        m_displayLine1.size(),
        "Current t: %dc",
        m_currentTemperature
    );

    snprintf(
        &m_displayLine2[0],
        m_displayLine2.size(),
        "Target  t: %dc",
        m_desiredTemperature
    );

    Screen::clear();
    Screen::print(0, 0, m_displayLine1.c_str());
    Screen::print(0, 1, m_displayLine2.c_str());
}
