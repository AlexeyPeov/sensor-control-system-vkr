#pragma once

#include <cstdint>
#include <functional>

#include "../print/print.h"

class Network
{
    public:
    struct constants
    {
        static constexpr uint8_t bufferSize = 2;
    };
    
    enum class MsgType
    {
        SET_TEMPERATURE,
    };
    
    struct TemperatureMessage
    {
        int16_t desiredTemp = 15;
    };
    
    static Network& instance();
    
    void init(std::function<void(TemperatureMessage)> onTempMsgCb = nullptr);
    
    void receiveMessage(MsgType msg, uint8_t* data, uint8_t size);
    
private:
    Network();
    
    template<typename Struct>
    Struct receiveTcpMsgStruct(uint8_t *data, uint8_t size);
    
private:
    uint8_t m_recBuffer[constants::bufferSize]{0};
    
    std::function<void(TemperatureMessage)> m_onTemperatureCb = nullptr;

};
