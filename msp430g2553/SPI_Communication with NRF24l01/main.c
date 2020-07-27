/**
 * main.c
 * 16 jul 2020.
 * Created By: Md Shakil Anwar
 */

#include <msp430.h>
#include "msprf24.h"
#include "nrf_userconfig.h"
#include "stdint.h"

//volatile unsigned int user;



int main(void)
{
    uint8_t addr[5];
    uint8_t buf[32];

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	/*      Setup Button       */
	P1DIR &= ~BIT3;                     ///Set P1.3 as button input
    P1REN |= BIT3;                      ///Enable Resistor
    P1OUT |= BIT3;                      ///Pull up button

    volatile unsigned int pinState, user, config, channel, awr, txCount=0, config_aa, cstatus;

	// Setting Clock
	DCOCTL  = 0;             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;   // Set range
    DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation

    // Red, Green LED used for status
    P1DIR |= 0x01;
    P1OUT &= ~0x01;

    user = 0xFE;

    /* Initial values for nRF24L01+ library config variables */
    rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16-bit
    rf_addr_width      = 5;
    rf_speed_power     = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
    rf_channel         = 120;


    msprf24_init();  // All RX pipes closed by default
    msprf24_set_pipe_packetsize(0, 2);
    msprf24_open_pipe(0, 0);  // Open pipe#0 with Enhanced ShockBurst enabled for receiving Auto-ACKs
        // Note: Pipe#0 is hardcoded in the transceiver hardware as the designated "pipe" for a TX node to receive
        // auto-ACKs.  This does not have to match the pipe# used on the RX side.

    // Transmit to 'rad01' (0x72 0x61 0x64 0x30 0x31)
    msprf24_standby();

    user = msprf24_current_state();
    addr[0] = 0x8B; addr[1] = 0xC9; addr[2] = 0xC9; addr[3] = 0xC4; addr[4] = 0xD9;
//    0x81c001c400 0x81c081c400

    w_tx_addr(addr);
    w_rx_addr(0, addr);  // Pipe 0 receives auto-ack's, autoacks are sent back to the TX addr so the PTX node
                         // needs to listen to the TX addr on pipe#0 to receive them.
    user = msprf24_current_state();
    config = r_reg(RF24_CONFIG);
    config_aa = r_reg(RF24_EN_AA);
    channel = r_reg(RF24_RF_CH);
    awr = r_reg(RF24_SETUP_AW);

    while(1){
        pinState = P1IN & BIT3;
        if(pinState == 0){
//            P1OUT &= ~BIT0; // Red LED off

            user = r_reg(RF24_STATUS);
            config = r_reg(RF24_CONFIG);
            config_aa = r_reg(RF24_EN_AA);
            channel = r_reg(RF24_RF_CH);
            awr = r_reg(RF24_SETUP_AW);

            /* Transmission Data */
            if(buf[0]=='a'){buf[0] = 'b';buf[1] = 'a';}
            else {buf[0] = 'a';buf[1] = 'b';}
            w_tx_payload(2, buf);
            msprf24_activate_tx();
//            cstatus = msprf24_current_state();
            LPM4;

            if (rf_irq & RF24_IRQ_FLAGGED) {
                rf_irq &= ~RF24_IRQ_FLAGGED;

                msprf24_get_irq_reason();
                if (rf_irq & RF24_IRQ_TX){
                    P1OUT ^= BIT0;  // Red LED on
                    txCount++;
                }
                if (rf_irq & RF24_IRQ_TXFAILED){
                    P1OUT &= ~BIT0; // Red LED off
                }

                msprf24_irq_clear(rf_irq);
//                user = msprf24_get_last_retransmits();
            }

            pinState = P1IN & BIT3;
            while(pinState == 0){
                pinState = P1IN & BIT3;
            }
//            P1OUT |= BIT0;  // Red LED on
            __delay_cycles(5000);
        }

    }
	return 0;
}
