#include "Network.h"
#include "../print/print.h"

#include "stm32f103xb.h"
#include "stm32f1xx_hal_uart.h"

extern UART_HandleTypeDef huart1;

static uint8_t* recBufferPtr = nullptr;

Network::Network() { }

const std::string& Network::msgToString(MsgTypeSend msg)
{
    static const std::string messages[6]
    {
        "[DEBUG] ",
        "[INFO] ",
        "[WARN]",
        "[ERROR]",
        "[OK]",
        "[FAIL]",
    };

    int i = static_cast<int>(msg);

    if(i >= 6)
    {
        error("Network::msgToString out of bounds, msgId: %d", msg);
        return messages[5];
    }

    return messages[i];
}

Network& Network::instance()
{
    static Network network;
    return network;
}

void Network::init(
    std::function<void(MsgTypeReceive type, uint8_t* data, uint8_t size)> onMsgCb
)
{
    debug("Init Network..");

    m_onReceiveMsgCb = std::move(onMsgCb);

    HAL_StatusTypeDef status = HAL_UART_Receive_IT(
        &huart1,
        m_recBuffer,
        constants::bufferSize
    );

    if (status != HAL_OK)
    {
        debug("HAL_UART_Receive_IT error.. %d", status);
    }

    recBufferPtr = m_recBuffer;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    // debug("call HAL_UART_RxCpltCallback..");

    if (huart->Instance == USART1)
    {

        Network::instance().triggerReceiveMsgCb(
            static_cast<Network::MsgTypeReceive>(recBufferPtr[0]),
            recBufferPtr+1,
            Network::constants::bufferSize-1
        );

        HAL_UART_Receive_IT(
            &huart1,
            recBufferPtr,
            Network::constants::bufferSize
        );
    }
}

std::string toString(Network::MsgTypeReceive t)
{
    switch (t)
    {
    case Network::MsgTypeReceive::SET_DESIRED_TEMPERATURE:
        return "SET_DESIRED_TEMPERATURE";
    case Network::MsgTypeReceive::GET_CURR_TEMPERATURE:
        return "GET_CURR_TEMPERATURE";
    case Network::MsgTypeReceive::GET_DESIRED_TEMPERATURE:
        return "GET_DESIRED_TEMPERATURE";
    case Network::MsgTypeReceive::KEEP_REFRIGERATOR_ON:
        return "KEEP_REFRIGERATOR_ON";
    case Network::MsgTypeReceive::KEEP_REFRIGERATOR_OFF:
        return "KEEP_REFRIGERATOR_OFF";
    case Network::MsgTypeReceive::REFRIGERATOR_MANUAL_CONTROL_OFF:
        return "REFRIGERATOR_MANUAL_CONTROL_OFF";
    case Network::MsgTypeReceive::GET_IS_REFRIGERATOR_ON:
        return "GET_IS_REFRIGERATOR_ON";
    }

    return "unknown";
}

void Network::triggerReceiveMsgCb(MsgTypeReceive msg, uint8_t* data, uint8_t size)
{
    debug("rec msg: %s", toString(msg).c_str());

    if (m_onReceiveMsgCb)
    {
        m_onReceiveMsgCb(msg, data, size);
    }
}

void Network::sendMessage(MsgTypeSend msgT, const char* format, ...)
{
    constexpr size_t bufferSize = 128;
    char buffer[bufferSize] = {0};

    va_list args;
    va_start(args, format);

    const std::string& msgHeader = msgToString(msgT);

    vsnprintf(buffer + msgHeader.size(), bufferSize - 2, format, args);
    va_end(args);

    memcpy(buffer,msgHeader.c_str(),msgHeader.size());

    buffer[bufferSize-1] = '\0';

    size_t len = strlen(buffer);
    
    if (len < bufferSize - 1)
    {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';        
    }

    HAL_UART_Transmit(
        &huart1, /*UART_HandleTypeDef*/
        reinterpret_cast<uint8_t*>(buffer),
        strlen(buffer),
        HAL_MAX_DELAY
    );

    memset(buffer, 0, bufferSize);
}
