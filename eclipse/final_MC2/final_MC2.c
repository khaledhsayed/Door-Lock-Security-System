
/*-------------------------------------------------------------------------------
[FILE NAME]:    final_MC2.c
[AUTHOR(S)]:    Khaled Hassan Sayed
[DATE CREATED]: Dec 16, 2020
[DESCRIPTION]:
 -------------------------------------------------------------------------------*/

/*-----------------------------INCLUDES-----------------------------------------*/
#include "final_MC2.h"


/*-------------------------Global Variables-------------------------------------*/
volatile uint8 g_password[10] ; /* array to save password of 6 characters */

static uint8 g_delayFlag = 0 ;/* flag for delaying the system if there is a thief */

static uint8 g_ticks = 0 ; /* to count No. of overflows of TIMER1 then certain time*/



/*-------------------------Function Definitions---------------------------------*/

/*-------------------------------------------------------------------------------
[Function Name]: savePassword
[Description]: save the password to EEPROM
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void savePassword(void)
{
	uint8 i ;

	/* save every character of password in separate location in EEPROM*/
	for(i = 0 ; i < 6 ; i++)
	{
		EEPROM_writeByte(EEPROM_ADDRESS+i , g_password[i]) ;
	}
}

/*-------------------------------------------------------------------------------
[Function Name]: getPassword
[Description]:  getting the current password from EEPROM
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void getPassword(void)
{
	uint8 i ;

	/* getting every character of password from separate locations from EEPROM*/
	for(i = 0 ; i < 6 ; i ++)
	{
		EEPROM_readByte(EEPROM_ADDRESS+i , &(g_password[i])) ;
	}
	/*  putting '#' for UART_recieveByte technique*/
	g_password[i] = '#' ;
}

/*-------------------------------------------------------------------------------
[Function Name]:  goToOperation
[Description]:  checks what is the operation requested from MC1
				then go to its function
[Args]:
[in]:     a_operation : getting the operation from MC1 then passing it here
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void goToOperation(uint8 a_operation)
{
	/*if operation is NEW_PASSWORD then go to savePassword function */
	if(a_operation == NEW_PASSWORD)
	{
		/* two Micro-controllers tells to us that they are ready
		 * for synchronization between them*/
		while(UART_recieveByte() != MC1_READY); // wait until MC1 is ready
    	UART_sendByte(MC2_READY) ;
    	UART_receiveString(g_password) ;

    	/*to save the new password */
    	savePassword() ;
	}

	/* if operation is BUZZER then buzzer alerting*/
	else if(a_operation == BUZZER)
	{
		/* turn the buzzer on*/
		SET_BIT(PORTC , PC4) ;

	    /*configuration of TIMER1 to do buzzerTermination function at the interrupt of mode
	     *  with callback technique*/
	    /* 1. overflow mode
	     * 2. initial value = 0
	     * 3. compare A value = 0 (unused)
	     * 4. compare B value = 0 (unused)
	     * 5. Duty Cycle value = 0 (unused)
	     * 6. Pre-scalar = 64 */
		Timer1_OVF_setCallBack(buzzerAlerting) ;
	    TIMER1_ConfigType TIMER1_config = { OVERFLOW , 0 , 0  , 0 ,0 , F_CPU_64} ;
	    TIMER1_init(&TIMER1_config ) ;

	    /* entering while loop for delaying till 1 minute passes
		     * then zero the flag from buzzerAlerting function */
	    g_delayFlag = 1 ;
	    while(g_delayFlag == 1) ;

	    /* turn the buzzer off after 1 minute*/
		CLEAR_BIT(PORTC , PC4) ;
	}
	/* if operation is DOOR then open the door and close it automatically */
	else if(a_operation == DOOR)
	{
		/* first turn the motor clockwise to open the door */
		PORTC &= ~(1<<PC2) ;
		PORTC |= (1<<PC3) ;

	    /*configuration of TIMER1 to do doorActivities function at the interrupt of mode
	     *  with callback technique*/
	    /* 1. overflow mode
	     * 2. initial value = 0
	     * 3. compare A value = 0 (unused)
	     * 4. compare B value = 0 (unused)
	     * 5. Duty Cycle value = 0 (unused)
	     * 6. Pre-scalar = 64 */
   		Timer1_OVF_setCallBack(doorActivities) ;
    	TIMER1_ConfigType TIMER1_config = { OVERFLOW , 0 , 0  , 0 ,0 , F_CPU_64} ;
    	TIMER1_init(&TIMER1_config ) ;

	    /* entering while loop for delaying till door is opening and closing
	     * then zero the flag from buzzerTermination function */
    	g_delayFlag = 1 ;
    	while(g_delayFlag == 1) ;
	}

}

