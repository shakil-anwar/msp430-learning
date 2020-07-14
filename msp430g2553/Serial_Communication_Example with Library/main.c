/*
 * UART/ Serial Communication Example
 *
 *  Created on: July 14,2020
 *      Author: Shakil Anwar
 *
 *  This code sends a successful String after getting 'a' each time in serail command 
 *  Default Baud rate is set as 9600bps
 */

#include "msp430g2553.h"
#include "UARTA0.h"


char srxBuffer;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop the Watch dog
  P1DIR &= ~BIT3;                     ///Set P1.3 as button input
  P1REN |= BIT3;                      ///Enable Resistor
  P1OUT |= BIT3;                      ///Pull up button

  Serial_begin();
  //---------------- Configuring the LED's ----------------------//

   P1DIR  |=  BIT0 + BIT6;  // P1.0 and P1.6 output
   P1OUT  |=  BIT0 + BIT6;  // P1.0 and P1.6 = 1


   while(1)
   {
       if(Serial.available())
       {
           if(Serial.read() == 'a')
           {
               Serial.println("You have sent a letter successfully");
           }
       }
   }
}
