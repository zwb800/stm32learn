#include "stm32f1xx_hal.h"
#define AHT_ADDR	0x38<<1

class AHT20 {
    private:
        I2C_HandleTypeDef* i2c;
    public:
        void SetI2C(I2C_HandleTypeDef* i2c);
        void Read(float* Temp,float* Humid);

};