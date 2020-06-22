# Initializing or Reconfiguring the USCI I2C Function in msp430f2232
## The recommended USCI initialization or reconfiguration process is:
1. Set UCSWRST in register UCB0CTL1 to enable software reset	
```
UCB0CTL1 |= UCSWRST; 
```
2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1) and configure ports.
```
1. configure clock and prescaler in SCL
2. configure I2C mode
3. configure master or slave or multimaster
4. Set general call response
5. Set slave address or master address(optional)
6. Set transmitter or reciver bit
```
3. Clear UCSWRST via software (BIC.B #UCSWRST,&UCxCTL1)
```
UCB0CTL1 &= ~UCSWRST; 
```
4. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
```
IE2 |= UCB0RXIE + UCB0TXIE;
```
## Initialization of registers for Master Mode
The USCI module is configured as an I2C master by selecting the I2C mode with UCMODEx = 11 and synchronous mode with UCSYNC = 1 and setting the UCMST bit. 
```
UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;
```
When the master is part of a multi-master system, UCMM must
be set and its own address must be programmed into the UCBxI2COA register. 
```
UCB0CTL1 |= UCMM; (optional)
CBxI2COA is a 16 bit address
UCGCEN- Bit 15- General call response enable (0-Do not respond to a general call, 1-Respond to a general call)
I2COAx Bits 9-0 I2C own address. The I2COAx bits contain the local address of the USCI_Bx I2C controller. The address is
right-justified. In 7-bit addressing mode, bit 6 is the MSB, and bits 9-7 are ignored. In 10-bit addressing
mode, bit 9 is the MSB.
```
Set master address bit length
```
UCA10 = 0; when 7-bit
UCA10 = 1; when 10-bit
```
Set UCSLA10 bit = 0 if address is 7 bit or UCSLA10 =0
```
UCB0CTL0 |= UCSLA10;
```
Set slave address to UCBxI2CSA register.
```
UCBxI2CSA= 0xADDRESS; 7 bit or 10 bit address. other bits are ignored.
```

## I2C Master Transmitter Mode
UCTR must be set to configure transmitter and Set UCTXSTT to generate a START condition. (then bus availability is checked)
```
UCB0CTL1 |= UCTR + UCTXSTT; 
```
When an interrupt is enabled, and the GIE bit is set, the interrupt flag will generate an interrupt request.
```
__bis_SR_register(CPUOFF + GIE); 
```
UCB0TXIFG interrupt bit is set  when START condition is generated. then it kicks USCIAB0TX_VECTOR and USCIAB0TX_ISR is called. Data is sent till last byte
```
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
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
```
UCBxTXIFG is set again as soon as the data is transferred from the buffer into the shift register. If there is no Data in UCBxTXBUF bus is held with SCL low until data is written into UCBxTXBUF. Data is transmitted or the bus is held as long as the UCTXSTP bit or UCTXSTT bit is not set.
Setting UCTXSTP will generate a STOP condition after the next acknowledge from the slave.
If the slave does not acknowledge the transmitted data the not-acknowledge interrupt flag UCNACKIFG is set. Then the master must react with either a STOP condition or a repeated START condition.

## I2C Master Receiver Mode



