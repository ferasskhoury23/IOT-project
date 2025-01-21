#include <WiFi.h>
#include <WebServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


const int lockPin = 26; // GPIO pin connected to the LED
bool isLocked = true;

// WiFi credentials
const char* ssid = "Ferass";
const char* password = "fygu6354";



// OLED Display Settings
#define i2c_Address 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin (not used)
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



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


  // Initialize the OLED Display
  if (!display.begin(i2c_Address)) {
    Serial.println(F("Failed to initialize OLED display"));
    while (1); // Stop if initialization fails
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("Smart Doorbell"));
  display.display();


  // Define server routes
  server.on("/lock", lockHandler);   // Lock route
  server.on("/unlock", unlockHandler); // Unlock route
  server.on("/status", statusHandler); // Status route
  server.on("/display", displayHandler);  // Display message route

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

  // Update OLED. comment if you are not using the display 
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Door is LOCKED"));
  display.display();
}

void unlockDoor() {
  digitalWrite(lockPin, LOW); // Turn LED off
  isLocked = false;
  Serial.println("Door is UNLOCKED");


  // Update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Door is UNLOCKED"));
  display.display();
}
  

/*void toggleLock() {
  if (isLocked) {
    unlockDoor();
  } else {
    lockDoor();
  }
}*/


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

  //update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Door is " + status);
  display.display();
}

//Message Handler
void displayHandler() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Serial.println("Message received: " + message);

    //update OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println(message);
    display.display();

    server.send(200, "text/plain", "Message displayed: " + message);
  } else {
    server.send(400, "text/plain", "No message provided");
  }
}




