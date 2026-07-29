/* 14 ch NRF 24 TRANSMITTER  Rx  reciver  
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
/*Create a unique pipe out. The receiver has towear the same unique code*/
const uint64_t pipeIn = 0x662266; //IMPORTANT: The same as in the receiver!!!
/*//////////////////////////////////////////////////////*/
RF24 radio(CE, CSN); // select  CSN and CE  pins
/*//////////////////////////////////////////////////////*/
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
//This function will only set the value to  0 if the connection is lost...
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
/**************************************************/

void setup()
{
  pinMode(0,OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
//  pinMode(LED_BUILTIN, OUTPUT);
 Serial.begin(9600); //Set the speed to 9600 bauds if you want.
//  //You should always have the same speed selected in the serial monitor
  resetData();
  radio.begin();
  if (Debug_mode)
    printf_begin();
  radio.setDataRate(RF24_250KBPS); //speed  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.openWritingPipe(pipeIn);//Open a pipe for writing
  radio.openReadingPipe(1, pipeIn);//Open a pipe for reading
  radio.openReadingPipe(2, pipeIn);//Open a pipe for reading
  radio.openReadingPipe(3, pipeIn);//Open a pipe for reading
  radio.openReadingPipe(4, pipeIn);//Open a pipe for reading
  radio.openReadingPipe(5, pipeIn);//Open a pipe for reading
  radio.setAutoAck(true); // Ensure autoACK is enabled
  radio.setChannel(108);// Set RF communication channel.
  radio.setPALevel(RF24_PA_MAX); //translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.enableDynamicPayloads(); //This way you don't always have to send large packets just to send them once in a while. This enables dynamic payloads on ALL pipes.
  //radio.disableDynamicPayloads();//This disables dynamic payloads on ALL pipes. Since Ack Payloads requires Dynamic Payloads, Ack Payloads are also disabled. If dynamic payloads are later re-enabled and ack payloads are desired then enableAckPayload() must be called again as well.
  radio.setCRCLength(RF24_CRC_16); // Use 8-bit or 16bit CRC for performance. CRC cannot be disabled if auto-ack is enabled. Mode :RF24_CRC_DISABLED  ,RF24_CRC_8 ,RF24_CRC_16
  radio.setRetries(10, 15);//Set the number of retry attempts and delay between retry attempts when transmitting a payload. The radio is waiting for an acknowledgement (ACK) packet during the delay between retry attempts.Mode: 0-15,0-15
  radio.startListening();//Start listening on the pipes opened for reading.
}
/******Reset the received data to 0 if connection is lost******/
unsigned long lastRecvTime = 0;
void recvData()
{
  while ( radio.available() )//Check whether there are bytes available to be read
  {
    radio.read(&data, sizeof(MyData));//Read payload data from the RX FIFO buffer(s).
    lastRecvTime = millis(); //here we receive the data
  }
}
/**************************************************************/
void loop()
{
  recvData(); //Resive Data
  unsigned long now = millis();
  //Here we check if we've lost signal, if we did we reset the values
  if ( now - lastRecvTime > 1000 ) {
    // Signal lost?
    resetData();
  }
  /*digitalWrite(0,0);
  digitalWrite(1,0);
  digitalWrite(2,0);
  digitalWrite(3,0);
  digitalWrite(4,0);
  digitalWrite(5,0);
  digitalWrite(6,0);
  digitalWrite(9,0);
  digitalWrite(10,0);
  digitalWrite(A0,0);
  digitalWrite(A1,0);
  digitalWrite(A2,0);*/
  analogWrite(3, map(data.xjoy1,0,1023,0,255));
  analogWrite(5, map(data.yjoy1,0,1023,0,255));
  analogWrite(6, map(data.xjoy2,0,1023,0,255));
  analogWrite(9, map(data.yjoy2,0,1023,0,255));
  
  //analogWrite(9, data.var1);
  analogWrite(10, map(data.var2,0,1023,0,255) );
  
  digitalWrite(0,data.swjoy1);
  digitalWrite(1,data.swjoy2);
  digitalWrite(2,data.btn1);
  digitalWrite(4,data.btn2);
  digitalWrite(A0,data.btn3);
  digitalWrite(A1,data.btn4);
  digitalWrite(A2,data.btn4);

  /*Serial.print("swj1:  ");
  Serial.print(data.swjoy1);
  Serial.print("\t\t");
  Serial.print("x joystic 1 : ");
  Serial.print(data.xjoy1);
  Serial.print("\t\t");
  Serial.print("y joystic 1 : ");
  Serial.println(data.yjoy1);
  
  
  Serial.print("swj2:  ");
  Serial.print(data.swjoy2);
  Serial.print("\t\t");
  Serial.print("x joystic 2 : ");
  Serial.print(data.xjoy2);
  Serial.print("\t\t");
  Serial.print("y joystic 2 : ");
  Serial.println(data.yjoy2);
  

  Serial.print("variable resistor 1  : ");
  Serial.print(data.var1);
  Serial.print("\t\t");
  Serial.print("variable resistor 2 : ");
  Serial.println(data.var2);
  

  Serial.print("switch 1  : ");
  Serial.print(data.btn1);
  Serial.print("\t\t");
  Serial.print("switch 2 : ");
  Serial.println(data.btn2);
  Serial.print("\t\t");
  Serial.print("switch 3  : ");
  Serial.print(data.btn3);
  Serial.print("\t\t");
  Serial.print("switch 4 : ");
  Serial.println(data.btn4);
  Serial.println("____________________________________________________________");*/
  

  delay(200);
  if (Debug_mode)
    radio.printDetails();//Show debug data
}
