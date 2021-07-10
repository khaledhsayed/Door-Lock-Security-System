/*-------------------------------------------------------------------------------
[FILE NAME]:    final_MC1.c
[AUTHOR(S)]:    Khaled Hassan Sayed
[DATE CREATED]: Dec 16, 2020
[DESCRIPTION]:
 -------------------------------------------------------------------------------*/

/*-----------------------------INCLUDES-----------------------------------------*/
#include "final_MC1.h"

/*-------------------------Global Variables-------------------------------------*/

volatile uint8 g_password[10] ; /* array to save password of 6 characters */

static  uint8 g_delayFlag = 0 ; /* flag for delaying the system if there is a thief */

static  uint8 g_ticks = 0 ; /* to count No. of overflows of TIMER1 then certain time*/

/*-------------------------Function Definitions---------------------------------*/

/*-------------------------------------------------------------------------------
[Function Name]: newPassword
[Description]: make the user to put a new password
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void newPassword(void)
{
	uint8 i , matchingFlag = 0 ; /* flag to check matching of re-entered password with new one*/
	uint8 newPassword [10] , reEnteredPassword [10] ; /* arrays to store new and re-entered passwords*/

	/* loop that not exit from till matching occur */
	while(matchingFlag == 0)
	{
		matchingFlag = 1 ; /* initially matched and if it isn't this flag equal zero */
		/*tell the user to "Enter New Password then go to next row */
		LCD_clearScreen() ;
		LCD_displayStringRowColumn(0 , 0 ,"Enter New Pass" );
		LCD_goToRowColumn( 1 , 0 ) ;

		/* make the user to enter the new password*/
    	for(i = 0 ; i < 6 ; i++)
    	{
    		/* getting and store password from the keypad */
    		newPassword [i] = KeyPad_getPressedKey() ;
    		LCD_displayString("*") ; /* show to user '*' at any password character */
    		_delay_ms(1600) ; /* delay for debouncing */

    	}

    	/* telling the user to Re-enter the password for safety*/
    	LCD_clearScreen() ;
    	LCD_displayStringRowColumn(0 , 0 , "Re-Enter New Pass" );
    	LCD_goToRowColumn( 1 , 0 ) ;
    	/* make the user to Re-enter the password for safety*/
    	for(i = 0 ; i < 6 ; i++)
    	{
    		/* getting and store re-entered password from the keypad */
    		reEnteredPassword [i] = KeyPad_getPressedKey() ;
    		LCD_displayString("*") ;/* show to user '*' at any password character */
    		_delay_ms(1600) ;/* delay for debouncing problems */

    	}
    	/* checking the matching between new password and Re-entered one  */
    	for(i = 0 ; i < 6 ; i++)
    	{
    		/* at any unmatched character then zero matching flag and do the while loop again*/
    		if(reEnteredPassword [i] != newPassword [i] )
    		{
    	    	LCD_clearScreen() ;
    	    	LCD_displayStringRowColumn(0 , 0 ,"Unmatched! again" );
    	    	_delay_ms(30000) ;
    			matchingFlag  = 0 ;
    			break ;
    		}
    	}

	}

	/* after matching then copying password to the global array
	 * and putting '#' for UART_recieveByte technique*/
	for(i = 0 ; i < 6 ; i++)
	{
		g_password[i] = newPassword [i] ;
	}
	g_password[i] = '#' ;

	/* function to send password using UART */
	sendPasswordToMC2() ;

}

/*-------------------------------------------------------------------------------
[Function Name]:  sendPasswordToMC2
[Description]:  send password using UART
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void sendPasswordToMC2(void)
{
	/* two Micro-controllers tells to us that they are ready
	 * for synchronization between them*/
	UART_sendByte(MC1_READY) ;
	while(UART_recieveByte() != MC2_READY); // wait until MC2 is ready
	UART_sendString(g_password) ;
}

