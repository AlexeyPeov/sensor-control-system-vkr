#include "TemperatureReader.h"

#include "api/DS18B20.h"

#include "../fn/fn.h"
#include "../print/print.h"

TemperatureReader::TemperatureReader(
    int tempMeasureDelayInMs,
    std::function<void(int16_t t)> onTempMeasuredCb
) : m_onTempMeasuredCb(std::move(onTempMeasuredCb))
{
    __temperature_reader_port_b11_init();

    uint8_t status = ds18b20_init(SKIP_ROM);

    setTempMeasureDelay(tempMeasureDelayInMs);

    debug("Temperature reader status: %d\r\n", status);

    if(status == 0)
    {
        m_state = State::START;
    }
}

void TemperatureReader::update(int dtInMs)
{
    if (m_state == State::START)
    {
        ds18b20_MeasureTemperCmd(SKIP_ROM, 0);

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
        uint8_t m_buffer[8] { 0 };

        ds18b20_ReadStratcpad(SKIP_ROM, m_buffer, 0);

        auto val = static_cast<uint16_t>(m_buffer[1]);

        uint16_t rawT = (val << 8) | m_buffer[0];

        float t = ds18b20_Convert(rawT);

        if (m_onTempMeasuredCb)
        {
            m_onTempMeasuredCb(static_cast<int16_t>(t));
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
