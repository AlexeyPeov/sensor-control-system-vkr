/*
 * MainLogic.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_MAINLOGIC_H_
#define MAINLOGIC_MAINLOGIC_H_

#include <cstdint>

#include "Button/Button.h"
#include "Network/Network.h"
#include "Potentiometer/Potentiometer.h"
#include "Refrigerator/Refrigerator.h"
#include "Screen/Screen.h"
#include "TemperatureReader/TemperatureReader.h"
#include "print/print.h"

#include "ReleModule/ReleModule.h"
#include "ValueChangeObserver/ValueChangeObserver.h"

class MainLogic
{
public:
    MainLogic();
    ~MainLogic();

    void update(int dtInMs);

private:
    enum class State
    {
        SWITCHING_RELE,
        CHANGING_DESIRED_TEMPERATURE
    };

private:
    void onUartMessage(
        Network::MsgTypeReceive type,
        uint8_t* data,
        uint8_t size
    );

    void updateDisplay(const ReleModule& curr);
    void onButtonPressed();

private:
    std::string m_displayLine1;
    std::string m_displayLine2;

    static constexpr uint8_t m_numSensors = 4;

    State m_state = State::SWITCHING_RELE;

    ReleModule m_releModules[m_numSensors];

    uint8_t m_currReleId = 0;

    Potentiometer m_potentiometer;
    Button m_button;

    ValueChangeObserver<int16_t> m_currModuleTemp;
    ValueChangeObserver<int16_t> m_potentiometerValueSwitchingReles;
    ValueChangeObserver<int16_t> m_potentiometerValueChangingTemp;
};

#endif /* MAINLOGIC_MAINLOGIC_H_ */
