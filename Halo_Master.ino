

// This example demonstrates CmdMessenger's callback  & attach methods
// For Arduino Uno and Arduino Duemilanove board (may work with other)

// Download these into your Sketches/libraries/ folder...

// CmdMessenger library available from https://github.com/dreamcat4/cmdmessenger
#include <CmdMessenger.h>


#include <Wire.h>
#include <MsTimer2.h>
#include <IRremote.h>

// Streaming4 library available from http://arduiniana.org/libraries/streaming/
#include <Streaming.h>

#define NUM_APPLE_COMMANDS 7
unsigned long apple_remote[7]= {
                              0x77E150A2,//up
                              0x77E130A2,//down
                              0x77E190A2,//left
                              0x77E160A2,//right
                              0x77E13AA2,//center
                              0x77E1C0A2,//menu
                              0x77E1FAA2//play/pause
};



int current_mode;

enum mode_type {
      OFF = 1,
      ON  = 2,
      RANDOM_FLASH = 3};
     
int RECV_PIN = 2;
unsigned long apple_code = 0x77E100A2;

IRrecv irrecv(RECV_PIN);

decode_results results;
unsigned long ir_command;


// Mustnt conflict / collide with our message payload data. Fine if we use base64 library ^^ above
char field_separator = ',';
char command_separator = ';';
int sendmessage = 0;
int color = 0;
uint8_t check_status_flag = 0;
char messageBuf[10]=  { 
  '\0' };

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);


#define MAX_LIGHTS 16
const char rainbow[18][3]=   {{0xf,  0,  0},
                              {0xf,  0,0x5},
                              {0xf,  0,0xa},
                              {0xf,  0,0xf},
                              {0xa,  0,0xf},
                              {0x5,  0,0xf},
                              {  0,  0,0xf},
                              {  0,0x5,0xf},
                              {  0,0xa,0xf},
                              {  0,0xf,0xf},
                              {  0,0xf,0xa},
                              {  0,0xf,0x5},
                              {  0,0xf,  0},
                              {0x5,0xf,  0},
                              {0xa,0xf,  0},
                              {0xf,0xf,  0},
                              {0xf,0xa,  0},
                              {0xf,0x5,  0}};
uint8_t light_status[MAX_LIGHTS];


// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// We can define up to a default of 50 cmds total, including both directions (send + recieve)
// and including also the first 4 default command codes for the generic error handling.
// If you run out of message slots, then just increase the value of MAXCALLBACKS in CmdMessenger.h

// Commands we send from the Arduino to be received on the PC
enum
{
  kCOMM_ERROR    = 000, // Lets Arduino report serial port comm error back to the PC (only works for some comm errors)
  kACK           = 001, // Arduino acknowledges cmd was received
  kARDUINO_READY = 002, // After opening the comm port, send this cmd 02 from PC to check arduino is ready
  kERR           = 003, // Arduino reports badly formatted cmd, or cmd not recognised

  // Now we can define many more 'send' commands, coming from the arduino -> the PC, eg
  // kICE_CREAM_READY,
  // kICE_CREAM_PRICE,
  // For the above commands, we just call cmdMessenger.sendCmd() anywhere we want in our Arduino program.

  kSEND_CMDS_END, // Mustnt delete this line
};

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below vv.
// They start at the address kSEND_CMDS_END defined ^^ above as 004
messengerCallbackFunction messengerCallbacks[] = 
{
  change_color,            // 004 in this example
  read_light_color,
  check_status,
  change_all,
  NULL
};
// Its also possible (above ^^) to implement some symetric commands, when both the Arduino and
// PC / host are using each other's same command numbers. However we recommend only to do this if you
// really have the exact same messages going in both directions. Then specify the integers (with '=')


// ------------------ C A L L B A C K  M E T H O D S -------------------------


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

void change_all(){
  int red, green, blue,i;
   char buf[20] = { 
    '\0'   };
  cmdMessenger.copyString(buf, 20); //Get second parameter
  String inString =  String(buf);
 // Serial.print(buf);
  red = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get third parameter
  inString =  String(buf);
 // Serial.print(buf);
  green = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get fourth parameter
  inString =  String(buf);
 // Serial.print(buf);
  blue = inString.toInt();
  
  for(i=0;i<MAX_LIGHTS;i++){
    setColor(i,red,green,blue,0);
  }
}
void change_color(){
  int red, green, blue,violet,channel,status= 0;

  //cmdMessenger.sendCmd(kACK,"Received Change Color Command");

  char buf[20] = { 
    '\0'   };
  cmdMessenger.copyString(buf, 20); //Get first parameter
  String inString =  String(buf);
 // Serial.print(buf);
  channel = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get second parameter
  inString =  String(buf);
 // Serial.print(buf);
  red = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get third parameter
  inString =  String(buf);
 // Serial.print(buf);
  green = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get fourth parameter
  inString =  String(buf);
 // Serial.print(buf);
  blue = inString.toInt();
  inString = "";
  cmdMessenger.copyString(buf, 20); //Get fourth parameter
  inString =  String(buf);
 // Serial.print(buf);
  violet = inString.toInt();

//  Serial.print("Channel: ");
//  Serial.print(channel);
//  Serial.print("Red: ");
//  Serial.print(red);
//  Serial.print(", Green: ");
//  Serial.print(green);
//  Serial.print(", Blue: ");
//  Serial.println(blue);

 
  status = setColor(channel,red,green,blue,violet);
  Serial.print(status);
  sendmessage = 1;
}

