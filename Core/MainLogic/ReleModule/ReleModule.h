#pragma once

#include <cstdint>

#include "../Network/Network.h"
#include "../Refrigerator/Refrigerator.h"
#include "../TemperatureReader/TemperatureReader.h"
#include "../print/print.h"

class ReleModule
{
public:
    struct constants
    {
        static constexpr int16_t tempMin = 10;
        static constexpr int16_t tempMax = 35;
        static constexpr int16_t initialDesiredTemp = 25;
        static constexpr int16_t refrigerantInitThresholdDeg = 2;
        
    };
public:
    ReleModule() = default;

    ReleModule(TemperatureReader tmpReader, Refrigerator refrigerator);

    void update(int dtInMs);

    void setPendingTemp(int16_t desiredTemp);    

    bool applyPendingTemp();

    bool isDesiredTempApplied() const;    

    int16_t getCurrentTemperature() const;

    bool setDesiredTemperature(int16_t temp);

    int16_t getDesiredTemperatire() const;

    int16_t getDesiredTemperatirePending() const;
    
    // const TemperatureReader& getTemperatureReader() const;
    const Refrigerator& getRefrigerator() const;

    void enableManualControl(bool keepAlwaysOn);

    void disableManualControl();

private:

    void onTemperatureMeasured(int16_t t);

    void updateRefrigerator(bool useThreshold = true);

private:
    TemperatureReader m_tempReader;
    Refrigerator m_refrigerator;

    int16_t m_currTemp = -1;
    int16_t m_desTemp = 0;
    int16_t m_desTempPending = 0;


    bool m_desiredTempChangeApplied = false;
};