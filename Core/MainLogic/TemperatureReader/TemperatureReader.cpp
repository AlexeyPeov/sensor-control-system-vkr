#include "TemperatureReader.h"

#include "../fn/fn.h"
#include "../print/print.h"
#include "main.h"
#include "stm32f103xb.h"

// static DS18B20 tempSensor(TemperatureReader_GPIO_Port,
// TemperatureReader_Pin);

TemperatureReader::TemperatureReader() : m_sensor(nullptr, 0)
{
}

TemperatureReader::TemperatureReader(
    GPIO_TypeDef* GPIO_PIN_LETTER,
    uint16_t GPIO_PIN_ID,
    int tempMeasureDelayInMs
)
    : m_sensor(GPIO_PIN_LETTER, GPIO_PIN_ID)
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

        if (m_timer >= m_delayTriggerTime)
        {
            m_timer = 0;
            m_state = State::START;
        }
    }
    else if (m_state == State::START)
    {
        bool success = m_sensor.readCelciusBegin();

        if (!success)
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

        std::optional<float> temp = m_sensor.readCelciusEnd();

        if (temp)
        {
            
            m_lastMeasuredTemperature = *temp;
        }
        else
        {
            m_state = State::NONE;
            return;
        }

        m_state = State::START;
    }
}

void TemperatureReader::setTempMeasureDelay(int delayInMs)
{
    m_delayTriggerTime = fn::max(800, delayInMs);
}


float TemperatureReader::getLastMeasuredTemperature() const
{
    return m_lastMeasuredTemperature;
}

bool TemperatureReader::isWorking() const
{
    return m_state != State::NONE;
}
