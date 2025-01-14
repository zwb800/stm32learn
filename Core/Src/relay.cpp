#include <stdio.h>
#include <string.h>
#include "relay.h"

#include "usart.h"
#include "stm32f1xx_hal_uart.h"


Relay::Relay(char* c){
    chr = c;
}

void Relay::Process(){
        char c = *chr;
        bool on = true;

        uint16_t pin = 100;
        if(c >= '5' && c <= '8'){
            on = false;
            c -= 4;
        }

        if(c == '1'){
        pin = RELAY0_Pin;
        }
        else if(c == '2'){
        pin = RELAY1_Pin;
        }
        else if(c == '3'){
        pin = RELAY2_Pin;
        }
        else if(c == '4'){
        pin = RELAY3_Pin;
        }
        
        if(pin != 100){
            char sBuf[100];
            uint8_t s = sprintf(sBuf, "Switch %c %s\r\n",c,on?"ON":"OFF");
            HAL_UART_Transmit(&huart3,(uint8_t*)sBuf,s,0xFF);

            HAL_GPIO_WritePin(RELAY0_GPIO_Port, pin,on? GPIO_PIN_SET:GPIO_PIN_RESET);
        }
        else{
            auto errTxt = "unknow command\r\n";
            HAL_UART_Transmit(&huart3,(uint8_t*)errTxt,strlen(errTxt),0xFF);
        }
}