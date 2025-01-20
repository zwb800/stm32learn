#include "arduino.h"

#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "aht20.h"
#include "acd10.h"
#include "ags10.h"
#include "relay.h"
#include <stdio.h>
#include "tim.h"


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

    void TIMCallback(TIM_HandleTypeDef* tim){
        if(tim == &htim1)
            Read();
    }

    void setup(){
        aht20.SetI2C(&hi2c1);
        acd10.SetI2C(&hi2c1);
        ags10.SetI2C(&hi2c1);
        HAL_UART_RegisterCallback(&huart3,HAL_UART_RX_COMPLETE_CB_ID,RxCallback);
        HAL_UART_Receive_IT(&huart3,rxBuf,1);

        HAL_TIM_RegisterCallback(&htim1,HAL_TIM_PERIOD_ELAPSED_CB_ID,TIMCallback);
        HAL_TIM_Base_Start_IT(&htim1);
    }

    void loop(){
        // Read();
        // HAL_Delay(5000);
    }
    void Read()
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        aht20.Read(&temp, &humid);
        co2 = acd10.Read();
        tvoc = ags10.Read();

        int v1 = tvoc / 1000;
        int v2 = (tvoc - v1 * 1000) / 10;

        int te1 = temp;
        int te2 = (temp - te1) * 100;

        int h1 = humid;
        int h2 = (humid - h1) * 100;

        char buf[100];
        auto len = sprintf(buf, "CO2:%d ppm TVOC:%d.%d ppm Temperature:%d.%d Humidity:%d.%d%% %s:%s:%s \r\n",
                           co2, v1, v2, te1, te2, h1, h2,
                           relay.State(RELAY0_Pin)?"ON":"OFF",
                           relay.State(RELAY1_Pin)?"ON":"OFF",
                           relay.State(RELAY2_Pin)?"ON":"OFF");

        HAL_UART_Transmit_IT(&huart3, (uint8_t *)buf, len);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        
    }
}
