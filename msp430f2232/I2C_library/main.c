#include <msp430f2232.h>
#include "msp430f2232_I2C.h"

char i2c_slaveAddress = 0x68;
char I2C_data_to_send = 0x00;
int TXByteCtr;
unsigned char PRxData;
unsigned char pinState = 0;
int Rx = 1;		// we need to read 7 bytes from RTC then we will reset it.

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= 0xFF;
	initiate_I2C_master(i2c_slaveAddress, 20); 	//intialization of I2C

	P3DIR &= ~BIT3;             ///Set P2.3 as button input
	P3REN |= BIT3;              ///Enable Resistor
	P3OUT |= BIT3;              ///Pull up button
	while(1){
	    pinState = P3IN & BIT3;
	    if(pinState == 0)
	    {
	    	P1OUT &= 0x00;
	    	//Transmit process
	    	//Rx = 0;
	    	if(Rx == 8)			// we need to read 7 bytes from RTC then we will reset it.
	    	{
	    		TXByteCtr=1;
	    		I2C_transmit_master(TXByteCtr, &I2C_data_to_send);
	    		Rx=1;
	    	}
	    	//Receive process
	    	PRxData = I2C_receive_master();
//	    	switch(Rx)
//	    	{
//	    	case 1:
//	    		second = PRxData; break;
//	    	case 2:
//	    		minute = PRxData; break;
//	    	case 3:
//	    		hour = PRxData; break;
//	    	case 4:
//	    		week = PRxData; break;
//	    	case 5:
//	    		day = PRxData; break;
//	    	case 6:
//	    		month = PRxData; break;
//	    	case 7:
//	    		year = PRxData; break;
//	    	}

	        Rx++;

	    	pinState = P3IN & BIT3;
	    	while(pinState == 0){
	    		pinState = P3IN & BIT3;
	    	}
	    	P1OUT |= PRxData;
	    }

	}
	return 0;
}




