#include "Network.h"

#include "../../Drivers\CMSIS\Device\ST\STM32F1xx\Include\stm32f103xb.h"
#include "../../Drivers\STM32F1xx_HAL_Driver\Inc\stm32f1xx_hal_uart.h"

extern UART_HandleTypeDef huart1;

static uint8_t* recBufferPtr = nullptr;

Network::Network() { }

Network& Network::instance()
{
    static Network network;
    return network;
}

void Network::init(std::function<void(TemperatureMessage)> onTempMsgCb)
{
    print("Init Network..");

    m_onTemperatureCb = std::move(onTempMsgCb);

    HAL_StatusTypeDef status = HAL_UART_Receive_IT(
        &huart1,
        m_recBuffer,
        constants::bufferSize
    );

    if (status != HAL_OK)
    {
        print("HAL_UART_Receive_IT error.. %d", status);
    }

    recBufferPtr = m_recBuffer;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    print("call HAL_UART_RxCpltCallback..");

    if (huart->Instance == USART1)
    {

        Network::instance().receiveMessage(
            static_cast<Network::MsgType>(recBufferPtr[0]),
            recBufferPtr,
            Network::constants::bufferSize
        );

        HAL_UART_Receive_IT(
            &huart1,
            recBufferPtr,
            Network::constants::bufferSize
        );
    }
}

std::string toString(Network::MsgType t)
{
    if (t == Network::MsgType::SET_TEMPERATURE)
    {
        return "set temperature";
    }

    return "";
}

void Network::receiveMessage(MsgType msg, uint8_t* data, uint8_t size)
{
    print("rec msg: %s", toString(msg).c_str());

    if (msg == MsgType::SET_TEMPERATURE)
    {
        TemperatureMessage msg = receiveTcpMsgStruct<TemperatureMessage>(
            m_recBuffer,
            constants::bufferSize
        );

        if (m_onTemperatureCb)
        {
            m_onTemperatureCb(msg);
        }
    }
}

template <typename Struct>
inline Struct Network::receiveTcpMsgStruct(uint8_t* data, uint8_t size)
{
    Struct msg;
    memcpy(&msg, data + 1, sizeof(Struct));
    return msg;
}
