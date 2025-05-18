#pragma once

#include <cstdint>
#include <functional>

#include "api/DS18B20.hpp"

class TemperatureReader
{
public:
    TemperatureReader();

    TemperatureReader(
        GPIO_TypeDef* GPIO_PIN_LETTER,
        uint16_t GPIO_PIN_ID,
        int tempMeasureDelayInMs
    );

    void update(int dtInMs);

    void setTempMeasureDelay(int delayInMs);

    int16_t getLastMeasuredTemperature() const;

    bool isWorking() const;

private:
    enum class State : uint8_t
    {
        NONE,
        START,
        MEASURING,
        READY
    };

    // void handleError(DS18B20_Status status);

private:
    uint16_t m_timer = 0;
    uint16_t m_delayTriggerTime = 0;

    State m_state = State::NONE;
    int16_t m_lastMeasuredTemperature = 25;

    DS18B20 m_sensor;    
};