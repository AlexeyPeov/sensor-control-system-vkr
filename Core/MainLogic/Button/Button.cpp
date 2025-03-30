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
    m_btnState.setValue(buttonState(m_buttonPort, m_buttonPin));
}
