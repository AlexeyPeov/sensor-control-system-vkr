#include "ReleModule.h"
#include "../fn/fn.h"

ReleModule::ReleModule(TemperatureReader tmpReader, Refrigerator refrigerator)
    : m_tempReader(std::move(tmpReader)),
      m_refrigerator(std::move(refrigerator))
{
    // m_tempReader.setOnTempMeasuredCb(
    //     [this](int16_t t)
    //     {
    //          onTemperatureMeasured(t);   
    //     }
    // );

    setDesiredTemperature(constants::initialDesiredTemp);
}

void ReleModule::update(int dtInMs)
{    
    m_tempReader.update(dtInMs);
    m_refrigerator.update(dtInMs);

    updateRefrigeratorState();
    
    m_currTemp = m_tempReader.getLastMeasuredTemperature();
}

void ReleModule::setPendingTemp(int16_t desiredTemp)
{
    int16_t prevPotentiometerValue = m_desTempPending;

    m_desTempPending = desiredTemp;

    if (prevPotentiometerValue == m_desTempPending)
    {
        return;
    }

    m_desiredTempChangeApplied = false;

    if (m_desTempPending == m_desiredTemp)
    {
        m_desiredTempChangeApplied = true;
    }
}

bool ReleModule::applyPendingTemp()
{
    return setDesiredTemperature(m_desTempPending);
}

bool ReleModule::setDesiredTemperature(int16_t temp)
{
    if (m_desiredTemp == temp)
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

    info("set desired temp: %d", temp);

    m_desiredTemp = temp;
    m_desiredTempChangeApplied = true;

    // updateDisplay();
    updateRefrigeratorState(false);

    return true;
}

int16_t ReleModule::getCurrentTemperature() const
{
    return m_currTemp;
}

bool ReleModule::isTempSensorWorking() const
{
    return m_tempReader.isWorking();
}

int16_t ReleModule::getDesiredTemperatire() const
{
    return m_desiredTemp;
}

int16_t ReleModule::getDesiredTemperatirePending() const
{
    return m_desTempPending;
}

const Refrigerator& ReleModule::getRefrigerator() const
{
    return m_refrigerator;
}

void ReleModule::enableManualControl(bool keepAlwaysOn)
{
    m_refrigerator.enableManualControl(keepAlwaysOn);
}

void ReleModule::disableManualControl()
{
    m_refrigerator.disableManualControl();
}

bool ReleModule::isDesiredTempApplied() const
{
    return m_desiredTempChangeApplied;
}

void ReleModule::onTemperatureMeasured(int16_t t)
{
    debug("onTemp measured: %d", t);
    m_currTemp = t;
}

void ReleModule::updateRefrigeratorState(bool useThreshold)
{
    if (m_refrigerator.isManualControl())
    {
        return;
    }

    int16_t desTemp = m_desiredTemp;

    if (useThreshold)
    {
        desTemp += constants::refrigerantInitThresholdDeg;
    }

    if (m_currTemp > desTemp && !m_refrigerator.isOn())
    {
        m_refrigerator.setOn();
    }
    else if (m_currTemp < m_desiredTemp && m_refrigerator.isOn())
    {
        m_refrigerator.setOff();
    }
}