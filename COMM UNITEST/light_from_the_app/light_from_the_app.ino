#include <WiFi.h>
#include <WebServer.h>


const int lockPin = 26; // GPIO pin connected to the LED
bool isLocked = true;

// WiFi credentials
const char* ssid = "Ferass";
const char* password = "fygu6354";


WebServer server(80);

void setup() {
  Serial.begin(115200); // Initialize Serial Monitor for debugging
  pinMode(lockPin, OUTPUT); // Set the pin as an output


  //connect to wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to Wi-Fi , IP is : ");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/lock", lockHandler);   // Lock route
  server.on("/unlock", unlockHandler); // Unlock route
  server.on("/status", statusHandler); // Status route

  // Start the server
  server.begin();
  Serial.println("Server started");


  // Start with the door locked
  lockDoor(); 
}

void loop() {
  server.handleClient(); // Handle incoming HTTP requests
}

void lockDoor() {
  digitalWrite(lockPin, HIGH); // Turn LED on
  isLocked = true;
  Serial.println("Door is LOCKED");
}

void unlockDoor() {
  digitalWrite(lockPin, LOW); // Turn LED off
  isLocked = false;
  Serial.println("Door is UNLOCKED");
}

void toggleLock() {
  if (isLocked) {
    unlockDoor();
  } else {
    lockDoor();
  }
}


// HTTP Handlers
void lockHandler() {
  lockDoor();
  server.send(200, "text/plain", "Door is LOCKED");
}

void unlockHandler() {
  unlockDoor();
  server.send(200, "text/plain", "Door is UNLOCKED");
}

void statusHandler() {
  String status = isLocked ? "LOCKED" : "UNLOCKED";
  server.send(200, "text/plain", "Door is " + status);
}