void read_light_color(){
  int red, green, blue,channel= 0;
  int status;
  cmdMessenger.sendCmd(kACK,"Received Change Color Command");

  char buf[20] = { 
    '\0'   };
  cmdMessenger.copyString(buf, 20); //Get first parameter
  String inString =  String(buf);
  channel = inString.toInt();
  inString = "";

  Serial.print("Requestion Light Status... ");
  Wire.beginTransmission(channel); // transmit to device #4
  Wire.write(0xAE);
  status=  Wire.endTransmission();    // stop transmitting
  if(status != 0){
    Serial.print("Light Did not respond!\n");
    light_status[channel] = 0;
    return;
  }
  Serial.print("Status requested, response: ");
  Wire.requestFrom(channel, 3);
  
    int c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
    Serial.print(" ");
    c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
    Serial.print(" ");
    c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
    Serial.print(" ");
  
  Serial.print(".\n");
  light_status[channel] =1;

}

void check_light_status(){
  int i,status;
  
  for(i =0; i< MAX_LIGHTS;i++){
     Wire.beginTransmission(i); // transmit to device #4
     Wire.write(0xAE);
      status=  Wire.endTransmission();    // stop transmitting
      if(status == 0){
        light_status[i] = 1;
        //Serial.print(i);
       // Serial.print(" ");
        Wire.requestFrom(i, 3);
          int c = Wire.read(); // receive a byte as character
          c = Wire.read(); // receive a byte as character
          c = Wire.read(); // receive a byte as character
          //Serial.print(c);         // print the character
          //Serial.print(" ");
      
       // Serial.print("\n");
      }else {
        light_status[i] = 0;
      }
  }
  
}

void setStatusFlag(){
 check_status_flag = 1; 
}

void check_status(){
  int i;
  check_light_status();
  for(i =0; i< MAX_LIGHTS;i++){
    Serial.print((int)light_status[i]);
    Serial.print(",");
  }
  Serial.print("\n");
}


// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void arduino_ready()
{
  // In response to ping. We just send a throw-away Acknowledgement to say "im alive"
  cmdMessenger.sendCmd(kACK,"Arduino ready");
}

void unknownCmd()
{
  // Default response for unknown commands and corrupt messages
  cmdMessenger.sendCmd(kERR,"Unknown command");
}

// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------



// ------------------ S E T U P ----------------------------------------------

void attach_callbacks(messengerCallbackFunction* callbacks)
{
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i])
  {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}

void setup() 
{
  int i;
  // Listen on serial connection for messages from the pc
  // Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
  Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB

  // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF, and you wish to remove them
  cmdMessenger.print_LF_CR();   // Make output more readable whilst debugging in Arduino Serial Monitor

  // Attach default / generic callback methods
  cmdMessenger.attach(kARDUINO_READY, arduino_ready);
  cmdMessenger.attach(unknownCmd);

  // Attach my application's user-defined callback methods
  attach_callbacks(messengerCallbacks);


  sendmessage = 0;
  Wire.begin(); // join i2c bus (address optional for master)

  for(i =0; i< MAX_LIGHTS;i++){
    light_status[i] =0; 
  }
  check_status_flag = 0;
//  MsTimer2::set(500, setStatusFlag); // 500ms period
//  MsTimer2::start();
  // blink
  pinMode(13, OUTPUT);
  color = 0;
  ir_command = 0;
  irrecv.enableIRIn(); // Start the receiver
  
  current_mode = RANDOM_FLASH;
}


// ------------------ M A I N ( ) --------------------------------------------

// Timeout handling
long timeoutInterval = 500; // 2 seconds
long previousMillis = 0;
int counter = 0;

void timeout()
{
  if(current_mode == RANDOM_FLASH){
    check_status_flag = 1;
  }
  // blink
  if (counter % 2)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
  counter ++;
  
  if(counter == 4){ //reset the command sequence after 2 seconds
    ir_command = 0;
    counter = 0;
  }
}  

void loop() 
{
   char i;
   int light;
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();
  if(sendmessage == 1) {

    sendmessage = 0;
  }
  
  if (irrecv.decode(&results) ) {
    if ((results.value&apple_code) == apple_code && results.value!=0xFFFFFFFF){
      for(i=0;i<NUM_APPLE_COMMANDS;i++){
        if(results.value == apple_remote[i]){
            ir_command = ir_command <<4;
            ir_command += i+1;
        }
      }
      Serial.println(ir_command, HEX);
    }
    irrecv.resume(); // Receive the next value
  }
  
  if(ir_command == 0x111){
    current_mode = OFF;
    for(i=0;i<MAX_LIGHTS;i++){
      setColor(i,0,0,0,0);
    }
  }
  if(ir_command == 0x222){
    current_mode = ON;
    for(i=0;i<MAX_LIGHTS;i++){
      setColor(i,255,255,255,0);
    }
  }
  if(ir_command == 0x333){
    current_mode = RANDOM_FLASH;
  }
  
//  if(check_status_flag == 1){
//   // check_light_status();
//      //i = random(1,5);
//       
//     for(i=1;i<=4;i++){
//       // if(light_status[i]){
//         color = random(18);
//          setColor(i,(rainbow[color][0]*16),(rainbow[color][1]*16),(rainbow[color][2]*16),0);
//        //} 
//      }
//      color++;
//          if (color == 18) {
//              color = 0;
//          }
//   check_status_flag =0; 
//  }
  // handle timeout function, if any
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
    

  }

  // Loop.
}


