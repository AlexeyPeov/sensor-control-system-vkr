#include "TemperatureReader.h"

#include "main.h"
#include "stm32f103xb.h"
#include "../fn/fn.h"
#include "../print/print.h"

#include "api/DS18B20.hpp"

static DS18B20 temp_sensor(TemperatureReader_GPIO_Port, TemperatureReader_Pin);

TemperatureReader::TemperatureReader(
    int tempMeasureDelayInMs,
    std::function<void(int16_t t)> onTempMeasuredCb
) : m_onTempMeasuredCb(std::move(onTempMeasuredCb))
{
    
    setTempMeasureDelay(tempMeasureDelayInMs);

    m_state = State::START;

    // debug("Temperature reader status: %d\r\n", status);

    // if(status == DS18B20_Status::DS18B20_OK)
    // {
    //     m_state = State::START;
    // }
    // else
    // {
    //     handleError(status);
    // }
}

void TemperatureReader::update(int dtInMs)
{


    if (m_state == State::NONE)
    {
        m_emulatedTimer += dtInMs;

        if(m_emulatedTimer >= m_delayTriggerTime)
        {
            m_emulatedTimer = 0;

            m_emulatedTemperature += 1;

            if(m_onTempMeasuredCb)
                m_onTempMeasuredCb(m_emulatedTemperature);
        }
    }
    else if (m_state == State::START)
    {
        // DS18B20_Status status = ds18b20_MeasureTemperCmd(SKIP_ROM, 0);

        // if(status != DS18B20_Status::DS18B20_OK)
        // {
        //     handleError(status);
        //     return;
        // }

        

        m_timer = 0;

        m_state = State::MEASURING;
    }
    else if (m_state == State::MEASURING)
    {
        m_timer += dtInMs;

        if (m_timer >= m_delayTriggerTime)
        {
            m_state = State::READY;
        }
    }
    else if (m_state == State::READY)
    {
        if(m_onTempMeasuredCb)
        {
            float temperature = temp_sensor.read_temp_celsius();

            m_onTempMeasuredCb(temperature);
        }

        m_state = State::START;
    }
}

void TemperatureReader::setTempMeasureDelay(int delayInMs)
{
    m_delayTriggerTime = fn::max(800, delayInMs);
}

void TemperatureReader::setOnTempMeasuredCb(
    std::function<void(int16_t t)> onTempMeasuredCb
)
{
    m_onTempMeasuredCb = std::move(onTempMeasuredCb);
}

bool TemperatureReader::isEmulatorMode() const { return m_state == State::NONE; }

void TemperatureReader::decreaseEmulatedTemperatureValBy(int16_t val) {
    m_emulatedTemperature = std::max(10,m_emulatedTemperature - val);
}

// void TemperatureReader::handleError(DS18B20_Status status) 
// {
//     m_timer = 0;
//     m_state = State::NONE;
//     error("TemperatureReader error %s, switch to emulated", toString(status).c_str());
// }
