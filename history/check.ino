// Define the analog pins used as digital pins
const int trigPin = 14; // A0
const int echoPin = 15; // A1
const int redLightPin = 16;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLightPin, OUTPUT);
}

void loop() {
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

  // If distance is less than 20 cm, turn on the red light
  if (distance < 6) {
    digitalWrite(redLightPin, HIGH);
  } else {
    digitalWrite(redLightPin, LOW);
  }
  
  // Print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Wait 1 second before the next measurement
//  delay(1000);
}
