/*-------------------------------------------------------------------------------
[FILE NAME]:    final_MC1.h
[AUTHOR(S)]:    Khaled Hassan Sayed
[DATE CREATED]: Dec 16, 2020
[DESCRIPTION]:
 -------------------------------------------------------------------------------*/

#ifndef FINAL_MC1_H_
#define FINAL_MC1_H_




/*-----------------------------INCLUDES-----------------------------------------*/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1_config.h"

/*-----------------------Definitions and configurations-------------------------*/
#define MC1_READY 0XFE
#define MC2_READY 0XFF
#define BUZZER  0XFD
#define DOOR    0XFC
#define NEW_PASSWORD 0XFB


/*-------------------------Function Declarations--------------------------------*/

void newPassword(void) ;

void sendPasswordToMC2(void) ;

void isPasswordThenAction(uint8 key) ;

void buzzerTermination(void) ;

void doorOperations(void) ;


#endif /* FINAL_MC1_H_ */
