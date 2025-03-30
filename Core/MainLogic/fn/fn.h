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
        return (val >= a && val <= b);
    }
    

    template<typename T>
    T fitInRange(T val, T min, T max)
    {
        if(val < min)   return min;
        if(val > max)   return max;

        return val;
    }

    template <typename Struct>
    Struct castToStruct(uint8_t* data, uint8_t size)
    {
        Struct msg;
        memcpy(&msg, data, sizeof(Struct));
        return msg;
    }

    // template <typename T>
    // T mapValue(const T& value, const T& min, const T& max)
    // {
    //     float v = value;
    //     float m = min;
    //     float maxF = max+1;
    //     return m + ((v * (maxF - m)) / maxF);
    // }
}
