/*
 * MainLogic.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#include "MainLogic.h"

MainLogic::MainLogic() :
    m_displayLine1("Current temp: 15c"), 
    m_displayLine2("Desir   temp: 15c"),
    m_temperatureReader(2000, [this](int16_t t){ onTemperatureMeasured(t); })
{    

    setDesiredTemperature(15);

    updateDisplay();

    Network::instance().init(
        [this](Network::TemperatureMessage msg)
        {
            setDesiredTemperature(msg.desiredTemp);
        }
    );
}

MainLogic::~MainLogic()
{

}

void MainLogic::update(int dtInMs)
{
    m_temperatureReader.update(dtInMs);
}

void MainLogic::onTemperatureMeasured(int16_t t) 
{
    if (m_currentTemperature != t)
    {
        m_currentTemperature = t;
        
        updateDisplay();
    }
}

void MainLogic::setDesiredTemperature(int16_t temp)
{
    if(m_desiredTemperature == temp)
    {
        return;
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
