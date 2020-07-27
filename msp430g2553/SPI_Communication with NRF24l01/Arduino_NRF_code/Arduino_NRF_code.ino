
#include <SPI.h>
#include "nRF24L01.h"

//#define NRF_DEBUG 1

/* RF speed settings -- nRF24L01+ compliant, older nRF24L01 does not have 2Mbps. */
#define RF24_SPEED_250KBPS  0x20
#define RF24_SPEED_1MBPS    0x00
#define RF24_SPEED_2MBPS    0x08
#define RF24_SPEED_MAX      RF24_SPEED_2MBPS
#define RF24_SPEED_MIN      RF24_SPEED_250KBPS
#define RF24_SPEED_MASK     0x28

/* RF transmit power settings */
#define RF24_POWER_7DBM        0x07
    // ^ 7dBm available with SI24R1 Taiwanese knockoff modules
#define RF24_POWER_0DBM        0x06
#define RF24_POWER_MINUS6DBM   0x04
#define RF24_POWER_MINUS12DBM  0x02
#define RF24_POWER_MINUS18DBM  0x00
#define RF24_POWER_MAX         RF24_POWER_0DBM
#define RF24_POWER_MIN         RF24_POWER_MINUS18DBM
#define RF24_POWER_MASK        0x07

/* Available states for the transceiver's state machine */
#define RF24_STATE_NOTPRESENT  0x00
#define RF24_STATE_POWERDOWN   0x01
#define RF24_STATE_STANDBY_I   0x02
#define RF24_STATE_STANDBY_II  0x03
#define RF24_STATE_PTX         0x04
#define RF24_STATE_PRX         0x05
#define RF24_STATE_TEST        0x06

/* IRQ "reasons" that can be tested. */
#define RF24_IRQ_TXFAILED      0x10
#define RF24_IRQ_TX            0x20
#define RF24_IRQ_RX            0x40
#define RF24_IRQ_MASK          0x70
// Bit 7 used to signify that the app should check IRQ status, without
// wasting time in the interrupt vector trying to do so itself.
#define RF24_IRQ_FLAGGED       0x80

/* Queue FIFO states that can be tested. */
#define RF24_QUEUE_TXFULL      RF24_FIFO_FULL
#define RF24_QUEUE_TXEMPTY     RF24_TX_EMPTY
#define RF24_QUEUE_RXFULL      RF24_RX_FULL
#define RF24_QUEUE_RXEMPTY     RF24_RX_EMPTY

#define CSN_EN digitalWrite(10, LOW);
#define CSN_DIS digitalWrite(10, HIGH);
#define CE_EN digitalWrite(9, HIGH);
#define CE_DIS digitalWrite(9, LOW);

uint8_t rf_status, config_rf;

uint8_t addre[5]={0x8B,0xC9,0xC9,0xC4,0xD9}, addr_rx[6];


uint8_t r_reg(uint8_t addr)
{
  uint16_t i;
#ifdef NRF_DEBUG
  Serial.print("Reading Address-------");
#endif
  CSN_EN;
  i = SPI.transfer16(RF24_NOP | ((addr & REGISTER_MASK) << 8));
  rf_status = (uint8_t) ((i & 0xFF00) >> 8);
  CSN_DIS;
  return (uint8_t) (i & 0x00FF);
}

void w_reg(uint8_t addr, uint8_t data)
{
  uint16_t i;
  CSN_EN;
  i = SPI.transfer16( (data & 0x00FF) | (((addr & REGISTER_MASK) | W_REGISTER) << 8) );
  rf_status = (uint8_t) ((i & 0xFF00) >> 8);
  CSN_DIS;
}

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  Serial.println(" SPI Connection ready");
  

}

void loop() {
  if(Serial.available())
  {
    int c = Serial.read();
    if(c =='k')
    {
      nrf_details();      
    }else if(c=='i')
    {
      NRF24_init();
      NRF24_rx_set_pipe_address(1,2,&addre[0]);
      nrf_activate_rx();                        // Start receive mode
    }else if( c == 's')
    {
      NRF24_rx_set_pipe_address(1,2,&addre[0]);
    }else if( c == 'r')
    {
      nrf_activate_rx();                        // Start receive mode
    }else if( c == 'f')
    {
      nrf_flush_rx();                           //flush rx
    }else if( c == 'o')
    {
      Serial.print("FIFO Status: ");
      Serial.println(r_reg(FIFO_STATUS));
    }else if( c == 'g')
    {
      read_data(2);
    }
  }

  if(nrf_rx_available()){
    Serial.println("RX Data Available");
    read_data(2);
  }

}


