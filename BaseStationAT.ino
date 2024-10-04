#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp32-hal-log.h>
#include <esp_event.h>

// WiFi credentials
const char* ssid = "Grimdropper";
const char* password = "letmein21";

// IP configuration for the access point
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Initialize WebServer and Servo objects
WebServer server(80);
Servo myservo;

// Function prototypes
void handleRoot();
void handleSet();
void handleVoltage();
void handleSignal();
float readVoltage();
int getSignalStrength();

void setup() {
  Serial.begin(57600);

  // Attach servo to GPIO pin 2
  myservo.attach(2);

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSet);
  server.on("/voltage", HTTP_GET, handleVoltage); // New route for voltage reading
  server.on("/signal", HTTP_GET, handleSignal); // New route for signal strength
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// Handle requests to the root URL
void handleRoot() {
  // HTML page to control servo and display voltage
  String page = "<!DOCTYPE html><html><head><title>GrimDropper</title></head><body style=\"background-color: black; color: white;\">";
  page += "<h1 style=\"color: red; text-align: center;\">Drop !!ZONE!! Console</h1>";
  page += "<div id=\"voltage\"></div>";
  page += "<div id=\"signal\"></div>"; // Display signal strength
  page += "<button onclick=\"setServo(0)\" style=\"background-color: green; color: black;\">Load</button>";
  page += "<button onclick=\"setServo(30)\" style=\"background-color: blue; color: black;\">Drop 1</button>";
  page += "<button onclick=\"setServo(50)\" style=\"background-color: blue; color: black;\">Drop 2</button>";
  page += "<button onclick=\"setServo(75)\" style=\"background-color: blue; color: black;\">Drop 3</button>";
  page += "<button onclick=\"setServo(95)\" style=\"background-color: blue; color: black;\">Drop 4</button>";
  page += "<button onclick=\"setServo(135)\" style=\"background-color: blue; color: black;\">Drop 5</button>";
  page += "<button onclick=\"setServo(180)\" style=\"background-color: red; color: white;\">Un-Load</button>";
  page += "<script>"
          "function setServo(degrees) {"
          "var xhr = new XMLHttpRequest();"
          "xhr.open('GET', '/set?deg=' + degrees, true);"
          "xhr.send();}"
          "function updateVoltage() {"
          "var voltageDiv = document.getElementById('voltage');"
          "var xhr = new XMLHttpRequest();"
          "xhr.onreadystatechange = function() {"
          "if (xhr.readyState == 4 && xhr.status == 200) {"
          "voltageDiv.innerHTML = 'Current Voltage: ' + xhr.responseText;"
          "}"
          "};"
          "xhr.open('GET', '/voltage', true);"
          "xhr.send();"
          "}"
          "function updateSignal() {" // Add function to update signal strength
          "var signalDiv = document.getElementById('signal');"
          "var xhr = new XMLHttpRequest();"
          "xhr.onreadystatechange = function() {"
          "if (xhr.readyState == 4 && xhr.status == 200) {"
          "signalDiv.innerHTML = 'Signal Strength: ' + xhr.responseText + ' dB';"
          "}"
          "};"
          "xhr.open('GET', '/signal', true);"
          "xhr.send();"
          "}"
          "setInterval(updateVoltage, 1000);"
          "setInterval(updateSignal, 1000);" // Call updateSignal function at the same interval as updateVoltage
          "</script>";
  page += "<style>button:active { background-color: green !important; }</style>";  // CSS for active buttons
  page += "</body></html>";

  server.send(200, "text/html", page);
}

// Handle requests to set servo angle
void handleSet() {
  // Get degree parameter from URL
  if (server.hasArg("deg")) {
    int degrees = server.arg("deg").toInt();
    if (degrees >= 0 && degrees <= 180) {
      // Set servo angle if it's within valid range
      myservo.write(degrees);
      server.send(200, "text/plain", "Servo set to " + String(degrees) + " degrees");
      
      // Determine which command to send based on servo position
      String command;
      if (degrees == 0) {
        command = "AT+SEND=1,1,0"; // Load
      } else if (degrees == 30) {
        command = "AT+SEND=1,2,30"; // Drop 1
      } else if (degrees == 50) {
        command = "AT+SEND=1,2,50"; // Drop 2
      } else if (degrees == 75) {
        command = "AT+SEND=1,2,75"; // Drop 3
      } else if (degrees == 95) {
        command = "AT+SEND=1,2,95"; // Drop 4
      } else if (degrees == 135) {
        command = "AT+SEND=1,3,135"; // Drop 5
      } else if (degrees == 180) {
        command = "AT+SEND=1,3,180"; // Un-Load
      }
      
      // Send the command over serial
      if (!command.isEmpty()) {
        Serial.println(command);
      }
    } else {
      // Send error response for invalid degree value
      server.send(400, "text/plain", "Invalid degree value");
    }
  } else {
    // Send error response if degree parameter is missing
    server.send(400, "text/plain", "Missing degree parameter");
  }
}

// Handle requests for current voltage reading
void handleVoltage() {
  float voltage = readVoltage();
  server.send(200, "text/plain", String(voltage));
}

// Handle requests for current signal strength
void handleSignal() {
  int signalStrength = getSignalStrength();
  server.send(200, "text/plain", String(signalStrength));
}

// Function to read voltage (dummy implementation)
float readVoltage() {
  // Replace this with your actual voltage reading code
  return 5.0; // Dummy value
}

// Function to get signal strength of the first connected client
int getSignalStrength() {
  wifi_sta_list_t wifi_sta_list;
  memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));

  esp_wifi_ap_get_sta_list(&wifi_sta_list);

  if (wifi_sta_list.num) {
    return wifi_sta_list.sta[0].rssi;
  }

  return 0; // Default value if no stations are connected or information retrieval fails
}

