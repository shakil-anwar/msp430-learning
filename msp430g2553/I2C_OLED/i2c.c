/*
 * i2c.c
 */

#include "i2c.h"

#include <msp430.h>
#include <stdint.h>

#define SDA BIT7                                                        // i2c sda pin
#define SCL BIT6                                                        // i2c scl pin

void i2c_init(unsigned char sync_clock_speed) {
    P1SEL    |= SCL + SDA;                                              // Assign I2C pins to USCI_B0
    P1SEL2   |= SCL + SDA;                                              // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0  = UCMST + UCMODE_3 + UCSYNC;                              // I2C Master, synchronous mode
    UCB0CTL1  = UCSSEL_2 + UCSWRST;                                     // Use SMCLK, keep SW reset
	if (sync_clock_speed == 0)
	{
		UCB0BR0 = 10;                             						// default sync speed fSCL = SMCLK/10 = 100KHz
		UCB0BR1 = 0;
	}else
	{
		UCB0BR0 = (unsigned char)sync_clock_speed & 0xFF;               // fSCL = SMCLK/sync_clock_speed
		UCB0BR1 = (unsigned char)(sync_clock_speed >> 8) & 0xFF;
	}
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    IE2      |= UCB0TXIE;                                               // Enable TX interrupt

} // end i2c_init

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) {
    UCB0I2CSA = slave_address;

    PTxData = DataBuffer;
    TxByteCtr = ByteCtr;

    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
                                                                        // Remain in LPM0 until all data is TX'd
}

void i2c_read(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr)
{
	UCB0I2CSA = slave_address;											//  writting slave address to registar
	
	PTxData = DataBuffer;
    TxByteCtr = ByteCtr;
	
	while (UCB0CTL1 & UCTXSTP);             							// Ensure stop condition got sent
    UCB0CTL1 &= ~UCTR ;                     							// Clear UCTR
    UCB0CTL1 |= UCTXSTT;                    							// I2C start condition
	while (UCB0CTL1 & UCTXSTT);             							// Start condition sent?
    UCB0CTL1 |= UCTXSTP;                    							// I2C stop condition
    __bis_SR_register(CPUOFF + GIE);        							// Enter LPM0 w/ interrupts
																		// Remain in LPM0 until all data is RX'd
}

/* =============================================================================
 * The USCIAB0RX_ISR is structured such that it can be used to receive any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, RXData
 * points to the next byte to receive.
 * ============================================================================= */

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{					   							  						// Master Receieve
	if (TxByteCtr) {
		if(TxByteCtr == 1)
		{
			UCB0CTL1 |= UCTXSTP;                                        // I2C stop condition
		}
        *PTxData = UCB0RXBUF;                      						// Get RX data
		PTxData++;														// Increment of data buffer address
		TxByteCtr--;													// Decrement RX byte counter
	}else {
		__bic_SR_register_on_exit(CPUOFF);        						// Exit LPM0
	}
}


/* =============================================================================
 * The USCIAB0TX_ISR is structured such that it can be used to transmit any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
 * points to the next byte to transmit.
 * ============================================================================= */
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) {
    if (TxByteCtr) {                                                    // Check TX byte counter
        UCB0TXBUF = *PTxData++;                                         // Load TX buffer
        TxByteCtr--;                                                    // Decrement TX byte counter
    } else {
        UCB0CTL1 |= UCTXSTP;                                            // I2C stop condition
        IFG2 &= ~UCB0TXIFG;                                             // Clear USCI_B0 TX int flag
        __bic_SR_register_on_exit(CPUOFF);                              // Exit LPM0
    }
}
