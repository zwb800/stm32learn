#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    void setup();
    void SetWaterHearterPower(uint32_t pwm);
    void loop();

    void Read();

#ifdef __cplusplus
}
#endif