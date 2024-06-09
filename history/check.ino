// Define the analog pins used as digital pins
const int trigPin_1 = 14; // A0
const int echoPin_1 = 15; // A1
const int redLightPin_1 = 16;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin_1, OUTPUT);
  pinMode(echoPin_1, INPUT);
  pinMode(redLightPin_1, OUTPUT);
}

void loop() {
  long duration, distance;
  
  // Clear the trigPin_1
  digitalWrite(trigPin_1, LOW);
  delayMicroseconds(2);
  
  // Set the trigPin_1 HIGH for 10 microseconds
  digitalWrite(trigPin_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_1, LOW);
  
  // Read the echoPin_1, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_1, HIGH);
  
  // Calculate the distance
  distance = duration * 0.034 / 2;

  // If distance is less than 20 cm, turn on the red light
  if (distance < 6) {
    digitalWrite(redLightPin_1, HIGH);
  } else {
    digitalWrite(redLightPin_1, LOW);
  }
  
  // Print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Wait 1 second before the next measurement
//  delay(1000);
}
