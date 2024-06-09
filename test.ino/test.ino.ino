#include <EEPROM.h>

#define APP_NUM 4 // Number of apparatus.
#define GREEN_DELAY 4000 // Green light delay
#define YELLOW_DELAY 1000 // Yellow light delay
#define MAX_DELAY 12000 // Maximum green light delay
#define DELAY_CHANGE 4000 // Ultrasonic function delay
#define CAR_COUNT_THRESHOLD 11 // Car count threshold for changing delays

// Define the analog pins used as digital pins
const int trigPin = 14; // A0
const int echoPin = 15; // A1
const int redLightPin = 16;

// Traffic lights
int green[APP_NUM] = {2, 5, 8, 11};
int delays[APP_NUM] = {GREEN_DELAY, GREEN_DELAY, GREEN_DELAY, GREEN_DELAY};
int yellow[APP_NUM] = {3, 6, 9, 12};
int red[APP_NUM] = {4, 7, 10, 13};
int data[4] = {0, 0, 0, 0};
bool receivedData = false;

// Controllers
int turn = 0;
int prevTurn = -1;  // Initialize to -1 to ensure first print happens
int curr = 0;

unsigned long previousMillis = 0;  // Stores the last time the light was updated
unsigned long previousYellowMillis = 0; // Stores the last time the yellow light was updated
bool isYellowPhase = false;  // Indicates if it's the yellow light phase

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

void resetAll() {
  resetEEPROM();
  for (int i = 0; i < APP_NUM; i++) {
    delays[i] = GREEN_DELAY;
  }
  Serial.println("reseting");
}

void setup() {
  // Initialize pins
  for (uint8_t i = 0; i < APP_NUM; i++) {
    pinMode(green[i], OUTPUT);
    pinMode(yellow[i], OUTPUT);
    pinMode(red[i], OUTPUT);
  }
  Serial.begin(9600);
  delay(50);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLightPin, OUTPUT);

  // Read data from EEPROM
  for (int i = 0; i < 4; i++) {
    data[i] = EEPROM.read(i);
  }

  for (int i = 0; i < 4; i++) {
    delays[i] = readIntFromEEPROM(i * 2 + 4);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Ultrasonic sensor code
  long duration, distance;

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distance = duration * 0.034 / 2;

  // If distance is less than 6 cm, turn on the red light
  if (distance < 6) {
    digitalWrite(redLightPin, HIGH);
  } else {
    digitalWrite(redLightPin, LOW);
  }

  // Print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (Serial.available()) {
    // Read the incoming data
    String receivedData = Serial.readString();

    // Process the received data
    int arraySize = 4;  // Modify according to the size of your array
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

    // Send a response bac  k to Python if data was received
    if (receivedData) {
      Serial.println("Data received successfully");
      receivedData = false;
    }
  }

  if (turn != prevTurn) {
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
    prevTurn = turn;  // Update prevTurn
  }

  if (!isYellowPhase) {
    if (currentMillis - previousMillis >= delays[turn]) {
      // Change to yellow phase
      digitalWrite(green[turn], LOW);
      digitalWrite(yellow[turn], HIGH);
      isYellowPhase = true;
      previousYellowMillis = currentMillis;

      // Calculate the delay for the next apparatus
      int next = (turn + 1) % 4;
      int ultra_delay = DELAY_CHANGE;
      if (data[next] <= CAR_COUNT_THRESHOLD) {
        ultra_delay = 0;
      }

      if (ultra_delay == 0 && delays[next] > GREEN_DELAY) {
        delays[next] -= DELAY_CHANGE;
      } else {
        if (ultra_delay != 0 && delays[next] < MAX_DELAY) {
          delays[next] += DELAY_CHANGE;
        }
      }

      for (int i = 0; i < 4; i++) {
        writeIntToEEPROM(i * 2 + 4, delays[i]);
      }
    }
  } else {
    if (currentMillis - previousYellowMillis >= YELLOW_DELAY) {
      // End yellow phase, switch to next light
      digitalWrite(yellow[turn], LOW);
      turn = (turn + 1) % 4;
      digitalWrite(red[turn], LOW);
      digitalWrite(green[turn], HIGH);
      previousMillis = currentMillis;
      isYellowPhase = false;
    }
  }

  // Turn on red lights for all non-active apparatus
  for (uint8_t i = 0; i < APP_NUM; i++) {
    if (i != turn) {
      digitalWrite(red[i], HIGH);
    } else if (!isYellowPhase) {
      digitalWrite(green[i], HIGH);
    }
  }
}
