# UART/SERIAL Communication Library for MSP430G2553

## Funchtions
This library has 5 functions.
````
Serial_begin();
````
this functions initiates serial communication protocol by setting all register with 9600 baud rate.

````
Serial.available();
````
This function has no parameter. 
Gives output = 1 data is available in received buffer.

````
Serial.read();
````
This function has no parameter.
Returns a character from buffer.

````
Serial.print( char *String )
````
This function has one character pointer type parameter.
It takes string as input and send it via UART/SERIAL

````
Serial.println( char *String )
````
This function has one character pointer type parameter.
It takes string as input and send it via UART/SERIAL also added a newline ('/r/n') at the end
