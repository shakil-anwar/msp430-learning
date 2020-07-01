#include <msp430f2232.h>
#include "msp430f2232_I2C.h"
#include "RTClib_ds1307.h"


unsigned char PRxData;			/// output variable to each time or date byte
unsigned char pinState = 0;
int Rx = 1;						// we need to read 7 bytes from RTC then we will reset it.
RTCds1307 myclock;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;			// stop watchdog timer
	P1DIR |= 0xFF;

	RTC_init(&myclock);					/// Initialization of RTC
	P3DIR &= ~BIT3;             		///Set P2.3 as button input
	P3REN |= BIT3;              		///Enable Resistor
	P3OUT |= BIT3;              		///Pull up button
	myclock.read();
	while(1){
	    pinState = P3IN & BIT3;
	    if(pinState == 0)
	    {

	    	P1OUT &= 0x00;
//	    	//Transmit process

	    	if(Rx == 8)							// we need to read 7 bytes from RTC then we will reset it.
	    	{
	    		myclock.hh = myclock.hh+1;		//Increment of Hour by one to check clock write function
	    		myclock.write();
	    		myclock.read();
	    		Rx=1;
	    	}
//	    	//Receive process

			switch(Rx)
			{
			case 1:
				PRxData = myclock.ss; break;
			case 2:
				PRxData = myclock.mm;break;
			case 3:
				PRxData = myclock.hh; break;
			case 4:
				break;
			case 5:
				PRxData = myclock.d; break;
			case 6:
				PRxData = myclock.m; break;
			case 7:
				PRxData = myclock.y; break;
			}

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
