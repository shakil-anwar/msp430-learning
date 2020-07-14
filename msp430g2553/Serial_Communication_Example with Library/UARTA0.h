/*
 * UARTA0.h
 *
 *  Created on: July 14,2020
 *      Author: Shakil Anwar
 */

#ifndef UARTA0_H_
#define UARTA0_H_

#include <msp430.h>

#define __UART__RX_BUFFER_LENGTH 10

char __uart_rx_buffer[__UART__RX_BUFFER_LENGTH], __uart_rx_buffer_index, __uart_rx_buffer_read_index;
char __uart_tx_buffer_size, *__uart_tx_buffer;

typedef char (*serial_handler1)(void); //function pointer for reading/ writing UART
//typedef char (*serial_handler2)(void); //function pointer for reading/ writing UART
typedef void (*serial_handler3)(char *); //function pointer for reading/ writing UART

/* ====================================================================
 * I2C Prototype Definitions
 * ==================================================================== */

typedef struct UART_Serial {
    serial_handler1 read;            ///UART read handler function
    serial_handler1 available;       ///UART RX availability function
    serial_handler3 print;           ///UART sending data without line break
    serial_handler3 println;         ///UART sending data with line break
} MSPSerial;

extern MSPSerial Serial;

void __UARTA0__send_serial(void);
void Serial_begin(void);
char __UARTA0__read_serial(void);
char __UARTA0__serial_available(void);
void __UARTA0__serial_print(char *__pt_tx_string);
void __UARTA0__serial_println(char *__pt_tx_string);

#endif /* End of  UARTA0_H_ */