/*-------------------------------------------------------------------------------
[Function Name]: isPasswordThenAction
[Description]:  checks the password entered with saved one initially then
				even true then do the action user needed or wrong 3 times
				then buzzer start with 1 minute alerting
[Args]:
[in]:     a_keyPressed : the key-pressed from user
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void isPasswordThenAction(uint8 a_keyPressed)
{
	/* matching flag for check matching entered password with saved one
	 * and wrong counter for counting till 3 wrong passwords enters */
	uint8 i , matchingFlag = 0 , wrongCounter = 0  ;
	/* array to save entered password from user*/
	uint8 enteredPassword[10];

	/* two micro-controllers tells to us that they are ready
	 * for synchronization between them*/
   	while(UART_recieveByte() != MC2_READY); // wait until MC2 is ready
	UART_sendByte(MC1_READY) ;
	UART_receiveString(g_password) ;

	/* LCD shows waiting for readability of the program*/
	LCD_clearScreen() ;
	LCD_displayStringRowColumn(0 , 0 , "waiting...") ;
	_delay_ms(30000);

	/* loop that not exit from till matching occur or 3 wrong tries */
	while((matchingFlag == 0) && (wrongCounter < 3))
	{
		matchingFlag = 1 ;/* initially matched and if it isn't this flag equal zero */

		/*tell the user to "Enter old Password then go to next row in LCD*/
    	LCD_clearScreen() ;
		LCD_displayStringRowColumn(0 , 0 , "Enter Old Pass") ;
		LCD_goToRowColumn( 1 , 0 ) ;

   		/* getting and store entered password from the keypad */
		for(i = 0 ; i < 6 ; i++)
		{
			enteredPassword [i] = KeyPad_getPressedKey() ;
			LCD_displayString("*") ;
			_delay_ms(1600) ;

		}
		/* checking the matching between entered password from user and saved onr */
	   	for(i = 0 ; i < 6 ; i++)
	    {
	   		/* at any unmatched character then zero matching flag
	   		 * and do the while loop again*/
	    	if(enteredPassword [i] != g_password [i] )
	    	{
	    	    LCD_clearScreen() ;
	    	    LCD_displayStringRowColumn(0 , 0 ,"Wrong Password!" );
	    	    _delay_ms(30000) ;
	    	    wrongCounter++ ;
	    		matchingFlag  = 0 ;
	    		break ;
	    	}
	    }


	}
	/* if the reason of exiting from the while loop was because of 3 times wrong entering */
	if(wrongCounter >= 3)
	{

		/* two micro-controllers tells to us that they are ready
		 * for synchronization between them*/
		UART_sendByte(MC1_READY) ;
		while(UART_recieveByte() != MC2_READY); // wait until MC2 is ready
		/* send to micro-controller 2 to switch on the buzzer for 1 minute*/
		UART_sendByte(BUZZER) ;

		/* display on LCD that it is a thief for more safety */
	    LCD_clearScreen() ;
	    LCD_displayStringRowColumn(0 , 0 ,"Thief!!!" );

	    /*configuration of TIMER1 to do buzzerTermination function at the interrupt of mode
	     *  with callback technique*/
	    /* 1. overflow mode
	     * 2. initial value = 0
	     * 3. compare A value = 0 (unused)
	     * 4. compare B value = 0 (unused)
	     * 5. Duty Cycle value = 0 (unused)
	     * 6. Pre-scalar = 64 */
		Timer1_OVF_setCallBack(buzzerTermination) ;
	    TIMER1_ConfigType TIMER1_config = { OVERFLOW , 0 , 0  , 0 ,0 , F_CPU_64} ;
	    TIMER1_init(&TIMER1_config ) ;

	    /* entering while loop for delaying till 1 minute passes
	     * then zero the flag from buzzerTermination function */
	    g_delayFlag = 1 ;
	    while(g_delayFlag == 1) ;
	}
	/* if the reason of exiting from the while loop was because of password is correct
	 * and operation is changing password by '*' pressing */
	else if (a_keyPressed == '*')
	{
		/* two micro-controllers tells to us that they are ready
		 * for synchronization between them*/
		UART_sendByte(MC1_READY) ;
		while(UART_recieveByte() != MC2_READY); // wait until MC2 is ready
		UART_sendByte(NEW_PASSWORD) ; /* send to MC2 that there will be new password to save in EEPROM*/

		/* changing password of the system again*/
		newPassword() ;
	}
	/* if the reason of exiting from the while loop was because of password is correct
	 * and operation is opening the door by '%' pressing */
	else if(a_keyPressed == '%')
	{
		/* two micro-controllers tells to us that they are ready
		 * for synchronization between them*/
		UART_sendByte(MC1_READY) ;
		while(UART_recieveByte() != MC2_READY); // wait until MC2 is ready
		UART_sendByte(DOOR) ;

		/*display the status of the door in the fuction doorOperation */
	    LCD_clearScreen() ;
	    LCD_displayStringRowColumn(0 , 0 ,"DOOR IS :" );

	    /*configuration of TIMER1 to do doorOperation function at the interrupt of mode
	     *  with callback technique*/
	    /* 1. overflow mode
	     * 2. initial value = 0
	     * 3. compare A value = 0 (unused)
	     * 4. compare B value = 0 (unused)
	     * 5. Duty Cycle value = 0 (unused)
	     * 6. Pre-scalar = 64 */
	    Timer1_OVF_setCallBack(doorOperations) ;
	    TIMER1_ConfigType TIMER1_config = { OVERFLOW , 0 , 0  , 0 ,0 , F_CPU_64} ;
	    TIMER1_init(&TIMER1_config ) ;

	    /* entering while loop for delaying till door is opening and closing
	     * then zero the flag from buzzerTermination function */
	    g_delayFlag = 1 ;
	    while(g_delayFlag == 1) ;
	}


}

