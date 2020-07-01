/*
 * RTClib_ds1307.h
 *
 *  Created on: Jul 1, 2020
 *      Author: Md Shakil Anwar
 */


#ifndef INCLUDE_RTCLIB_DS1307_H_
#define INCLUDE_RTCLIB_DS1307_H_

#include <stdint.h>
#include <msp430f2232.h>



typedef void (*rtc_handler)(void); //function pointer for reading/ writing I2C handler which will is defined in library.


typedef struct rtcds1307 {
    uint8_t ss;				///time in second
    uint8_t mm;				///time in minute
    uint8_t hh;				///time in hour
    uint8_t d;				///day in date
    uint8_t m;				///month in date
    uint16_t y;				///year in date
    char RTC_set_data[8];		///global variable to transmit I2C data.
    rtc_handler read;			///I2C read handler function
    rtc_handler write;			///I2C write handler function
} RTCds1307;

void RTC_init(RTCds1307 *RTC);				///Initialization of rtc with all I2C baud rate and all other function.

void RTCds1307_read(void);				/// Function to read RTC
void RTCds1307_write(void);				/// Function to write in RTC

#endif /* INCLUDE_RTCLIB_DS1307_H_ */
