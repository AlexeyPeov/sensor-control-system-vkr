#include "fn.h"

#include "stm32f1xx_hal.h"

void fn::DelayMicro(uint32_t us)
{
    uint32_t cycles = (SystemCoreClock / 1000000L) * us;
    uint32_t start = DWT->CYCCNT;
    volatile uint32_t cnt;

    do
    {
        cnt = DWT->CYCCNT - start;
    } while (cnt < cycles);
}