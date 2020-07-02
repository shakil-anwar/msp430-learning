# Initializing or Reconfiguring the USCI SPI Function in msp430f2232
## The recommended USCI initialization or reconfiguration process is:
1. Set UCSWRST in register UCB0CTL1 to enable software reset	
```
UCB0CTL1 |= UCSWRST; 
```
2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1) and configure ports.
  * configure clock and prescaler in SCL
  * configure SPI mode
  * configure master or slave 
  * configure ports
````
P3SEL  |= BIT1 + BIT2 + BIT3;                 // Assign SPI mode pins to USCIB0		P3.1 as SIMO || P3.2 as SOMI  || P3.3 as CLK
P3SEL  &= ~BIT4;								                      // P3.4 is selected as I/O to select device
P3DIR  |= BIT4;								                       // P3.4 selected as output
P3OUT  |= BIT4;								                       // P3.4 output is set to HIGH

UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;     
                                              // UCCKPH = 1; Data is captured on the first UCLK edge and changed on the following edge Clock Phase
                                              // UCMSB  = 1; MSB first
                                              // UCMST  = 1; Master Mode
                                              // UCMODE= 00; 3Pin SPI
                                              // UC7BIT = 0; 8 bit data
                                              // UCSYNC = 1; synchronous mode
UCB0CTL1 |= UCSSEL_2 + UCSWRST;            	  // Use SMCLK, keep SW reset
UCB0BR0 = (unsigned char)sync_clock_speed_prescaler & 0xFF;       // fSCL = SMCLK/sync_clock_speed
UCB0BR1 = (unsigned char)(sync_clock_speed_prescaler >> 8) & 0xFF;
````
3. Clear UCSWRST via software (BIC.B #UCSWRST,&UCxCTL1)
```
UCB0CTL1 &= ~UCSWRST; 
```

## Initialization of registers for Master Mode Transmission
After the initialization of registers for Master Mode Slave device must be selected by by active low.
````
P3OUT  &= ~BIT4;									// Select Device
````
to start writing data to UCB0TXBUF check UCB0TXIFG flag if it is set of not.
```
while(!(IFG2 & UCB0TXIFG));							// checking if TX_Buffer is ready to write
```
when flag is set data can be set to UCB0TXBUF
````
UCB0TXBUF = *txBuffer;								// writing value to TX Buffer
````
As soon as data is set. device will move the data to shift register and start transmission. As transmission and Receive is concurrent in SPI mode,
We will wait for data byte written to UCB0RXBUF by checking UCB0RXIFG flag set or not.
````
while(!(IFG2 & UCB0RXIFG));							// checking if RX_Buffer is ready to read
````
when data is read from UCB0RXBUF, UCB0RXIFG flag will auto matically be cleared.
````
*rxBuffer = UCB0RXBUF; 								// reading value from RX Buffer
````
Then after a complete byte transmit and receive in SPI mode, slave device is deselected.
````
P3OUT  |= BIT4;										// Unselect Device
````

SPI communication STOPs when device is unselected.
