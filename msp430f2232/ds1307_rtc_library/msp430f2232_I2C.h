/*
 * msp430f2232_I2C.h
 *
 *  Created on: Jun 26, 2020
 *      Author: Md Shakil Anwar
 */

/*********************************************************************************
 *This library is only for msp430f2232 I2C/ Two Wire Synchronous communication communication
 * Default sync speed 100KHz for sync_clock_speed = 0;
 * if You don't want to define any value follow the following defaul list
 * TXByteCounter =1;
 *********************************************************************************
 */


#ifndef INCLUDE_MSP430F2232_I2C_H_
#define INCLUDE_MSP430F2232_I2C_H_

#include <stdint.h>
#include <msp430f2232.h>




void initiate_I2C_master(char slaveAddress, uint8_t sync_clock_speed);	/// Initiation of communication only need 7 bit salve address and SPI Communication speed
void I2C_transmit_master(int TXByteCounter, char *I2C_dataToSend);		/// To transmit data length of data and data pointer need to be assigned
unsigned char I2C_receive_master(void);									/// this function receives 8 bit data in output whenever it is called

__interrupt void USCIAB0RX_ISR(void);

__interrupt void USCIAB0TX_ISR(void);


#endif /* INCLUDE_MSP430F2232_I2C_H_ */
