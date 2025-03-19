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

    bool isEmulatorMode() const;

    void decreaseEmulatedTemperatureValBy(int16_t val);

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
    uint16_t m_timer = 0;
    uint16_t m_delayTriggerTime = 0;

    State m_state = State::NONE;
    int16_t m_emulatedTemperature = 25;
    int16_t m_emulatedTimer = 0;

    std::function<void(int16_t temperature)> m_onTempMeasuredCb = nullptr;
};