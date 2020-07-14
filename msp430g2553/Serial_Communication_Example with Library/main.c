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
  volatile unsigned int pinState;


  //---------------- Configuring the LED's ----------------------//

   P1DIR  |=  BIT0 + BIT6;  // P1.0 and P1.6 output
   P1OUT  |=  BIT0 + BIT6;  // P1.0 and P1.6 = 1


   while(1)
   {
       pinState = P1IN & BIT3;
//       if(pinState == 0)
       if(Serial.available())
       {
           if(Serial.read() == 'a')
           {
               Serial.println("Finally Done");
               pinState = P1IN & BIT3;
               while(pinState == 0){
                   pinState = P1IN & BIT3;
               }
               __delay_cycles(5000);
           }
       }
   }


}

