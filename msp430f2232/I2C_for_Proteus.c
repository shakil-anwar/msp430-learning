#include <msp430f2232.h>

char i2c_slaveAddress = 0x68;
int TXByteCtr;
unsigned char PRxData;
unsigned int Count;
int Rx = 8;		// we need to read 7 bytes from RTC then we will reset it.
char I2C_data_to_send = 0x00;

void initiate_I2C_master(char);
void I2C_transmit_master(void);
void I2C_receive_master(void);

unsigned char pinState = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= BIT0;
	P3SEL  |= BIT1 + BIT2;                     // Assign I2C pins to USCI_B0		P3.1 and P3.2 I2C Data & Clock
	initiate_I2C_master(i2c_slaveAddress); 	//intialization of I2C

	P1DIR &= ~BIT3;             ///Set P1.3 as button input
	P1REN |= BIT3;              ///Enable Resistor
	P1OUT |= BIT3;              ///Pull up button
	while(1){
	    pinState = P1IN & BIT3;
	    if(pinState == 0)
	    {
	    	//Transmit process
	    	//Rx = 0;
	    	if(Rx == 8)			// we need to read 7 bytes from RTC then we will reset it.
	    	{
	    		TXByteCtr = 1;
	    		I2C_transmit_master();
	    		Rx=1;
	    	}
	    	//Receive process
	    	I2C_receive_master();

	    	pinState = P1IN & BIT3;
	    	while(pinState == 0){
	    		pinState = P1IN & BIT3;
	    	}
	    }

	}
	return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
    if(Rx)
    {                              			 	  // Master Receieve
        PRxData = UCB0RXBUF;                      // Get RX data
        Rx++;
        __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0

    }
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{												// Master Receieve
      if (TXByteCtr)                            // Check TX byte counter
      {
    	  UCB0TXBUF = I2C_data_to_send;                     // Load TX buffer
    	  TXByteCtr--;                            // Decrement TX byte counter
      }
      else
      {
    	  UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    	  IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
    	  __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
       }
}

void initiate_I2C_master(char slaveAddress) {
      UCB0CTL1 |= UCSWRST;                      // Enable SW reset
      UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
      UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
      UCB0BR0 = 104;                             // fSCL = SMCLK/104 = 9600
      UCB0BR1 = 0;
      UCB0I2CSA = slaveAddress;                         // Slave Address is 069h
      UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
      IE2 |= UCB0RXIE + UCB0TXIE;               //Enable RX and TX interrupt
}

void I2C_transmit_master(void){
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
}

void I2C_receive_master(void){
        while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
        UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
        UCB0CTL1 |= UCTXSTT;                    // I2C start condition
        while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
        UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
        __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
}


