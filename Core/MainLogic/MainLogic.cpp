/*
 * MainLogic.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#include "MainLogic.h"
#include "fn/fn.h"

MainLogic::MainLogic()
    : m_displayLine1("Current temp: 15c"),
      m_displayLine2("Desir   temp: 15c"),
      m_button(Button_GPIO_Port, Button_Pin),
      m_currModuleTemp(-1),
      m_potentiometerValueSwitchingReles(-1),
      m_potentiometerValueChangingTemp(-1)

{

    struct PinInfo
    {
        GPIO_TypeDef* letter;
        uint16_t pinId;

        PinInfo(GPIO_TypeDef* info, uint16_t id)
            : letter(info),
              pinId(id)
        {
        }
    };

    PinInfo tempReaderInfos[m_numSensors] = {
        { GPIOB, GPIO_PIN_12 },
        { GPIOB, GPIO_PIN_13 },
        { GPIOB, GPIO_PIN_14 },
        { GPIOB, GPIO_PIN_15 },
    };

    PinInfo blinekrInfos[m_numSensors] = {
        { GPIOB, GPIO_PIN_3 },
        { GPIOB, GPIO_PIN_4 },
        { GPIOB, GPIO_PIN_5 },
        { GPIOB, GPIO_PIN_8 },
    };

    for (uint8_t i = 0; i < m_numSensors; ++i)
    {
        m_releModules[i] = ReleModule(
            TemperatureReader(
                tempReaderInfos[i].letter,
                tempReaderInfos[i].pinId,
                2000
            ),
            Refrigerator(Blinker(blinekrInfos[i].letter, blinekrInfos[i].pinId))
        );
    }

    updateDisplay(m_releModules[m_currReleId]);

    Network::instance().init(
        [this](Network::MsgTypeReceive msgT, uint8_t* data, uint8_t size)
        {
            onUartMessage(msgT, data, size);
        }
    );

    m_button.setOnClick(
        nullptr,
        [this]
        {
            onButtonPressed();
        }
    );
}

MainLogic::~MainLogic()
{
}

void MainLogic::update(int dtInMs)
{
    for (int i = 0; i < m_numSensors; ++i)
    {
        m_releModules[i].update(dtInMs);
    }

    m_button.update(dtInMs);

    if (m_state == State::SWITCHING_RELE)
    {
        int16_t value = m_potentiometer.getValueMapped(0, m_numSensors - 1);

        if (value == -1)
        {
            error("potentiom failure");
            return;
        }

        if (m_potentiometerValueSwitchingReles.setValue(value))
        {
            m_currReleId = fn::fitInRange<uint8_t>(value, 0, m_numSensors - 1);

            updateDisplay(m_releModules[m_currReleId]);
        }

        bool changed = m_currModuleTemp.setValue(
            m_releModules[m_currReleId].getCurrentTemperature()
        );

        if (changed)
        {
            updateDisplay(m_releModules[m_currReleId]);
        }
    }
    else if (m_state == State::CHANGING_DESIRED_TEMPERATURE)
    {
        int16_t value = m_potentiometer.getValueMapped(
            ReleModule::constants::tempMin,
            ReleModule::constants::tempMax
        );

        if (value == -1)
        {
            error("potentiom failure");
            return;
        }

        if (m_potentiometerValueChangingTemp.setValue(value))
        {
            m_releModules[m_currReleId].setPendingTemp(value);

            updateDisplay(m_releModules[m_currReleId]);
        }
    }
}

void MainLogic::onUartMessage(
    Network::MsgTypeReceive type,
    uint8_t* data,
    uint8_t size
)
{
    uint8_t moduleId = data[0];

    if (!fn::isInRange(moduleId, 0, m_numSensors - 1))
    {
        Network::sendMessage(
            Network::MsgTypeSend::RESULT_FAIL,
            "rec module id out of range - %d",
            moduleId
        );

        return;
    }

    if (type == Network::MsgTypeReceive::SET_DESIRED_TEMPERATURE)
    {

        auto msg = fn::castToStruct<TemperatureMessage>(data + 1, size - 1);

        ReleModule& rele = m_releModules[moduleId];

        bool success = rele.setDesiredTemperature(msg.desiredTemp);

        auto msgT = success ? Network::MsgTypeSend::RESULT_OK
                            : Network::MsgTypeSend::RESULT_FAIL;

        Network::sendMessage(
            msgT,
            "setDesiredTemp: %d",
            rele.getDesiredTemperatire()
        );
    }
    else if (type == Network::MsgTypeReceive::GET_CURR_TEMPERATURE)
    {
        if (m_releModules[moduleId].isTempSensorWorking())
        {
            Network::sendMessage(
                Network::MsgTypeSend::RESULT_OK,
                "%d",
                m_releModules[moduleId].getCurrentTemperature()
            );
        }
        else
        {
            Network::sendMessage(
                Network::MsgTypeSend::RESULT_FAIL,
                "",
                "Temp sensor not working.."
            );
        }
    }
    else if (type == Network::MsgTypeReceive::GET_DESIRED_TEMPERATURE)
    {
        Network::sendMessage(
            Network::MsgTypeSend::RESULT_OK,
            "%d",
            m_releModules[moduleId].getDesiredTemperatire()
        );
    }
    else if (type == Network::MsgTypeReceive::KEEP_REFRIGERATOR_ON)
    {
        bool alwaysOn = true;
        m_releModules[moduleId].enableManualControl(alwaysOn);

        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");

        if (moduleId == m_currReleId)
        {
            updateDisplay(m_releModules[m_currReleId]);
        }
    }
    else if (type == Network::MsgTypeReceive::KEEP_REFRIGERATOR_OFF)
    {
        bool alwaysOn = false;
        m_releModules[moduleId].enableManualControl(alwaysOn);

        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");

        if (moduleId == m_currReleId)
        {
            updateDisplay(m_releModules[m_currReleId]);
        }
    }
    else if (type == Network::MsgTypeReceive::REFRIGERATOR_MANUAL_CONTROL_OFF)
    {
        m_releModules[moduleId].disableManualControl();

        Network::sendMessage(Network::MsgTypeSend::RESULT_OK, "");

        if (moduleId == m_currReleId)
        {
            updateDisplay(m_releModules[m_currReleId]);
        }
    }
    else if (type == Network::MsgTypeReceive::GET_IS_REFRIGERATOR_ON)
    {
        Network::sendMessage(
            Network::MsgTypeSend::RESULT_OK,
            "%d",
            m_releModules[moduleId].getRefrigerator().isOn()
        );
    }

    updateDisplay(m_releModules[moduleId]);
        
}

void MainLogic::onButtonPressed()
{
    // int16_t potentiometerValue = m_potentiometer.getValue();

    debug("onBtnPressed, state:%d", m_state);

    if (m_state == State::SWITCHING_RELE)
    {
        m_state = State::CHANGING_DESIRED_TEMPERATURE;
    }
    else if (m_state == State::CHANGING_DESIRED_TEMPERATURE)
    {
        m_releModules[m_currReleId].applyPendingTemp();

        m_state = State::SWITCHING_RELE;
    }
}

void MainLogic::updateDisplay(const ReleModule& info)
{
    debug("update display, releId: %d", m_currReleId);

    if (m_state == State::CHANGING_DESIRED_TEMPERATURE)
    {
        snprintf(
            &m_displayLine1[0],
            m_displayLine1.size(),
            "id%d t:%dc  on:%d",
            m_currReleId,
            info.getCurrentTemperature(),
            info.getRefrigerator().isOn()
        );
    }
    else
    {
        snprintf(
            &m_displayLine1[0],
            m_displayLine1.size(),
            "id%d t:%dc  chsn",
            m_currReleId,
            info.getCurrentTemperature()
        );
    }


    if (info.isDesiredTempApplied())
    {
        snprintf(
            &m_displayLine2[0],
            m_displayLine2.size(),
            "trg t:%dc        ",
            info.getDesiredTemperatire()
        );
    }
    else
    {
        snprintf(
            &m_displayLine2[0],
            m_displayLine2.size(),
            "trg t:%dc, *%dc ",
            info.getDesiredTemperatire(),
            info.getDesiredTemperatirePending()
        );
    }

    // Screen::clear();
    Screen::print(0, 0, m_displayLine1.c_str());
    Screen::print(0, 1, m_displayLine2.c_str());
}
