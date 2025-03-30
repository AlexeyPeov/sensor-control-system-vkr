#include "Button.h"

#include "main.h"

#include "../print/print.h"

static GPIO_PinState buttonState(GPIO_TypeDef* buttonPort, uint16_t buttonPin)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(buttonPort, buttonPin);

    // debug("button state: %d", !state);

    if (state == GPIO_PinState::GPIO_PIN_RESET)
        return GPIO_PinState::GPIO_PIN_SET;
    return GPIO_PinState::GPIO_PIN_RESET;
}

Button::Button(GPIO_TypeDef* buttonPort, uint16_t m_buttonPin)
    : m_buttonPort(buttonPort), m_buttonPin(m_buttonPin)
{
}

Button::Button(
    GPIO_TypeDef* buttonPort,
    uint16_t buttonPin,
    std::function<void()> onPress,
    std::function<void()> onRelease
)
    : m_buttonPort(buttonPort), m_buttonPin(m_buttonPin),
      m_onPress(std::move(onPress)), m_onRelease(std::move(onRelease)),
      m_btnState(
          static_cast<GPIO_PinState>(3),
          [this](GPIO_PinState newState)
          {
            debug("btn state changed");

              if (newState == GPIO_PinState::GPIO_PIN_SET && m_onPress)
              {
                  HAL_Delay(10);

                  newState = buttonState(m_buttonPort, m_buttonPin);

                  if (newState == GPIO_PIN_SET)
                  {
                      m_onPress();
                  }
              }

              if (newState == GPIO_PinState::GPIO_PIN_RESET && m_onRelease)
              {
                  HAL_Delay(10);

                  newState = buttonState(m_buttonPort, m_buttonPin);

                  if (newState == GPIO_PIN_RESET)
                  {
                      m_onRelease();
                  }
              }
          }
      )
{
}

void Button::setOnClick(
    std::function<void()> onPress,
    std::function<void()> onRelease
)
{
    m_onPress = std::move(onPress);
    m_onRelease = std::move(onRelease);
}

void Button::update(int dtInMs)
{
    // struct ButtonWasUp
    // {
    // public:
    //     ButtonWasUp() : m_value(0xFFFFFFFF) { }

    //     bool operator[](int index) const
    //     {
    //         if (index < 0 || index >= 32)
    //             return false;

    //         return (m_value & (1 << index)) != 0;
    //     }

    //     void set(int index, bool state)
    //     {
    //         if (index < 0 || index >= 32)
    //             return;

    //         if (state)
    //             m_value |= (1 << index);
    //         else
    //             m_value &= ~(1 << index);
    //     }

    // private:
    //     uint32_t m_value;
    // };

    // static ButtonWasUp buttonWasUp;

    // определить момент «клика» несколько сложнее, чем факт того,
    // что кнопка сейчас просто нажата. Для определения клика мы
    // сначала понимаем, отпущена ли кнопка прямо сейчас...

    bool buttonIsUp = digitalRead(m_buttonPort, m_buttonPin);
    // ...если «кнопка была отпущена и (&&) не отпущена сейчас»...
    if (m_buttonWasUp && !buttonIsUp)
    {
        // ...может это «клик», а может и ложный сигнал (дребезг),
        // возникающий в момент замыкания/размыкания пластин кнопки,
        // поэтому даём кнопке полностью «успокоиться»...
        HAL_Delay(10);
        // ...и считываем сигнал снова
        buttonIsUp = digitalRead(m_buttonPort, m_buttonPin);

        // если она всё ещё нажата...
        if (!buttonIsUp)
        {
            // ...это клик!
            if (m_onPress != nullptr)
            {
                m_onPress();
            }
        }
    }
    else if (!m_buttonWasUp && buttonIsUp)
    {
        HAL_Delay(10);
        buttonIsUp = digitalRead(m_buttonPort, m_buttonPin);

        if (buttonIsUp && m_onRelease != nullptr)
        {
            m_onRelease();
        }
    }

    // запоминаем последнее состояние кнопки для новой итерации
    m_buttonWasUp = buttonIsUp;    
}
