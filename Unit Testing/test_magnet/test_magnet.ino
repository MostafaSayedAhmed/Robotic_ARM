// Define pin connections
#define in1Pin D5
#define in2Pin D6
#define enaPin D7

void setup() {

  // Set all the motor control pins to outputs
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);

  digitalWrite(enaPin, HIGH); // Full speed

}

void loop() {
  // Run the motor forward
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  delay(2000); // Run for 2 seconds

  // Stop the motor
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  delay(1000); // Stop for 1 second

  // Run the motor backward
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  // digitalWrite(enaPin, HIGH); // Full speed
  delay(2000); // Run for 2 seconds

  // Stop the motor
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  delay(1000); // Stop for 1 second
}
