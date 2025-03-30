#pragma once

#include <cstdint>
#include <functional>
#include "../ValueChangeObserver/ValueChangeObserver.h"

#include "stm32f103xb.h"

class Button
{
public:

    Button(
        GPIO_TypeDef* buttonPort,
        uint16_t buttonPin
    );

    Button(
        GPIO_TypeDef* buttonPort,
        uint16_t buttonPin,
        std::function<void()> onPress,
        std::function<void()> onRelease
    );

    void setOnClick(
        std::function<void()> onPress = nullptr, 
        std::function<void()> onRelease = nullptr
    );


    void update(int dtInMs);

private:
    GPIO_TypeDef* m_buttonPort = nullptr;
    uint16_t m_buttonPin = 0;
    std::function<void()> m_onPress = nullptr;
    std::function<void()> m_onRelease = nullptr;
    ValueChangeObserver<GPIO_PinState> m_btnState;

};