/*-------------------------------------------------------------------------------
[Function Name]:buzzerTermination
[Description]: calculate the time of buzzer then delay the whole system
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void buzzerTermination(void)
{

	g_ticks++ ; /* No. of overflows of TIMER1 */
	LCD_displayStringRowColumn(1 , 0 ,"Catch him" );
	SET_BIT(SREG , 7) ; /*AVR maybe clear this bit after interrupt
	 then open it manually again */

	/*every tick (overflow) takes 0.5 sec then it is 1 minute */
	if(g_ticks >=120)
	{
	  	LCD_clearScreen() ;
		LCD_displayStringRowColumn(1 , 0 ,"Try again" );
		_delay_ms(10000) ;

		/*zero the delay to exit from the while loop of delaying */
		g_delayFlag = 0 ;
		g_ticks = 0 ; /* zero the ticks for any other interrupt*/
		Timer1_DeInit(); /* de-activate the timer*/

	}

}

/*-------------------------------------------------------------------------------
[Function Name]: doorOperations
[Description]: shows the door status on LCD
[Args]:   NONE
[in]:     NONE
[out]:    NONE
[Return]: NONE
 -------------------------------------------------------------------------------*/
void doorOperations(void)
{

	g_ticks++ ;/* No. of overflows of TIMER1 */
	SET_BIT(SREG , 7) ; /*AVR maybe clear this bit after interrupt
	then open it manually again */

	/*every tick (overflow) takes 0.5 second then
	 * at first 15 seconds door is opening */
	if(g_ticks < 30)
	{
		LCD_displayStringRowColumn(1 , 0 ,"opening" );
	}
	/* the door holds for 3 seconds */
	else if(g_ticks < 36)
	{
		LCD_displayStringRowColumn(1 , 0 ,"holding" );
	}
	/* the last 15 seconds door is closing */
	else if(g_ticks < 66)
	{
		LCD_displayStringRowColumn(1 , 0 ,"closing" );
	}
	/*after 15 + 3 + 15 seconds operation is done */
	else if(g_ticks >= 66)
	{
	  	LCD_clearScreen() ;
		LCD_displayStringRowColumn(1 , 0 ,"Done" );
		_delay_ms(10000) ;

		/*zero the delay to exit from the while loop of delaying */
		g_delayFlag = 0 ;
		g_ticks = 0 ; /* zero the ticks for any other interrupt*/
		Timer1_DeInit(); /* de-activate the timer*/
	}

}

int main(void)
{
	uint8 keyPressed ; /* holds the pressing of even '*' or '%'*/
	LCD_init() ; /*initialization of LCD */

	/*initialization of configure UART driver by :
	 * 1. 8 bits data
	 * 2. baud rate of 9600
	 * 3. disable parity bit
	 * 4. one stop bit
	 *
	 * then send structure of configuration to the driver*/
	Uart_ConfigType UART_config = {8 , 9600 ,DISABLED , ONE_STOP_BIT };
	UART_init(&UART_config) ; /* */

	newPassword(); /* first time password */

	SET_BIT(SREG , 7) ; /*enabling global interrupt bit */


	/*the application is even change password or open the door */
    while(1)
    {
    	/* display options of application*/
    	LCD_displayStringRowColumn(0 , 0 , " *: Change Pass") ;
    	LCD_displayStringRowColumn(1 , 0 , " %: Open Door") ;

    	/*waiting tell even '*' or '%' pressing */
    	keyPressed = KeyPad_getPressedKey();
    	if(keyPressed == '*' || keyPressed == '%' )
    	{
    		/*after pressing checking that it is the user from password
    		 * then do the action needed */
    		isPasswordThenAction(keyPressed);
    	}

    }
}
