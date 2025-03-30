#pragma once

#include <cstdint>

#include "../Blinker/Blinker.h"

class Refrigerator
{
public:
    Refrigerator() = default;

    Refrigerator(Blinker blinker);

    bool isOn() const;
    
    void setOn();
    
    void setOff();

    void enableManualControl(bool alwaysOn);
    void disableManualControl();
    bool isManualControl() const;
    
    void update(int dtInMs);

private:
    uint16_t m_timer = 0;
    bool m_isManual = false;
    Blinker m_blinker;

};
