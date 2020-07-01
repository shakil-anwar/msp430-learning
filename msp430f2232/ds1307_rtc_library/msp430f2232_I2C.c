/*
 * msp430f2232_I2C.h
 *
 *  Created on: Jun 29, 2020
 *      Author: Md Shakil Anwar
 */


#include "msp430f2232_I2C.h"
unsigned char PRxData; 			/// Received Data Buffer
int TXByteCtr;					/// Transmit data byte counter
char *I2C_data_buffer = 0x00;	/// Transmit Data Buffer Pointer

void initiate_I2C_master(char slaveAddress, uint8_t sync_clock_speed) {
	  P3SEL  |= BIT1 + BIT2;                     // Assign I2C pins to USCI_B0		P3.1 and P3.2 I2C Data & Clock
      UCB0CTL1 |= UCSWRST;                      // Enable SW reset
      UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
      UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
      if (sync_clock_speed == 0)
      {
          UCB0BR0 = 10;                             // default sync speed fSCL = SMCLK/10 = 100KHz
          UCB0BR1 = 0;
      }else
      {
    	  UCB0BR0 = (unsigned char)sync_clock_speed & 0xFF;                             // fSCL = SMCLK/sync_clock_speed
    	  UCB0BR1 = (unsigned char)(sync_clock_speed >> 8) & 0xFF;
      }

      UCB0I2CSA = slaveAddress;                         // Slave Address is 069h
      UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
      IE2 |= UCB0RXIE + UCB0TXIE;               //Enable RX and TX interrupt
}

void I2C_transmit_master(int TXByteCounter, char *I2C_dataToSend){
	TXByteCtr = TXByteCounter;
	I2C_data_buffer = I2C_dataToSend;		//copy I2C data to address
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             			// I2C TX, start condition
    __bis_SR_register(CPUOFF + GIE);        			// Enter LPM0 w/ interrupts
}

unsigned char I2C_receive_master(void){
        while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
        UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
        UCB0CTL1 |= UCTXSTT;                    // I2C start condition
        while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
        UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
        __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
        return PRxData;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
												// Master Receieve
        PRxData = UCB0RXBUF;                      // Get RX data
        __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{												// Master Receieve
      if (TXByteCtr)                            // Check TX byte counter
      {
    	  UCB0TXBUF = *I2C_data_buffer;           // Load TX buffer
    	  TXByteCtr--;                            // Decrement TX byte counter
    	  I2C_data_buffer++;					  // Increment of buffer address
      }
      else
      {
    	  UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    	  IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
    	  __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
       }
}
