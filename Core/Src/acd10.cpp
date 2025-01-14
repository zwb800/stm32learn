#include "acd10.h"

void ACD10::SetI2C(I2C_HandleTypeDef* i){
    i2c = i;
}

int ACD10::Read(){
    uint8_t data[2];
    data[0] = 0x03;
    data[1] = 0x00;
    uint8_t receive[9];

    HAL_I2C_Master_Transmit(i2c,0x54,data,2,0xFF);

	HAL_I2C_Master_Receive(i2c,0x54,receive,9,0xFF);

	int co2 = receive[0];
	co2 <<= 8;
	co2 |= receive[1];
	co2 <<= 8;
	co2 |= receive[3];
	co2 <<= 8;
	co2 |= receive[4];
	uint16_t temperature = receive[6] << 8 | receive[7];
	temperature = (temperature - 3200) / 1.8;
    return co2;
}