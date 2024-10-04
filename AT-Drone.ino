#include <SoftwareSerial.h>
#include <ESP32Servo.h>

Servo myservo; // Initialize Servo object

void setup() {
  Serial.begin(57600); // Begin serial communication
  myservo.attach(2); // Attach servo to GPIO pin 2
}

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read from serial
    String data = Serial.readStringUntil('\n'); // Read data until newline character
    if (data.startsWith("+RCV=")) { // Check if the line starts with "+RCV="
      data.remove(0, 5); // Remove "+RCV=" from the beginning
      // Parse the data using comma as delimiter
      int comma1 = data.indexOf(',');
      int comma2 = data.indexOf(',', comma1 + 1);
      int comma3 = data.indexOf(',', comma2 + 1);
      int comma4 = data.indexOf(',', comma3 + 1);
      // Extract channel, number of bits, angle, and signals
      String channelStr = data.substring(0, comma1);
      String bitsStr = data.substring(comma1 + 1, comma2);
      String angleStr = data.substring(comma2 + 1, comma3);
      String signal1Str = data.substring(comma3 + 1, comma4);
      String signal2Str = data.substring(comma4 + 1);
      // Convert strings to integers
      int channel = channelStr.toInt();
      int bits = bitsStr.toInt();
      int angle = angleStr.toInt();
      int signal1 = signal1Str.toInt();
      int signal2 = signal2Str.toInt();
      // Set servo angle
      myservo.write(angle);
    }
  }
}