void NRF24_init(void){
  CE_DIS;

  CSN_EN;
  SPI.transfer(RF24_NOP);       // Checking NRF24
  CSN_DIS;

  delay(200);                   // Wait 200ms for RF transceiver to initialize.

  w_reg(NRF_STATUS, (config_rf & 0x00));    // Clear IRQs
  w_reg(EN_RXADDR, 0x00);                   // Disable RX Addresses
  w_reg(EN_AA, 0x00);                       // Disable Auto Ack
  w_reg(DYNPD, 0x00);                       // Disable Dynamic Payload

  config_rf = 2000;                         //retransmit delay in micro second(us)
  config_rf = (config_rf- 250)/250;
  config_rf <<= 4;
  w_reg(SETUP_RETR, config_rf);
  
  w_reg(RF_SETUP, (RF24_SPEED_1MBPS | RF24_POWER_0DBM));   //setup RF Speed and Power
  config_rf = 120;
  w_reg(RF_CH, (config_rf & 0x7F));         // setup rf channel
  w_reg(SETUP_AW, 0x03);                    // setup address width = 5(0x03)
  w_reg(NRF_STATUS, RF24_IRQ_MASK);         // clear inturrupt
  w_reg(NRF_CONFIG, (1<<EN_CRC) | (1<<CRCO));
  
  nrf_flush_tx();                           //flush tx
  nrf_flush_rx();                           //flush rx
}


void NRF24_rx_set_pipe_address(uint8_t pipe, uint8_t payload_data_size, uint8_t *addr)
{
  config_rf = r_reg(EN_RXADDR);
  
  w_reg(EN_RXADDR, config_rf | (1<<pipe));                   // Enable RX Addresses

  //sending address
  Serial.println("S");
  uint8_t rf_addr = 5;
  int i;
  CSN_EN;                                                   // Setting RF address
  config_rf = SPI.transfer((RX_ADDR_P0 + pipe) | W_REGISTER);
  for(i=rf_addr-1;i>=0;i--){
    SPI.transfer(addr[i]);
  }
  CSN_DIS;
  Serial.println("S");

  w_reg(RX_PW_P0 + pipe, payload_data_size);                   // Enable RX Addresses  
}


void nrf_activate_rx (void)
{
  nrf_flush_rx();                           // Flush RX Buffer
  w_reg(NRF_STATUS, (1<<RX_DR));                 // Clear rx interrupt
  
  CE_DIS;
  config_rf = r_reg(NRF_CONFIG);
  w_reg(NRF_CONFIG, config_rf | (1<<PWR_UP) | (1<<PRIM_RX));
  delay(50);
  CE_EN;
}

void nrf_flush_rx(void)
{
  CSN_EN;                                   //flush rx
  config_rf = SPI.transfer(FLUSH_RX);
  CSN_DIS;
}

void nrf_flush_tx(void)
{
  CSN_EN;                                   //flush tx
  config_rf = SPI.transfer(FLUSH_TX);
  CSN_DIS;
}

void nrf_details(void)
{
  Serial.print("RF Status: ");
  Serial.println(rf_status, BIN);
  
  config_rf = r_reg(NRF_CONFIG);
  Serial.print("NRF config: ");
  Serial.println(config_rf, BIN);

  config_rf = r_reg(EN_RXADDR);
  Serial.print("Enabled RX Address: ");
  Serial.println(config_rf, BIN);

  config_rf = r_reg(SETUP_AW);
  Serial.print("RX Addr Width: ");
  Serial.println(config_rf, BIN);


  int i;
  for(i=1;i<6;i++){
    addr_rx[i]= RF24_NOP;
  }
  addr_rx[0]=(RX_ADDR_P1 & REGISTER_MASK);
  Serial.print("RX Addr: ");
  CSN_EN;
  SPI.transfer(&addr_rx[0],6);
  CSN_DIS;
  for(i=1;i<6;i++){ 
    Serial.print(addr_rx[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  
  config_rf = r_reg(RF_CH);
  Serial.print("RF channel: ");
  Serial.println(config_rf);
  
  config_rf = r_reg(RF_SETUP);
  Serial.print("RF setup: ");
  Serial.println(config_rf, BIN);

  Serial.println("");
  Serial.println("");
  
}

uint8_t nrf_rx_available(void)
{
  return (r_reg(NRF_STATUS) & (1<<RX_DR));
}

void read_data(uint8_t data_size)
{
  int i;
  addr_rx[0]=R_RX_PAYLOAD;
  for(i=1;i<data_size;i++){
    addr_rx[i]= RF24_NOP;
  }
  CSN_EN;
  SPI.transfer(&addr_rx[0],(data_size+1));
  CSN_DIS;
  Serial.println((char)addr_rx[1]);
  Serial.println((char)addr_rx[2]);
  
  w_reg(NRF_STATUS, (1<<RX_DR));                 // Clear rx interrupt
}
