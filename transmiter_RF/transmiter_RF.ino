/* 14 ch NRF 24 TRANSMITTER  Tx   transmiter
    GND    ->   GND           
    Vcc    ->   3.3V
    CE     ->   D7            
    CSN    ->   D8           
    CLK    ->   D13          
    MOSI   ->   D11           
    MISO   ->   D12           
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
/*//////////////////////////////////////////////////////*/
#define CSN 8
#define CE  7
#define Debug_mode false
/*Create a unique pipe out. The receiver has to wear the same unique code*/
const uint64_t pipeOut = 0x662266; //IMPORTANT: The same as in the receiver!!!
/*//////////////////////////////////////////////////////*/
RF24 radio(CE, CSN); // select  CSN and CE  pins
/*//////////////////////////////////////////////////////*/
const int swj1 = 0; // switch joystic 1
const int x1 = A0; // x joystic 1
const int y1 = A1; // y joystic 1

const int swj2 = 1; // switch joystic 2
const int x2 = A2; // x joystic 2
const int y2 = A3; // y joystic 2

const int v1 = A4; // variable resistor 1
const int v2 = A5; // variable resistor 2

const int sw1 = 2; // button 1 
const int sw2 = 3; // button 2 
const int sw3 = 4; // button 3 
const int sw4 = 5; // button 4 

//Create a struct to send over NRF24L01
struct MyData {
  byte xjoy1;     // x joystic 1
  byte yjoy1;     // y joystic 1
  byte xjoy2;     // x joystic 2
  byte yjoy2;     // y joystic 2
  byte var1;      // variable resistor 1
  byte var2;      // variable resistor 2
  byte swjoy1;    // switch joystic 1
  byte swjoy2;    // switch joystic 2
  byte btn1;      // button 1 
  byte btn2;      // button 2 
  byte btn3;      // button 3 
  byte btn4;      // button 4 
};
MyData data;

/*//////////////////////////////////////////////////////*/
//This function will only set the value to  default if the connection is lost...
void resetData(){
  data.xjoy1 = 0;
  data.yjoy1 = 0;
  data.xjoy2 = 0;
  data.yjoy2 = 0;
  data.var1 = 0;
  data.var2 = 0;
  data.swjoy1 = 0;
  data.swjoy2 = 0;
  data.btn1 = 0;
  data.btn2 = 0;
  data.btn3 = 0;
  data.btn4 = 0;
}


void setup()
{
  pinMode(swj1, INPUT);
  pinMode(swj2, INPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  pinMode(sw4, INPUT);
  
  Serial.begin(9600);
  if (Debug_mode)
    printf_begin();
  radio.begin();
  radio.setDataRate(RF24_250KBPS); //speed  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.openWritingPipe(pipeOut); //Open a pipe for writing
  radio.openReadingPipe(0, pipeOut); //Open a pipe for reading
  radio.setChannel(108);// Set RF communication channel.
  radio.setPALevel(RF24_PA_MAX); //translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.enableDynamicPayloads(); //This way you don't always have to send large packets just to send them once in a while. This enables dynamic payloads on ALL pipes.
  //radio.disableDynamicPayloads();//This disables dynamic payloads on ALL pipes. Since Ack Payloads requires Dynamic Payloads, Ack Payloads are also disabled. If dynamic payloads are later re-enabled and ack payloads are desired then enableAckPayload() must be called again as well.
  radio.setCRCLength(RF24_CRC_16); // Use 8-bit or 16bit CRC for performance. CRC cannot be disabled if auto-ack is enabled. Mode :RF24_CRC_DISABLED  ,RF24_CRC_8 ,RF24_CRC_16
  radio.setRetries(10, 15);//Set the number of retry attempts and delay between retry attempts when transmitting a payload. The radio is waiting for an acknowledgement (ACK) packet during the delay between retry attempts.Mode: 0-15,0-15
  radio.setAutoAck(true); // Ensure autoACK is enabled
  radio.stopListening();//Stop listening for incoming messages, and switch to transmit mode.
  resetData();

}

/**************************************************/

void loop()
{
  // here change data you want to transmit
  //count++;



  
  //data.digit1 = count;      //fill data to MyData

  data.xjoy1 = analogRead(x1);
  data.yjoy1 = analogRead(y1);
  data.xjoy2 = analogRead(x2);
  data.yjoy2 = analogRead(y2);
  data.var1 = analogRead(v1);
  data.var2 = analogRead(v2);
  data.swjoy1 = digitalRead(swj1);
  data.swjoy2 = digitalRead(swj2);
  data.btn1 = digitalRead(sw1);
  data.btn2 = digitalRead(sw2);
  data.btn3 = digitalRead(sw3);
  data.btn4 = digitalRead(sw4);

  Serial.print("xjoy1 :");
  Serial.print(data.xjoy1);
  Serial.print("\t");

  Serial.print("yjoy1 :");
  Serial.print(data.yjoy1);
  Serial.print("\t");

  Serial.print("xjoy2 :");
  Serial.print(data.xjoy2);
  Serial.print("\t");

  Serial.print("yjoy2 :");
  Serial.print(data.yjoy2);
  Serial.print("\t");
  
  NRF24L01_Transmit();//Transmit MyData
  delay(200);
}

void NRF24L01_Transmit() {
  radio.writeFast(&data, sizeof(MyData));//Transmit Data. use one of this two: write() or writeFast()
  if (Debug_mode)
    radio.printDetails();//Show debug data
  bool OK = radio.txStandBy();//Returns 0 if failed. 1 if success.
  delayMicroseconds(50);
  radio.flush_tx();//Empty all 3 of the TX (transmit) FIFO buffers
}
