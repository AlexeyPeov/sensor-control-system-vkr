#include "Refrigerator.h"

Refrigerator& Refrigerator::instance()
{
    static Refrigerator instance;
    return instance;
}

bool Refrigerator::isOn() const { return m_isOn; }

void Refrigerator::setOn()
{
    if (!m_isManual)
        m_isOn = true;
}

void Refrigerator::setOff()
{
    if (!m_isManual)
        m_isOn = false;
}

void Refrigerator::enableManualControl(bool alwaysOn)
{
    m_isManual = true;
    m_isOn = alwaysOn;
}

void Refrigerator::disableManualControl() { m_isManual = false; }

bool Refrigerator::isManualControl() const { return m_isManual; }

void Refrigerator::update(int dtInMs) { }

Refrigerator::Refrigerator() { }
