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

    auto& refrigerator = Refrigerator::instance();

    int16_t desTemp = m_desiredTemperature
        + constants::refrigerantInitThresholdDeg;

    if(m_temperatureReader.isEmulatorMode() && refrigerator.isOn())
    {               
        static uint16_t timer = 0;

        timer += dtInMs;

        if(timer > 1000)
        {
            timer = 0;
            m_temperatureReader.decreaseEmulatedTemperatureValBy(1);
        }

    }

    if(!refrigerator.isManualControl())
    {
        if (m_currentTemperature > desTemp && !refrigerator.isOn())
        {        
            refrigerator.setOn();
            updateDisplay();
        }
    
        if (m_currentTemperature <= m_desiredTemperature && refrigerator.isOn())
        {
            refrigerator.setOff();
            updateDisplay();
        }
    }


    int16_t prevPotentiometerValue = m_potentiometerValueRead;

    m_potentiometerValueRead = m_potentiometer.getValueMapped(
        constants::tempMin,
        constants::tempMax
    );

    if (m_potentiometerValueRead == constants::tempMin - 1)
    {
        return;
    }

    if (prevPotentiometerValue == m_potentiometerValueRead)
    {
        return;
    }

    m_desiredTempChangeApplied = false;

    if (m_potentiometerValueRead == m_desiredTemperature)
    {
        m_desiredTempChangeApplied = true;
    }

    // m_desiredTemperature = m_potentiometerValueRead;
    debug("potentiometer value update prev: %d, curr: %d", prevPotentiometerValue, m_potentiometerValueRead);
    updateDisplay();
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
            "%d",
            m_currentTemperature
        );
    }
    else if (type == Network::MsgTypeReceive::GET_DESIRED_TEMPERATURE)
    {
        Network::sendMessage(
            Network::MsgTypeSend::DESIRED_TEMPERATURE,
            "%d",
            m_desiredTemperature
        );
    }
    else if (type == Network::MsgTypeReceive::KEEP_REFRIGERATOR_ON)
    {
        bool alwaysOn = true;
        Refrigerator::instance().enableManualControl(alwaysOn);        
        updateDisplay();
        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");
    }
    else if (type == Network::MsgTypeReceive::KEEP_REFRIGERATOR_OFF)
    {
        bool alwaysOn = false;
        Refrigerator::instance().enableManualControl(alwaysOn);
        updateDisplay();

        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");
    }
    else if (type == Network::MsgTypeReceive::REFRIGERATOR_MANUAL_CONTROL_OFF)
    {
        Refrigerator::instance().disableManualControl();

        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");
    }
    else if (type == Network::MsgTypeReceive::GET_IS_REFRIGERATOR_ON)
    {
        Network::sendMessage(
            Network::MsgTypeSend::IS_REFRIGERATOR_ON,
            "%d",
            Refrigerator::instance().isOn()
        );
    }
}

void MainLogic::onTemperatureMeasured(int16_t t)
{
    // debug("onTemp measured: %d", t);

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
    m_desiredTempChangeApplied = true;

    updateDisplay();

    return true;
}

void MainLogic::updateDisplay()
{
    debug("update display");

    snprintf(
        &m_displayLine1[0],
        m_displayLine1.size(),
        "cur t:%dc  on:%d",
        m_currentTemperature,
        Refrigerator::instance().isOn()
    );

    if (m_desiredTempChangeApplied)
    {
        snprintf(
            &m_displayLine2[0],
            m_displayLine2.size(),
            "trg t:%dc        ",
            m_desiredTemperature
        );
    }
    else
    {
        snprintf(
            &m_displayLine2[0],
            m_displayLine2.size(),
            "trg t:%dc, *%dc ",
            m_desiredTemperature,
            m_potentiometerValueRead
        );
    }

    // Screen::clear();
    Screen::print(0, 0, m_displayLine1.c_str());
    Screen::print(0, 1, m_displayLine2.c_str());
}
