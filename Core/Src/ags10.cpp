#include "ags10.h"

void AGS10::SetI2C(I2C_HandleTypeDef* i){
    i2c = i;
}

int AGS10::Read(){
    uint8_t receive[5];
    HAL_I2C_Master_Receive(i2c,AGS10_ADDR,receive,5,0xFF);

	uint8_t tvocReady = (receive[0] & 1) == 0;
	int tvoc = -1;
	if(tvocReady){
		tvoc = receive[1];
		tvoc <<=8;
		tvoc |= receive[2];
		tvoc <<=8;
		tvoc |= receive[3];
	}
    return tvoc;
}