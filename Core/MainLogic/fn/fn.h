#pragma once

#include <cstdint>

namespace fn
{
    void DelayMicro(uint32_t us);
    
    inline int max(int a, int b)
    {
        if(a > b) return a;
        return b;
    }
}
