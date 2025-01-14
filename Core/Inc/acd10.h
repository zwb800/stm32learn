#include "stm32f1xx_hal.h"
#define ACD10_ADDR	0x54

class ACD10 {
    private:
        I2C_HandleTypeDef* i2c;
    public:
        void SetI2C(I2C_HandleTypeDef* i2c);
        int Read();
};