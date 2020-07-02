# RTClib for MSP430F2232
RTC based on the DS1307 chip connected via I2C. 
Include "RTClib_ds1307.h" at your code along with I2C library.
````
#include "RTClib_ds1307.h"
````
## Compatibility
This library is compatible only with 
* **ds1307 module**

## Dependencies
An I2C library is needed 
* [I2C Library for msp430f2232](https://github.com/shakil-anwar/msp430-learning/tree/master/msp430f2232/I2C_library)

## Documentation
This library provides a RTC stuct which is used to read ds1307 RTC and set/reset time in it.
````
typedef struct rtcds1307 {
    /*
    *
    *
    */
} RTCds1307;
````
 In this stuct two function is used by pointer function and has 6 parameter to read data.
 Stucture must be defined as global
 ````
 RTCds1307 [defined_name]
 ````
 
 Then read time from RTC using **name.read()** function
 ````
 [defined_name].read();
````

Then set  time in RTC using **name.write()** function
````
[defined_name].write();
````

The following parameters are used to read time and gives integer output
````
[defined_name].ss       //This is called to read second(s).
[defined_name].mm       //This is called to read minute(s).
[defined_name].hh       //This is called to read hour(s).
[defined_name].d        //This is called to read date of the month.
[defined_name].m        //This is called to read number of month.
[defined_name].y        //This is called to read year.
````


