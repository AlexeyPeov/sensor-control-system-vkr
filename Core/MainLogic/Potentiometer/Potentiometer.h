#pragma once

#include <cstdint>

#include "main.h"

#include "../print/print.h"

class Potentiometer
{
public:
    Potentiometer();
    
    ///
    /// @return read value mapped between min and max. example: from 0 to 100
    ///  min - including;
    ///  max - including;
    /// on error returns min - 1
    int16_t getValueMapped(int min, int max);

    
};