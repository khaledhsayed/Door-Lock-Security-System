/*-------------------------------------------------------------------------------
[FILE NAME]:    final_MC2.h
[AUTHOR(S)]:    Khaled Hassan Sayed
[DATE CREATED]: Dec 16, 2020
[DESCRIPTION]:
 -------------------------------------------------------------------------------*/

#ifndef FINAL_MC2_H_
#define FINAL_MC2_H_

/*-----------------------------INCLUDES-----------------------------------------*/
#include "external_eeprom.h"
#include "uart.h"
#include "timer1_config.h"

/*-----------------------Definitions and configurations-------------------------*/
#define MC1_READY 0XFE
#define MC2_READY 0XFF
#define BUZZER  0XFD
#define DOOR    0XFC
#define NEW_PASSWORD 0XFB
#define EEPROM_ADDRESS 0x0311


/*-------------------------Function Declarations--------------------------------*/

void savePassword(void) ;
void getPassword(void) ;

void goToOperation(uint8 a_operation) ;

void doorActivities(void) ;

void buzzerAlerting(void) ;

#endif /* FINAL_MC2_H_ */
