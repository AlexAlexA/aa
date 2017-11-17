/**
  ******************************************************************************
  * @file    main.c
  * @author  Alexander Steinmetz
  * @version V1.0
  * @date    29-December-2016
  * @brief   main function and interrupts
  ******************************************************************************


**********************************************************************/
#include "stm32f4xx_conf.h"
#include "stm32f4xx_conf.h"
#include "init.h"
#include "globalheader.h"
#include "eeprom.h"
#include "shortcuts.h"
#include "debug.h"
#include "motor_driver.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>


#define MOTOR   mdriver

volatile uint8_t debounce_flag=0;
volatile uint32_t i;

int main(void)
{
    //RCC_DeInit();
    //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

for (i = 0; i < 2000000; i++);        // Warten bis Spannung stabil etwa 60 ms damit Daten aus Flash korrekt gelesen werden
    SystemInit();
    InitClock();
    InitGPIO();
    InitInterrupt();
    initUSART1();                       // für Empfangen von Bluetooth-Transceiver
    init_debug();                       // für Debug-Meldungen (USART6)
    InitTimer();                        // Timer auf 1s eingestellt
    MOTOR.init();
    //InitWDG();

    /*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    GPIO_InitTypeDef GPIO_Led;
    GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Led.GPIO_OType = GPIO_OType_PP;
    GPIO_Led.GPIO_Pin = GPIO_Pin_12;
    GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Led.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_Led);*/

  while(1)
  {
      PRINTF("\n\rTest ob Debug Meldungen angezeigt werden.");
      //GPIOC->ODR ^= GPIO_Pin_12;
      for(volatile int i=0; i<=10000000; i++);

  }
}


/**< USART Interrrupt für RX */
void USART1_IRQHandler (void){

    char cmd[2];
    //uint16_t debug_flagreg;
    PRINTF("\n\rInterrupt fuer neues Kommando von Transceiver: ");
    //volatile uint32_t Timeout = 0xFFFFF;
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    //while(((debug_flagreg = USART1->SR)&(1<<5)) == 0){
    //if (Timeout-- == 0) break;
    //}
    cmd[0] = (char)USART_ReceiveData(USART1);
    cmd[1] = '\0';

    switch (cmd[0]) {
    case 'R':
        PRINTF(cmd);
        PRINTF("\n\r-Rechts");
        MOTOR.rotate_right();
        //rotate_right_test();
        TIM_SetCounter(TIM10,0);
        TIM_Cmd(TIM10,ENABLE);
        break;
    case 'L':
        PRINTF(cmd);
        PRINTF("\n\r-Links");
        MOTOR.rotate_left();
        //rotate_left_test();
        TIM_SetCounter(TIM10,0);
        TIM_Cmd(TIM10,ENABLE);
        break;
    case 'H':
        PRINTF(cmd);
        PRINTF("\n\r-Hoch");
        MOTOR.up();
        //up_test();
        break;
    case 'U':
        PRINTF(cmd);
        PRINTF("\n\r-Runter");
        MOTOR.down();
        //down_test();
        break;
    case 'A':
        PRINTF(cmd);
        PRINTF("\n\r-Aufstehen");
        MOTOR.down();
        //down_test();
        break;
    case 'S':
        PRINTF(cmd);
        PRINTF("\n\r-Stopp");
        MOTOR.off();
        //off_test();
        break;
    case 'T':
        PRINTF(cmd);
        PRINTF("\n\r-Rechts Automatik");
        MOTOR.rotate_right();
        //rotate_right_test();
        break;
    case 'B':
        PRINTF(cmd);
        PRINTF("\n\r-Links Automatik");
        MOTOR.rotate_left();
        //rotate_left_test();
        break;
    }

    USART_ClearFlag(USART1, USART_FLAG_ORE);
    USART_ClearFlag(USART1,USART_FLAG_RXNE);

}

/**< Timer 10 Interrrupt für Motoren abschalten */
void TIM1_UP_TIM10_IRQHandler(void){
    PRINTF("\n\rTimer-Interrupt zum Abschalten der Motoren.");
    if(TIM_GetITStatus(TIM10,TIM_IT_Update)){
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
        //off_test();
        TIM_Cmd(TIM10,DISABLE);
    }

}
