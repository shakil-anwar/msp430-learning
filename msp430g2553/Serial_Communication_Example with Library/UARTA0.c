/*
 * UARTA0.c
 *
 *  Created on: Jul 14, 2020
 *      Author: Shakil Anwar
 */

#include "UARTA0.h"


/* reference to the globally-defined Serial setup */
MSPSerial Serial;

void __UARTA0__send_serial(void);
char __UARTA0__read_serial(void);
char __UARTA0__serial_available(void);
void __UARTA0__serial_print(char *__pt_tx_string);
void __UARTA0__serial_println(char *__pt_tx_string);

/* Initialization of Serial Protocol */
void Serial_begin(void)
{
    Serial.read = __UARTA0__read_serial;
    Serial.available = __UARTA0__serial_available;
    Serial.print = __UARTA0__serial_print;
    Serial.println = __UARTA0__serial_println;
    //------------- Initialization of RX Buffer Value ------------//

    __uart_rx_buffer_index = 0;
    __uart_rx_buffer_read_index = 0;
    __uart_tx_buffer_size = 1;

    //------------------- Configure the Clocks -------------------//

    if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
     {
        while(1);          // do not load, trap CPU!!
     }

    DCOCTL  = 0;             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;   // Set range
    DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation

    //--------- Setting the UART function for P1.1 & P1.2 --------//

    P1SEL  |=  BIT1 + BIT2;  // P1.1 UCA0RXD input
    P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output

    //------------ Configuring the UART(USCI_A0) ----------------//
    UCA0CTL1  =  UCSWRST;
    UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
    UCA0CTL0  =  0x00;
    UCA0BR0   =  104;                 // 104 From datasheet table-
    UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
    UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
    UCA0STAT &=  ~UCLISTEN;            // loop back mode disabled
    UCA0CTL1 &=  ~UCSWRST;             // Clear UCSWRST to enable USCI_A0

    //---------------- Enabling the interrupts ------------------//


    IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
    __bis_SR_register(GIE);        // Enter LPM0 w/ interrupts
}


void __UARTA0__serial_println(char *__pt_tx_string){
    __uart_tx_buffer = __pt_tx_string;            // copying function pointer parameter to global buffer
    __uart_tx_buffer_size = 0;                    // clearing transmit buffer size
    do{
        __uart_tx_buffer_size++;                  // measuring string size
        __pt_tx_string++;
    }
    while(*__pt_tx_string != '\0');               // checking string end
    __UARTA0__send_serial();                      // sending string

    /*---------- Adding a new line --------------*/

    char __uart_tx_newline[]= {13,10};  //(ASCII 13, or '\r') (ASCII 10, or '\n')
    __uart_tx_buffer = __uart_tx_newline;
    __uart_tx_buffer_size = 2;
    __UARTA0__send_serial();
}


void __UARTA0__serial_print(char *__pt_tx_string){
    __uart_tx_buffer = __pt_tx_string;            // copying function pointer parameter to global buffer
    __uart_tx_buffer_size = 0;                    // clearing transmit buffer size
    do{
      __uart_tx_buffer_size++;                  // measuring string size
      __pt_tx_string++;
    }
    while(*__pt_tx_string != '\0');
    __UARTA0__send_serial();                      // sending string
}

void __UARTA0__send_serial(void)
{
    IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
    while((IFG2 & UCA0TXIFG) == 0);   // Waiting for Enabling UCA0TXIFG Bit
    __bis_SR_register(GIE);        // Enter LPM0 w/ interrupts
}

char __UARTA0__read_serial(void)
{
    if(__uart_rx_buffer_read_index == __uart_rx_buffer_index){
        return 0x00;
    }else{
        if(__uart_rx_buffer_read_index < __UART__RX_BUFFER_LENGTH)
        {
            return __uart_rx_buffer[__uart_rx_buffer_read_index++];
        }else{
            __uart_rx_buffer_read_index = 0;
            return __uart_rx_buffer[__uart_rx_buffer_read_index++];
        }
    }
}

char __UARTA0__serial_available(void){
    if(__uart_rx_buffer_read_index == __uart_rx_buffer_index){
      return 0x00;
    }else{
      return 0x01;
    }
}


//-----------------------------------------------------------------------//
//                Transmit and Receive interrupts                        //
//-----------------------------------------------------------------------//

#pragma vector = USCIAB0TX_VECTOR
__interrupt void TransmitInterrupt(void)
{
    P1OUT  |= BIT0;//light up P1.0 Led on Tx
    UCA0TXBUF = *__uart_tx_buffer;                  // Transmit a byte
    __uart_tx_buffer++;
    __uart_tx_buffer_size--;
    while((IFG2 & UCA0TXIFG) == 0);
    if(__uart_tx_buffer_size == 0){
      IE2 &= ~UCA0TXIE;                  // Enable the Transmit interrupt
      __uart_tx_buffer_size = 1;
    }
    P1OUT  &= ~BIT0;//light down P1.0 Led on Tx
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void ReceiveInterrupt(void)
{
    P1OUT  |= BIT6;     // light up P1.6 LED on RX
    if(__uart_rx_buffer_index < __UART__RX_BUFFER_LENGTH){
      __uart_rx_buffer[__uart_rx_buffer_index++] = UCA0RXBUF;
    }else{
      __uart_rx_buffer_index = 0;
    }
    P1OUT  &= ~BIT6;     // light down P1.6 LED on RX
}

