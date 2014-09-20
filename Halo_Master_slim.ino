
#include <Wire.h>
#include <MsTimer2.h>
#include <IRremote.h>

// Streaming4 library available from http://arduiniana.org/libraries/streaming/
#include <Streaming.h>


char messageBuf[10]=  { 
  '\0' };

unsigned char checksum(unsigned char * array,int size){
  int i;
  unsigned char sum;

  sum = 0;
  for(i=0;i<size; i++){
    sum += array[i];
  }
  return sum;
}

//Send color return 1 if successful
uint8_t setColor(uint8_t address,uint8_t red,uint8_t green, uint8_t blue,uint8_t violet){
  char status;

  messageBuf[0] = 0xaf;
  messageBuf[1] = red ;             
  messageBuf[2] = green ;    
  messageBuf[3] = blue ;
  messageBuf[4] = violet;
  messageBuf[5] = checksum((unsigned char*)messageBuf,5);

  Wire.beginTransmission(address); // transmit to device #4
  Wire.write((uint8_t*)messageBuf,6);
  status = Wire.endTransmission();    // stop transmitting
  if(status != 0){
    return 0; 
  }

  return 1;
}

int light_array[10][4];


void setup() 
{
  int i;
  // Listen on serial connection for messages from the pc
  // Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
  Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB

  Wire.begin(); // join i2c bus (address optional for master)

}

void loop() 
{
  while (Serial.available() > 0){
    int command = Serial.parseInt();
    Serial.print(command); 
    if(command == 345){
      int num_lights = Serial.parseInt();
      for(int i = 0; i< num_lights; i++){
        int addr,r,g,b;
//        light_array[i][0] = Serial.parseInt();
//        light_array[i][1] = Serial.parseInt();
//        light_array[i][2] = Serial.parseInt();
//        light_array[i][3] = Serial.parseInt();
        addr = Serial.parseInt();
        r = Serial.parseInt();
        g = Serial.parseInt();
        b = Serial.parseInt();
        setColor(addr,r,g,b,0);
      }
//      for(int i = 0; i< num_lights; i++){
//         setColor(light_array[i][0],light_array[i][1],light_array[i][2],light_array[i][3],0);
//      }
    }
  }
  

}


