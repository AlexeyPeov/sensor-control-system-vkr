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

    Network::instance().init(
        [this](Network::MsgTypeReceive msgT, uint8_t* data, uint8_t size)
        { onUartMessage(msgT, data, size); }
    );

    m_button.setOnClick(nullptr, [this] { onButtonPressed(); });
}

MainLogic::~MainLogic() { }

void MainLogic::update(int dtInMs)
{
    m_temperatureReader.update(dtInMs);
    m_button.update(dtInMs);

    if (m_currentTemperature > m_desiredTemperature)
    {
        // start refrigerating
    }
}

void MainLogic::onUartMessage(
    Network::MsgTypeReceive type,
    uint8_t* data,
    uint8_t size
)
{
    if (type == Network::MsgTypeReceive::SET_DESIRED_TEMPERATURE)
    {

        auto msg = fn::castToStruct<TemperatureMessage>(data, size);

        bool success = setDesiredTemperature(msg.desiredTemp);

        auto msgT = success ? Network::MsgTypeSend::RESULT_OK
                            : Network::MsgTypeSend::RESULT_FAIL;

        Network::sendMessage(msgT, "setDesiredTemp: %d", m_desiredTemperature);
    }
    else if (type == Network::MsgTypeReceive::GET_CURR_TEMPERATURE)
    {
        Network::sendMessage(
            Network::MsgTypeSend::CURR_TEMPERATURE,
            "%d", m_currentTemperature
        );
    }
    else if (type == Network::MsgTypeReceive::GET_DESIRED_TEMPERATURE)
    {
        Network::sendMessage(
            Network::MsgTypeSend::DESIRED_TEMPERATURE,
            "%d", m_desiredTemperature
        );
    }
    else if (type == Network::MsgTypeReceive::SET_REFRIGERATOR_ON)
    {
        Network::sendMessage(
            Network::MsgTypeSend::RESULT_OK,
            ""
        );
    }
    else if (type == Network::MsgTypeReceive::SET_REFRIGERATOR_OFF)
    {
        Network::sendMessage(
            Network::MsgTypeSend::RESULT_OK,
            ""
        );
    }
    else if (type == Network::MsgTypeReceive::GET_IS_REFRIGERATOR_ON)
    {
        Network::sendMessage(
            Network::MsgTypeSend::IS_REFRIGERATOR_ON,
            "%d", false
        );
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

bool MainLogic::setDesiredTemperature(int16_t temp)
{
    if (m_desiredTemperature == temp)
    {
        return true;
    }

    if (!fn::isInRange(temp, constants::tempMin, constants::tempMax))
    {
        error(
            "setDesiredTemperature out of range %d-%d, rec: %d",
            constants::tempMin,
            constants::tempMax,
            temp
        );

        return false;
        // temp = fn::fitInRange(temp,constants::tempMin, constants::tempMax);
    }

    debug("set desired temp: %d", temp);

    m_desiredTemperature = temp;
    updateDisplay();

    return true;
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
