#include <SoftwareSerial.h>
#include <ESP32Servo.h> // Include ESP32Servo library

Servo myservo; // Initialize Servo object for standard servo
Servo continuousServo; // Initialize Servo object for continuous servo

const int cutPin = 7;
const int pod1Pin = 12;
const int pod2Pin = 13;

const unsigned long activationDuration = 10000; // Duration to keep pins on (10 seconds)
const unsigned long cooldownDuration = 3000; // Cooldown duration (3 seconds)

unsigned long cutStartTime = 0;
bool cutActive = false;
unsigned long cutCooldownStartTime = 0;

unsigned long pod1StartTime = 0;
bool pod1Active = false;
unsigned long pod1CooldownStartTime = 0;

unsigned long pod2StartTime = 0;
bool pod2Active = false;
unsigned long pod2CooldownStartTime = 0;

void setup() {
  Serial.begin(57600); // Begin serial communication
  myservo.attach(2); // Attach standard servo to GPIO pin 2
  continuousServo.attach(11); // Attach continuous servo to GPIO pin 11
  pinMode(cutPin, OUTPUT); // Set cut pin as output
  pinMode(pod1Pin, OUTPUT); // Set POD1 pin as output
  pinMode(pod2Pin, OUTPUT); // Set POD2 pin as output
  digitalWrite(cutPin, LOW); // Ensure cut pin is initially low
  digitalWrite(pod1Pin, LOW); // Ensure POD1 pin is initially low
  digitalWrite(pod2Pin, LOW); // Ensure POD2 pin is initially low
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    if (data.startsWith("+RCV=")) {
      data.remove(0, 5);
      int comma1 = data.indexOf(',');
      int comma2 = data.indexOf(',', comma1 + 1);
      int comma3 = data.indexOf(',', comma2 + 1);
      String commandOrAngle = data.substring(comma2 + 1, comma3);

      if (commandOrAngle == "u") {
        continuousServo.write(0);
        Serial.println("Continuous servo set to full speed forward");
      } else if (commandOrAngle == "s") {
        continuousServo.write(89);
        Serial.println("Continuous servo stopped");
      } else if (commandOrAngle == "d") {
        continuousServo.write(180);
        Serial.println("Continuous servo set to full speed reverse");
      } else if (commandOrAngle == "c") {
        if (!cutActive) {
          cutActive = true;
          cutStartTime = millis();
          digitalWrite(cutPin, HIGH);
          Serial.println("Cut pin activated");
        }
      } else if (commandOrAngle == "p1") {
        if (!pod1Active) {
          pod1Active = true;
          pod1StartTime = millis();
          digitalWrite(pod1Pin, HIGH);
          Serial.println("POD1 pin activated");
        }
      } else if (commandOrAngle == "p2") {
        if (!pod2Active) {
          pod2Active = true;
          pod2StartTime = millis();
          digitalWrite(pod2Pin, HIGH);
          Serial.println("POD2 pin activated");
        }
      } else {
        int angle = commandOrAngle.toInt();
        if (angle >= 0 && angle <= 180) {
          myservo.write(angle);
          Serial.println("Standard servo set to angle: " + String(angle));
        } else {
          Serial.println("Invalid angle command");
        }
      }
    }
  }

  // Manage the cut activation and cooldown
  if (cutActive) {
    if (millis() - cutStartTime >= activationDuration) {
      digitalWrite(cutPin, LOW);
      cutActive = false;
      cutCooldownStartTime = millis();
      Serial.println("Cut pin deactivated, cooldown started");
    } else if (millis() - cutCooldownStartTime >= cooldownDuration) {
      cutCooldownStartTime = millis(); // Reset cooldown timer to avoid redundant delays
    }
  }

  // Manage POD1 activation and cooldown
  if (pod1Active) {
    if (millis() - pod1StartTime >= activationDuration) {
      digitalWrite(pod1Pin, LOW);
      pod1Active = false;
      pod1CooldownStartTime = millis();
      Serial.println("POD1 pin deactivated, cooldown started");
    } else if (millis() - pod1CooldownStartTime >= cooldownDuration) {
      pod1CooldownStartTime = millis(); // Reset cooldown timer to avoid redundant delays
    }
  }

  // Manage POD2 activation and cooldown
  if (pod2Active) {
    if (millis() - pod2StartTime >= activationDuration) {
      digitalWrite(pod2Pin, LOW);
      pod2Active = false;
      pod2CooldownStartTime = millis();
      Serial.println("POD2 pin deactivated, cooldown started");
    } else if (millis() - pod2CooldownStartTime >= cooldownDuration) {
      pod2CooldownStartTime = millis(); // Reset cooldown timer to avoid redundant delays
    }
  }
}
