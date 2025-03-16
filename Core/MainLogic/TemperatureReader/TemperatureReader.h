#pragma once

#include <cstdint>
#include <functional>

#include "api/DS18B20.h"

class TemperatureReader
{
public:

    TemperatureReader(
        int tempMeasureDelayInMs,
        std::function<void(int16_t t)> onTempMeasuredCb
    );

    void update(int dtInMs);

    void setTempMeasureDelay(int delayInMs);

    void setOnTempMeasuredCb(std::function<void(int16_t t)> onTempMeasuredCb);

private:
    enum class State : uint8_t
    {
        NONE,
        START,
        MEASURING,
        READY
    };

    void handleError(DS18B20_Status status);

private:
    int m_timer = 0;
    int m_delayTriggerTime = 0;
    State m_state = State::NONE;
    std::function<void(int16_t temperature)> m_onTempMeasuredCb = nullptr;
};