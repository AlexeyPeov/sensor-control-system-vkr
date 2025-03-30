#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <queue>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "main.h"

struct TemperatureMessage
{
    uint8_t desiredTemp = 15;
};

class Network
{
public:
    struct constants
    {
        static constexpr uint8_t bufferSize = 8;
    };

    // uart msg состоит из 8и байт, первый байт - тип сообщения.

    // SET_DESIRED_TEMPERATURE: 
    // 1й байт - id сообщения
    // 2й байт - id сенсора (enum SensorId)
    // 3й байт отвечает за желаемую температуру
    // пример выставления желаемой температуры 22c
    // [ 0x01, 0x01, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00 ]

    // другие сообщения, нули могут быть заменены чем угодно
    // // [ 0xmsgid, 0xsensorId, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ]
    enum class MsgTypeReceive : uint8_t
    {
        SET_DESIRED_TEMPERATURE             = 0x01,
        GET_CURR_TEMPERATURE                = 0x02,
        GET_DESIRED_TEMPERATURE             = 0x03,
        KEEP_REFRIGERATOR_ON                = 0x04,
        KEEP_REFRIGERATOR_OFF               = 0x05,
        REFRIGERATOR_MANUAL_CONTROL_OFF     = 0x06,
        GET_IS_REFRIGERATOR_ON              = 0x07,
    };

    enum class SensorId : uint8_t
    {
        SENSOR_1             = 0x00,
        SENSOR_2             = 0x01,
        SENSOR_3             = 0x02,
        SENSOR_4             = 0x03,
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
        std::function<void(MsgTypeReceive msg, uint8_t* data, uint8_t size)>
            onReceiveMsgCb
        = nullptr
    );

    void triggerReceiveMsgCb(MsgTypeReceive msg, uint8_t* data, uint8_t size);

    static void sendMessage(MsgTypeSend msgT, const char* format, ...);

private:
    Network();

private:
    uint8_t m_recBuffer[constants::bufferSize] { 0 };

    std::function<void(MsgTypeReceive msg, uint8_t* data, uint8_t size)>
        m_onReceiveMsgCb = nullptr;
};
