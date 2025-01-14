#include "arduino.h"

#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "aht20.h"
#include "acd10.h"
#include "ags10.h"
#include "relay.h"


extern "C"{

    int co2;
    int tvoc = -1;
    float temp,humid;
    uint8_t rxBuf[1];
    Relay relay((char*)rxBuf);
    AHT20 aht20;
    ACD10 acd10;
    AGS10 ags10;


    void RxCallback(UART_HandleTypeDef* huart){
        relay.Process();
        HAL_UART_Receive_IT(huart,rxBuf,1);
    }

    void setup(){
        aht20.SetI2C(&hi2c1);
        acd10.SetI2C(&hi2c1);
        ags10.SetI2C(&hi2c1);
        HAL_UART_RegisterCallback(&huart3,HAL_UART_RX_COMPLETE_CB_ID,RxCallback);
        HAL_UART_Receive_IT(&huart3,rxBuf,1);
    }

    void loop(){
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
        aht20.Read(&temp,&humid);
        co2 = acd10.Read();
        tvoc = ags10.Read();

        int v1 = tvoc / 1000;
        int v2 = (tvoc - v1 * 1000) / 10;

        int te1 = temp;
        int te2 = (temp - te1) * 100;

        int h1 = humid;
        int h2 = (humid - h1) * 100;

        char buf[100];
        auto len = sprintf(buf,"CO2:%d ppm TVOC:%d.%d ppm Temperature:%d.%d Humidity:%d.%d%%\r\n",
            co2,v1,v2,te1,te2,h1,h2);

        
        CDC_Transmit_FS((uint8_t*)buf, len);
        HAL_UART_Transmit_IT(&huart3,(uint8_t*)buf,len);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
        HAL_Delay(5000);
    }
}
