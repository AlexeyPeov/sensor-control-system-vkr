#pragma once

#include <functional>
#include <utility>

#include "../print/print.h"

template <typename T>
class ValueChangeObserver
{
public:
    ValueChangeObserver() = default;

    ValueChangeObserver(T&& defaultValue)
        : m_currentValue(std::forward<T>(defaultValue))
    {
    }

    bool setValue(const T& value)
    {
        if (value != m_currentValue)
        {
            m_currentValue = value;
            return true;
        }

        return false;
    }
    const T& getValue() const
    {
        return m_currentValue;
    }

private:
    T m_currentValue;
};
