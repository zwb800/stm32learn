#include "stm32f1xx_hal.h"
#define AGS10_ADDR	0x34

class AGS10 {
    private:
        I2C_HandleTypeDef* i2c;
    public:
        void SetI2C(I2C_HandleTypeDef*);
        int Read();
};