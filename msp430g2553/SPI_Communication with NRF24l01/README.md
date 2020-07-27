# Configuration for compatibility with nRF24XX
this guide is for data transmission without auto acknowledgement ( [copied from NFR24lxx datasheet](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf) )

## How to setup nRF24L01 to receive from an nRF2401/nRF2402/nRF24E1/nRF24E2:
* Use same CRC configuration as the nRF2401/nRF2402/nRF24E1/nRF24E2
uses
* Set the PRIM_RX bit to 1
* Disable auto acknowledgement on the data pipe that will be addressed
* Use the same address width as the PTX device
* Use the same frequency channel as the PTX device
* Select data rate 1Mbit/s on both nRF24L01 and nRF2401/nRF2402/nRF24E1/nRF24E2
* Set correct payload width on the data pipe that will be addressed
* Set PWR_UP and CE high


## How to setup nRF24L01 to transmit to an nRF2401/nRF24E1:
* Use same CRC configuration as the nRF2401/nRF2402/nRF24E1/nRF24E2 uses
* Set the PRIM_RX bit to 0
* Set the Auto Retransmit Count to 0 to disable the auto retransmit functionality
* Use the same address width as the nRF2401/nRF2402/nRF24E1/nRF24E2 uses
* Use the same frequency channel as the nRF2401/nRF2402/nRF24E1/nRF24E2 uses
* Select data rate 1Mbit/s on both nRF24L01 and nRF2401/nRF2402/nRF24E1/nRF24E2
* Set PWR_UP high
* Clock in a payload that has the same length as the nRF2401/nRF2402/nRF24E1/nRF24E2 is configured to receive
* Pulse CE to send the packet
