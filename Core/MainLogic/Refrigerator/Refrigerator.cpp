#include "Refrigerator.h"
#include <utility>
Refrigerator::Refrigerator(Blinker blinker) : m_blinker(std::move(blinker))
{

}

bool Refrigerator::isOn() const
{
    return m_blinker.isOn();
}

void Refrigerator::setOn()
{
    if (!m_isManual)
        m_blinker.set(true);
}

void Refrigerator::setOff()
{
    if (!m_isManual)
        m_blinker.set(false);

}

void Refrigerator::enableManualControl(bool alwaysOn)
{
    m_isManual = true;

    m_blinker.set(alwaysOn);
}

void Refrigerator::disableManualControl() { m_isManual = false; }

bool Refrigerator::isManualControl() const { return m_isManual; }

void Refrigerator::update(int dtInMs) { }
