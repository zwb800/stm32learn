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
#include "adc.h"
#include <stdlib.h>

#define ADC_MAX 4095.0 //12bit adc
#define ADC_MAX_VOLTAGE 3.3
#define R1 20100
#define R2 1990
#define MODULE_TIMES (R1+R2)/R2

#define CELL_MAX 13.8
#define CELL_MIN 11.1
#define CELL 2
#define TARGET_CAPACITY 90
#define P 10;



extern "C"{

    int co2;
    int tvoc = -1;
    float temp,humid;
    float voltage;
    int capacity;
    int16_t pwm;

    const float BATTERY_MAX = CELL_MAX * CELL;
    const float BATTERY_MIN = CELL_MIN * CELL;
    const float BATTERY_RANGE = BATTERY_MAX - BATTERY_MIN;

    uint8_t rxBuf[1];
    Relay relay((char*)rxBuf);
    AHT20 aht20;
    ACD10 acd10;
    AGS10 ags10;
    uint8_t relayDelay;

    


    void RxCallback(UART_HandleTypeDef* huart){
        relay.Process();
        HAL_UART_Receive_IT(huart,rxBuf,1);
    }

    void TIMCallback(TIM_HandleTypeDef* tim){
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

        HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
        SetWaterHearterPower(0);
    }

    void SetWaterHearterPower(uint32_t pwm)
    {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, pwm);
    }

    void loop(){
        // Read();
        // HAL_ADC_Start(&hadc1);
        // if(HAL_ADC_PollForConversion(&hadc1,100) == HAL_OK){
        //     voltage = HAL_ADC_GetValue(&hadc1);
        // }
        // HAL_Delay(1000);
    } 
    void Read()
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_ADC_Start(&hadc1);
        if(HAL_ADC_PollForConversion(&hadc1,100) == HAL_OK){
            voltage = HAL_ADC_GetValue(&hadc1) * ADC_MAX_VOLTAGE * MODULE_TIMES / ADC_MAX;
        }
        aht20.Read(&temp, &humid);
        co2 = acd10.Read();
        tvoc = ags10.Read();

        int v1 = tvoc / 1000;
        int v2 = (tvoc - v1 * 1000) / 10;

        int te1 = temp;
        int te2 = abs(temp - te1) * 100;

        int h1 = humid;
        int h2 = (humid - h1) * 100;

        int vo1 = voltage;
        int vo2 = (voltage - vo1) *100;

        capacity = (voltage - BATTERY_MIN) * 100 / BATTERY_RANGE;

        pwm += (capacity - TARGET_CAPACITY) * P;
        pwm = pwm > 10000 ? 10000:pwm;
        pwm = pwm < 0 ? 0:pwm;

        SetWaterHearterPower(pwm);

        char buf[100];
        auto len = sprintf(buf, "CO2:%d ppm TVOC:%d.%d ppm Temp:%d.%02d Hum:%d.%02d%% Battery:%d.%02dv %d%% %s:%s:%s:%s WaterHeater:%d.%02d%%\r\n",
                           co2, v1, v2, te1, te2, h1, h2,vo1,vo2,capacity,
                           relay.State(RELAY0_Pin)?"ON":"OFF",
                           relay.State(RELAY1_Pin)?"ON":"OFF",
                           relay.State(RELAY2_Pin)?"ON":"OFF",
                           relay.State(RELAY3_Pin)?"ON":"OFF",pwm / 100,pwm - (pwm / 100 * 100));

        HAL_UART_Transmit_IT(&huart3, (uint8_t *)buf, len);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        
    }
}
