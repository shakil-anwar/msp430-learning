# I2C library for MSP430F2232

## Functions
This library has Three functions.

#### [Function 1] takes two input and has no output. It initializes I2C Communication. Inputs are:
* slave address
* clock speed of I2C communication
````
void initiate_I2C_master(char slaveAddress, uint8_t sync_clock_speed);
````

#### [Function 2] takes two input and has no output. It sends data through I2C. Inputs are:
* length of data
* data pointer
````
void I2C_transmit_master(int TXByteCounter, char *I2C_dataToSend);	
````
#### [Function 2] takes no input and has an output of type (unsigned char). It read one byte data at a time and returns at output.

````
unsigned char I2C_receive_master(void);
````
