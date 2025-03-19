#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <queue>
#include <array>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

struct TemperatureMessage
{
    int16_t desiredTemp = 15;
};

class Network
{
public:
    struct constants
    {
        static constexpr uint8_t bufferSize = 8;
    };
    
    enum class MsgTypeReceive : uint8_t
    {
        SET_DESIRED_TEMPERATURE = 0x01,
        GET_CURR_TEMPERATURE,
        GET_DESIRED_TEMPERATURE,
        KEEP_REFRIGERATOR_ON,
        KEEP_REFRIGERATOR_OFF,
        REFRIGERATOR_MANUAL_CONTROL_OFF,
        GET_IS_REFRIGERATOR_ON,
    };

    enum class MsgTypeSend : uint8_t
    {
        LOG_DEBUG = '0',
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        RESULT_OK = 'T',
        RESULT_FAIL = 'F',

        CURR_TEMPERATURE = 'C',
        DESIRED_TEMPERATURE = 'D',
        IS_REFRIGERATOR_ON = 'R',
    };
    
    static Network& instance();
    
    void init(
        std::function<void(
            MsgTypeReceive msg, 
            uint8_t* data, 
            uint8_t size
        )> onReceiveMsgCb = nullptr
    );
    
    void triggerReceiveMsgCb(MsgTypeReceive msg, uint8_t* data, uint8_t size);

    static void sendMessage(MsgTypeSend msgT, const char* format, ...);

private:
    Network();

private:
    uint8_t m_recBuffer[constants::bufferSize]{0};    
    
    std::function<void(
        MsgTypeReceive msg, 
        uint8_t* data, 
        uint8_t size
    )> m_onReceiveMsgCb = nullptr;    
};
