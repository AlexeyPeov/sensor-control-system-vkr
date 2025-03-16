#include "Potentiometer.h"

extern ADC_HandleTypeDef hadc1;

Potentiometer::Potentiometer() { HAL_ADC_Start(&hadc1); }

int16_t Potentiometer::getValueMapped(int min, int max)
{
    HAL_StatusTypeDef status = HAL_ADC_PollForConversion(&hadc1, 1000);

    if (status != HAL_StatusTypeDef::HAL_OK)
    {
        error(
            "Potentiometer::getValueMapped couldn't read, hal status: %d",
            status
        );

        return min - 1;
    }
    
    constexpr uint32_t ADC_MAX = 4095;
    uint32_t readValue = HAL_ADC_GetValue(&hadc1);
    float normalizedValue = static_cast<float>(readValue) / ADC_MAX;

    float res = min + (normalizedValue * (max - min));

    return static_cast<int16_t>(res);
}