/*-------------------------------------------------------------------------------
[Function Name]:doorActivities
[Description]: open the door in 15 seconds then hold for 5 seconds
				then close it in 15 seconds
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void doorActivities(void)
{

	g_ticks++ ; /* No. of overflows of TIMER1 */
	SET_BIT(SREG , 7) ; /*AVR maybe clear this bit after interrupt
		then open it manually again */

	/*every tick (overflow) takes 0.5 second then
	 * at first 15 seconds door is opening */
	if(g_ticks == 30)
	{
		PORTC &= ~(1<<PC2) & ~(1<<PC3) ;
	}

	/* the door holds for 3 seconds */
	else if(g_ticks == 36)
	{
		PORTC &= ~(1<<PC3) ;
		PORTC |= (1<<PC2) ;
	}

	/* the last 15 seconds door is closing */

	/*after 15 + 3 + 15 seconds operation is done */
	else if(g_ticks == 66)
	{
		PORTC &= ~(1<<PC2) & ~(1<<PC3) ;

		g_delayFlag = 0 ;
		g_ticks = 0 ;
		Timer1_DeInit();
	}

}

/*-------------------------------------------------------------------------------
[Function Name]: buzzerAlerting
[Description]: calculate 1 minute then turn off the buzzer
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void buzzerAlerting(void)
{
	g_ticks++ ;/* No. of overflows of TIMER1 */
	SET_BIT(SREG , 7) ;/*AVR maybe clear this bit after interrupt
	 then open it manually again */

	/*every tick (overflow) takes 0.5 sec then it is 1 minute */
	if(g_ticks >=120)
	{

		CLEAR_BIT(PORTC , PC4) ; /*turn off the buzzer */

		/*zero the delay to exit from the while loop of delaying */
		g_delayFlag = 0 ;

		g_ticks = 0 ;/* zero the ticks for any other interrupt*/
		Timer1_DeInit(); /* de-activate the timer*/

	}

}



int main(void)
{

	uint8 operation ; /*to store the operation getted from MC1 */

	/* make the PC2 , PC3 and PC4 pins output pins for motor and buzzer*/
	DDRC |= (1<<PC2) | (1<<PC3) | (1<<PC4) ;

	/*initialization of configure UART driver by :
	 * 1. 8 bits data
	 * 2. baud rate of 9600
	 * 3. disable parity bit
	 * 4. one stop bit
	 *
	 * then send structure of configuration to the driver*/
	Uart_ConfigType UART_config = {8 , 9600 ,DISABLED , ONE_STOP_BIT };
	UART_init(&UART_config) ;

	/*initialization of EEPROM */
	EEPROM_init() ;

	SET_BIT(SREG , 7) ;/*enabling global interrupt bit */

	/* two Micro-controllers tells to us that they are ready
	 * for synchronization between them*/
	while(UART_recieveByte() != MC1_READY); // wait until MC1 is ready
	UART_sendByte(MC2_READY) ;
	UART_receiveString(g_password) ;

	/* save first sended password in the EEPROM*/
	savePassword() ;

	/*the application is even getting password and send it to MC1
	 * or recieve a  new password to save in EEPROM
	 * or doing operation even with door motor or buzzer*/
    while(1)
    {
    	getPassword() ;

    	/* two Micro-controllers tells to us that they are ready
    	 * for synchronization between them*/
    	UART_sendByte(MC2_READY) ;
    	while(UART_recieveByte() != MC1_READY); // wait until MC1 is ready
    	UART_sendString(g_password) ;

    	/* two Micro-controllers tells to us that they are ready
    	 * for synchronization between them*/
    	while(UART_recieveByte() != MC1_READY); // wait until MC1 is ready
    	UART_sendByte(MC2_READY) ;
    	operation = UART_recieveByte() ;

    	/* send the operation to function then checks
    	 * what is the operation requested from MC1
    					then go to its function */
    	goToOperation(operation) ;

    }
}
