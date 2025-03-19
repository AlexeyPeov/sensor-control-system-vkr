#pragma once

#include <cstdint>

class Refrigerator
{
public:
    static Refrigerator& instance();

    bool isOn() const;
    
    void setOn();
    
    void setOff();

    void enableManualControl(bool alwaysOn);
    void disableManualControl();
    bool isManualControl() const;
    
    void update(int dtInMs);
    
private:
    Refrigerator();

private:
    uint16_t m_timer = 0;
    bool m_isOn = false;
    bool m_isManual = false;
};
