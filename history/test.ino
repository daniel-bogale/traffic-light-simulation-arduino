const int trigPin_1 = 9; // Pin connected to Trig of HC-SR04
const int echoPin_1 = 10; // Pin connected to Echo of HC-SR04

void setup() {
  Serial.begin(9600); // Starts the serial communication
  pinMode(trigPin_1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin_1, INPUT); // Sets the echoPin_1 as an INPUT
}

void loop() {
  long duration;
  int distance;

  // Clears the trigPin
  digitalWrite(trigPin_1, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_1, LOW);
  
  // Reads the echoPin_1, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_1, HIGH);

  // Debugging: Print the duration to see if it makes sense
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.println(" µs");
  
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(1000); // Wait for a second before next reading
}
