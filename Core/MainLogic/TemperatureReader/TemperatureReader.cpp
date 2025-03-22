#include "TemperatureReader.h"

#include "main.h"
#include "stm32f103xb.h"
#include "../fn/fn.h"
#include "../print/print.h"

#include "api/DS18B20.hpp"

static DS18B20 tempSensor(TemperatureReader_GPIO_Port, TemperatureReader_Pin);

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
        m_timer += dtInMs;

        if(m_timer >= m_delayTriggerTime)
        {
            m_timer = 0;
            m_state = State::START;
        }
    }
    else if (m_state == State::START)
    {
        bool success = tempSensor.readCelciusBegin();

        if(!success)
        {
            m_state = State::NONE;
            return;
        }

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
            // std::optional<float> temp = tempSensor.readCelciusEnd();

            std::optional<float> temp = tempSensor.readCelciusEnd();

            if(temp)
            {
                m_onTempMeasuredCb(*temp);
                m_lastMeasuredTemperature = *temp;
            }
            else
            {
                m_state = State::NONE;
                return;
            }

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

