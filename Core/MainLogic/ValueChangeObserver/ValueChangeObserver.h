#pragma once

#include <functional>
#include <utility>

#include "../print/print.h"

template <typename T> class ValueChangeObserver
{
public:
    ValueChangeObserver() = default;

    template <typename U>
    ValueChangeObserver(
        U&& defaultValue,
        std::function<void(const T&)> cbOnChange
    )
        : m_prevValue(std::forward<U>(defaultValue)),
          m_currentValue(m_prevValue), m_cbOnChange(std::move(cbOnChange))
    {
    }

    template <typename U> void setValue(U&& value)
    {
        if (value != m_currentValue)
        {
            m_prevValue = std::move(m_currentValue);
            m_currentValue = std::forward<U>(value);

            if (m_cbOnChange)
            {
                m_cbOnChange(m_currentValue);
            }
        }
    }

    const T& getPrevValue() const
    {
        return m_prevValue;
    }

    const T& getCurrValue() const
    {
        return m_currentValue;
    }

private:
    T m_prevValue;
    T m_currentValue;
    std::function<void(const T&)> m_cbOnChange = nullptr;
};
