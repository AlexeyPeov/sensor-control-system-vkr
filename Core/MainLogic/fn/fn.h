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

    inline bool isInRange(int val, int a, int b)
    {
        if(val > a || val < b)
        {
            return false;
        }
        
        return true;
    }

    template<typename T>
    T fitInRange(T val, T min, T max)
    {
        if(val < min)   return min;
        if(val > max)   return max;

        return val;
    }
}
