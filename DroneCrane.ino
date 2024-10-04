#include <SoftwareSerial.h>
#include <ESP32Servo.h> // Include ESP32Servo library

Servo myservo; // Initialize Servo object for standard servo
Servo continuousServo; // Initialize Servo object for continuous servo

void setup() {
  Serial.begin(57600); // Begin serial communication
  myservo.attach(2); // Attach standard servo to GPIO pin 2
  continuousServo.attach(11); // Attach continuous servo to GPIO pin 11
}

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read from serial
    String data = Serial.readStringUntil('\n'); // Read data until newline character

    if (data.startsWith("+RCV=")) { // Check if the line starts with "+RCV="
      data.remove(0, 5); // Remove "+RCV=" from the beginning

      // Parse the data using comma as delimiter
      int comma1 = data.indexOf(','); // Find the first comma
      int comma2 = data.indexOf(',', comma1 + 1); // Find the second comma
      int comma3 = data.indexOf(',', comma2 + 1); // Find the third comma

      // Extract values
      String commandOrAngle = data.substring(comma2 + 1, comma3); // This can be a command or angle
      // String signalInfo = data.substring(comma3 + 1); // Signal info, not used in this code

      // Check if commandOrAngle is a control command for the continuous servo
      if (commandOrAngle == "u") {
        continuousServo.write(0); // Set continuous servo to full speed in one direction
      } else if (commandOrAngle == "s") {
        continuousServo.write(89); // Stop the continuous servo
      } else if (commandOrAngle == "d") {
        continuousServo.write(180); // Set continuous servo to full speed in the opposite direction
      } else {
        // Otherwise, treat commandOrAngle as an angle for the standard servo
        int angle = commandOrAngle.toInt();
        if (angle >= 0 && angle <= 180) { // Ensure the angle is within the valid range
          myservo.write(angle);
        }
      }
    }
  }
}
