Initializing or Reconfiguring the USCI I2C Function in msp430f2232
The recommended USCI initialization or reconfiguration process is:
1. Set UCSWRST  in register UCB0CTL1 			//setting software reset
2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1) and configure ports.
3. Configure ports.
4. Clear UCSWRST via software (BIC.B #UCSWRST,&UCxCTL1)
5. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
Master Mode
The USCI module is configured as an I2C master by selecting the I2C mode with UCMODEx = 11 and
UCSYNC = 1 and setting the UCMST bit. When the master is part of a multi-master system, UCMM must
be set and its own address must be programmed into the UCBxI2COA register. When UCA10 = 0, 7-bit
addressing is selected. When UCA10 = 1, 10-bit addressing is selected. The UCGCEN bit selects if the
USCI module responds to a general call. UCTR may be set or cleared to configure transmitter or receiver,

I2C Master Transmitter Mode

Set slave address to UCBxI2CSA register.
Set UCSLA10 bit = 0 if address is 7 bit or UCSLA10 =0
Set UCTXSTT to generate a START condition. (then bus availability is checked)
UCBxTXIFG is set if START is generated after transmission slave address.0
UCBxTXIFG is set again as soon as the data is transferred from the buffer into the shift register. If there is no Data in UCBxTXBUF bus is held with SCL low until data is written into UCBxTXBUF. Data is transmitted or the bus is held as long as the UCTXSTP bit or UCTXSTT bit is not set.
Setting UCTXSTP will generate a STOP condition after the next acknowledge from the slave.
If the slave does not acknowledge the transmitted data the not-acknowledge interrupt flag UCNACKIFG is set. Then the master must react with either a STOP condition or a repeated START condition.


6. In USCI_BX_Control_Register_1 (UCBxCTRL1 ) set UTCR for transmit data and reset for receiving data.
7. When an interrupt is enabled, and the GIE bit is set, the interrupt flag will generate an interrupt request.

