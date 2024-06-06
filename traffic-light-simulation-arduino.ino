#include <EEPROM.h>

#define APP_NUM     4 // Number of apparatus.
#define GREEN_DELAY 3000 // Green light delay
#define YELLOW_DELAY 1000 // Yellow light delay
#define MAX_DELAY 9000 // Yellow light delay
#define DELAY_CHANGE 3000 // Ultrasonic function delay
#define CAR_COUNT_THRESHOLD 11 // Ultrasonic function delay


//traffic lights
int green[APP_NUM]={2,5,8,11};
int delays[APP_NUM]={GREEN_DELAY,GREEN_DELAY,GREEN_DELAY,GREEN_DELAY};
int yellow[APP_NUM]={3,6,9,12};
int red[APP_NUM]={4,7,10,13};
int data[4] = {0,0,0,0};
bool receivedData = false;

//controllers
int turn=0;
int curr = 0;

void writeIntToEEPROM(int address, int value) {
  EEPROM.write(address, value & 0xFF);
  EEPROM.write(address + 1, (value >> 8) & 0xFF);
}

int readIntFromEEPROM(int address) {
  int lowByte = EEPROM.read(address);
  int highByte = EEPROM.read(address + 1);
  return lowByte | (highByte << 8);
}

void resetEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}
void resetAll(){
  resetEEPROM();
  for (int i = 0; i < APP_NUM; i++) {
    delays[i] = GREEN_DELAY;
  }
  Serial.println("reseting");
}

void setup() {
  // put your setup code here, to run once:
  for (uint8_t i = 0; i < APP_NUM; i++) { //
    pinMode(green[i], OUTPUT);
    pinMode(yellow[i], OUTPUT);
    pinMode(red[i], OUTPUT);
  }
  Serial.begin(9600);
  delay(50);

  //  resetAll();
  // resetEEPROM();

  // Read data from EEPROM
  for (int i = 0; i < 4; i++) {
    data[i] = EEPROM.read(i);
  }

  for (int i = 0; i < 4; i++) {
    delays[i] = readIntFromEEPROM(i * 2 + 4);
  }
}

void loop() {
 
  if (Serial.available()) {
    // Read the incoming data
    String receivedData = Serial.readString();

    // Process the received data
    int arraySize = 4;  // Modify according to the size of your array
    // int receivedArray[arraySize];
    int i = 0;
    int value = 0;

    char *token = strtok(const_cast<char *>(receivedData.c_str()), ",");
    while (token != NULL && i < arraySize) {
      value = atoi(token);
      if (value != 0) {
        data[i] = value;
        EEPROM.write(i, value);  // Store data in EEPROM
      }
      token = strtok(NULL, ",");
      i++;
    }

    // Send a response back to Python if data was received
    if (receivedData) {
      Serial.println("Data received successfully");
      receivedData = false;
    }
  }
    Serial.print("turn");
    Serial.print(",");
    Serial.print(turn);
    Serial.print(",");
    Serial.println(delays[turn]);
    Serial.print("cars");
    Serial.print(",");
    Serial.print(data[0]);
    Serial.print(",");
    Serial.print(data[1]);
    Serial.print(",");
    Serial.print(data[2]);
    Serial.print(",");
    Serial.println(data[3]);
    Serial.println("******");

    
    
   
      // Defualt function like normal trafic lights
    for (uint8_t i = 0; i < APP_NUM; i++) { //
      if(i==turn){
        digitalWrite(green[i], HIGH);
      }
      else{
        digitalWrite(red[i], HIGH);
      }   
    }
    
    
    int next=(turn+1)%4;
    int ultra_delay=DELAY_CHANGE;
    delay(YELLOW_DELAY);
    if(data[next]<=CAR_COUNT_THRESHOLD){
      ultra_delay=0;
    }
    
    if(ultra_delay == 0 and delays[next]>GREEN_DELAY){
      delays[next]-=DELAY_CHANGE;
    }
    else{
      if(ultra_delay != 0 and delays[next]<MAX_DELAY){
          delays[next]=delays[next]+DELAY_CHANGE;
      }
    }

    for (int i = 0; i < 4; i++) {
      writeIntToEEPROM(i * 2 + 4, delays[i]);
    }

    //Changes lights according to various delays
    delay(delays[turn]);
    digitalWrite(green[turn], LOW);
    digitalWrite(yellow[turn], HIGH);
    delay(YELLOW_DELAY);
    digitalWrite(yellow[turn], LOW);
    turn=(turn+1)%4;
    digitalWrite(red[turn], LOW);
    
}
