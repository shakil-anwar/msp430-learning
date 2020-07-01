/*
 * RTClib_ds1307.c
 *
 *  Created on: Jul 1, 2020
 *      Author: Md Shakil Anwar
 */

#include "RTClib_ds1307.h"

/* reference to the user-defined RTC setup */
static RTCds1307 *rtcSlave;				///RTC pointer

/**************************************************************************/
/*!
    @brief  Convert a binary coded decimal value to binary. RTC stores time/date values as BCD.
    @param val BCD value
    @return Binary value
*/
/**************************************************************************/
static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }

/**************************************************************************/
/*!
    @brief  Convert a binary value to BCD format for the RTC registers
    @param val Binary value
    @return BCD value
*/
/**************************************************************************/
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

void RTCds1307_read(void);		/// Function to read RTC
void RTCds1307_write(void);		/// Function to write in RTC


/**************************************************************************/
/*!
    @brief  Start I2C for the DS1307 and test succesful connection
    @return True if Wire can find DS1307 or false otherwise.
*/
/**************************************************************************/
void RTC_init(RTCds1307 *RTC)
{
	rtcSlave=RTC;
	rtcSlave->read = RTCds1307_read;		/// point to RTC read function
	rtcSlave->write = RTCds1307_write;		/// pointing to RTC write function
	char i2c_slaveAddress = 0x68;
	initiate_I2C_master(i2c_slaveAddress, 20); 	//intialization of I2C slave address and baud rate
}


void RTCds1307_read(void)
{
	int TXByteCtr = 1;
	char I2C_data_to_send = 0x00;
	I2C_transmit_master(TXByteCtr, &I2C_data_to_send);
	rtcSlave->ss = bcd2bin(I2C_receive_master() & 0x7F);
	rtcSlave->mm = bcd2bin(I2C_receive_master());
	rtcSlave->hh = bcd2bin(I2C_receive_master());
	I2C_receive_master();
	rtcSlave->d = bcd2bin(I2C_receive_master());
	rtcSlave->m = bcd2bin(I2C_receive_master());
	rtcSlave->y = bcd2bin(I2C_receive_master())+2000;
}

/**************************************************************************/
/*!
    @brief  Set the date and time in the DS1307
*/
/**************************************************************************/

void RTCds1307_write(void){
	int TXByteCtr = 8;
	rtcSlave->RTC_set_data[0] = 0x00;
	rtcSlave->RTC_set_data[1] = bin2bcd(rtcSlave->ss);
	rtcSlave->RTC_set_data[2] = bin2bcd(rtcSlave->mm);
	rtcSlave->RTC_set_data[3] = bin2bcd(rtcSlave->hh);
	rtcSlave->RTC_set_data[4] = bin2bcd(0x00);
	rtcSlave->RTC_set_data[5] = bin2bcd(rtcSlave->d);
	rtcSlave->RTC_set_data[6] = bin2bcd(rtcSlave->m);
	rtcSlave->RTC_set_data[7] = bin2bcd(rtcSlave->y - 2000);
	I2C_transmit_master(TXByteCtr, rtcSlave->RTC_set_data);
}
