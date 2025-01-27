
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <Keypad.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include "sounds.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define BUTTON1_PIN 5   // Button Green connected to D5 (GPIO5)
#define BUTTON2_PIN 18  // Button Blue connected to D19 (GPIO19)
#define BUTTON3_PIN 19  // Button white connected to D18 (GPIO18)
#define PIN  27      // NeoPixel data pin
#define NUMPIXELS 4  // Number of NeoPixels

// Wi-Fi credentials
#define WIFI_SSID "ICST"
#define WIFI_PASSWORD "arduino123"

// Firebase Realtime Database URL
#define DATABASE_URL "https://smart-doorbell-2025-default-rtdb.europe-west1.firebasedatabase.app"

// I2C OLED display setup (SH1106G, 128x64 resolution)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (-1 if not used)
#define I2C_ADDRESS 0x3C
//#define senderID 110180556964

//////////////////////////////////objects//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NeoPixel strip object
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// OLED display object
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// FirebaseClient objects
WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;
//////////////////////////////////////keypad settings////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Keypad settings
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Keypad layout
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//byte rowPins[ROWS] = {13, 27, 12, 14}; // Connect to the row pinouts of the keypad
//byte colPins[COLS] = {25, 26, 33, 32}; // Connect to the column pinouts of the keypad

byte rowPins[ROWS] = {26, 25, 33, 32}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {13, 12, 14, 27}; // Connect to the column pinouts of the keypad


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

///////////////////////////global Variables//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


String enteredPassword = "";  // Buffer to store the entered password from the keypad
String mainPassword = "";     // Main password fetched from Firebase
std::vector<String> tempPasswords; // vector to store temporary passwords
const int passLength = 6;     // Password length
const int bufferLimit = 10;   // Maximum buffer length
String displayed_message = "";  // Firebase message or NO WIFI (the message to display on the OLED)
String entered_keys = "";       // Keys entered by the user
String door_state = "Locked";   // Door state (Locked/Unlocked)
String status_message = "";

unsigned long lastPasswordFetch = 0;  // Timestamp for the last password fetch
const unsigned long fetchInterval = 3000; // Fetch interval: 5 seconds
const unsigned long autoLockInterval = 10000; // auto lock the door after 10 seconds

unsigned long unlockStartTime = 0;  // Time when the door was unlocked
bool doorUnlocked = false;          // Tracks if the door is currently unlocked

//////////////////////////////////////setup + loop///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  ///////////pixel//////////////////
  setuppixel();
  /////////////////////////////
  //for sound
  setupI2S();


  ///////////buttons//////////////////
  pinMode(BUTTON1_PIN, INPUT_PULLUP); // Button 1 with internal pull-up
  pinMode(BUTTON2_PIN, INPUT_PULLUP); // Button 2 with internal pull-up
  pinMode(BUTTON3_PIN, INPUT_PULLUP); // Button 3 with internal pull-up

  ////////////important initializations/////////////////
  // Connect to Wi-Fi
  connect_wifi();

  // Initialize OLED display
  init_OLED_display();

  // Initialize Firebase
  init_firebase();

  // Initial fetch of the main parameters (lock state, main password, temporary passwords ..) 
  fetchMainPassword();
  fetchTempPasswords();
  fetchLockState();
  fetch_message();
}

void loop() {
  //check Wi-Fi connection
  bool isConnected = (WiFi.status() == WL_CONNECTED);
  displayed_message = isConnected ? displayed_message : "NO WIFI";

  //LED 
  led();
  
  //fetch Firebase data periodically
  if (isConnected && millis() - lastPasswordFetch >= fetchInterval) {
    lastPasswordFetch = millis();
    fetchMainPassword();
    fetchTempPasswords();
    fetchLockState();
    fetch_message();
  }

  //gets a key from the keypad and handles it
  handleKepad();

  //lock the door automatically after 10 seconds if unlocked
  autoLockDoor();
  
  //check Buttons
  checkButtons();

  //update OLED Display
  updateDisplay();
}

//////////////////////init functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_firebase(){
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  ssl.setInsecure();
  initializeApp(client, app, getAuth(noAuth));
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  client.setAsyncResult(result);
}


void init_OLED_display(){
  if (!display.begin(I2C_ADDRESS, true)) {  // Pass I2C address and reset flag
    Serial.println("OLED initialization failed");
    while (true); // Halt if OLED initialization fails
  }
  display.display();
  delay(1000);
  display.clearDisplay();
}


void connect_wifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void setuppixel(){
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void led(){
  if (doorUnlocked) {
    setColor(0, 150, 0);  // Green color
  }
  else {
    setColor(150, 0, 0);  // Red color
  }
  pixels.show();
}

/// @brief Set the color of all NeoPixels
void setColor(int r, int g, int b) {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}


/// @brief Update the OLED display with the current state and messages
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Door State: ");
  display.println(door_state);
  display.print("Message: ");
  display.println(displayed_message);
  display.print("Status: ");
  display.println(status_message);
  display.print("Entered: ");
  display.println(entered_keys);
  display.display();
}

/// @brief Check if any of the buttons are pressed and update the notification accordingly
void checkButtons() {
  if (digitalRead(BUTTON1_PIN) == LOW) { // green button
    status_message = "Doorbell ringing!";
    Serial.println(status_message);

    DoorbellRingSound();
    updateNotification("The Door is ringing!");
    delay(200); // Debounce delay
    resetNotificationAfterDelay();
  }

  if (digitalRead(BUTTON2_PIN) == LOW) { // Blue button
    status_message = "Delivery Arrived Message Has been Sent To The Owner";
    Serial.println(status_message);
    updateNotification("Delivery on the Door");
    delay(200); // Debounce delay
    resetNotificationAfterDelay();
  }

  if (digitalRead(BUTTON3_PIN) == LOW) { // white button
    status_message = "ALERT!!!!";
    Serial.println(status_message);
    updateNotification("Security Alert!");
    delay(200); // Debounce delay
    resetNotificationAfterDelay();
  }
}

/// @brief Toggle the door lock state and update the status message , also records the unlock time.
void toggleDoorLock(){
  if (doorUnlocked) {
      lockDoor(); // Lock the door if it is currently unlocked
      status_message = "Door locked.";
      doorUnlocked = false;
    }
    else {
      status_message = "Access granted.";
      unlockDoor(); // Unlock the door if it is currently locked
      doorUnlocked = true;
      unlockStartTime = millis(); // Record the unlock time
    }
}


/// @brief Check the entered password (locally) and unlock/lock the door accordingly
void checkPassword() {
  if (enteredPassword == mainPassword) {
    CorrectPasswordSound(); // Play sound for correct password
    toggleDoorLock();
  }
  else {
    // Check if entered password matches any temporary password
    bool tempPasswordValid = false;
    for (const String& tempPass : tempPasswords) {
      if (enteredPassword == tempPass) {
        tempPasswordValid = true;
        toggleDoorLock();
        CorrectPasswordSound(); // Play sound for correct password
        break;
      }
    }
    if (!tempPasswordValid) {
      WrongPasswordSound(); // Play sound for wrong password
      status_message = "Access Denied.";
      Serial.println(status_message);
    }
  }
  //clear the buffer after checking
  enteredPassword = ""; 
  entered_keys = "";
}

/// @brief handles keypad input
void handleKepad(){

  char key = keypad.getKey(); // Check if a key is pressed
  if (key) {
    KeyPressedSound();
    if (key == '#') {
      // Submit the entered password only if it's exactly passLength characters long
      if (enteredPassword.length() == passLength) {
        checkPassword();
      } else {
        status_message = "Password must be 6 chars.";
        Serial.println(status_message);
        enteredPassword = ""; // Clear the buffer
        entered_keys = "";
      }
    } else if (key == '*') {
      // Clear the entered password
      resetPasswordSound(); // Play sound for password reset
      status_message = "Password entry cleared.";
      Serial.println(status_message);
      enteredPassword = "";
      entered_keys = "";
    } else {
      // Add the key to the password buffer
      enteredPassword += key;
      entered_keys += key;
      status_message = "Entering Password.";
      Serial.println(status_message);

      // Check if buffer length exceeds the limit
      if (enteredPassword.length() > bufferLimit) {
        status_message = "Buffer limit exceeded.";
        Serial.println(status_message);
        enteredPassword = ""; // Reset the buffer
        entered_keys = "";
      }
    }
  }

}

/// @brief auto locks the door after 10 seconds
void autoLockDoor(){
  if (doorUnlocked && millis() - unlockStartTime >= autoLockInterval) {
    lockDoor();
    doorUnlocked = false;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Unlock the door and update the state in Firebase
void unlockDoor() {
  Serial.println("Unlocking the door...");

  //update local state and OLED display
  doorUnlocked = true;
  door_state = "Unlocked";

  //update Firebase if connected
  if (WiFi.status() == WL_CONNECTED) {
    bool success = Database.set<String>(client, "/lock_state", "unlocked");
    if (success) {
      Serial.println("lock_state updated to unlocked in Firebase.");
    }
    else {
      Serial.print("Failed to update lock_state in Firebase: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    displayed_message = "NO WIFI";
    Serial.println("Wi-Fi not connected. State updated locally only.");
  }
}

/// @brief Lock the door and update the state in Firebase
void lockDoor() {
  Serial.println("Locking the door...");

  // Update local state and OLED display
  doorUnlocked = false;
  door_state = "Locked";

  // Update Firebase if connected
  if (WiFi.status() == WL_CONNECTED) {
    bool success = Database.set<String>(client, "/lock_state", "locked");
    if (success) {
      Serial.println("lock_state updated to locked in Firebase.");
    }
    else {
      Serial.print("Failed to update lock_state in Firebase: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    Serial.println("Wi-Fi not connected. State updated locally only.");
  }
}

/// @brief Update the notification in Firebase
void updateNotification(String notification) {
  if (WiFi.status() == WL_CONNECTED) {
    bool success = Database.set<String>(client, "/notification", notification);
    if (success) {
      Serial.println("Notification updated successfully: " + notification);
    }
    else {
      Serial.print("Failed to update notification: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    Serial.println("Wi-Fi not connected. Skipping notification update.");
  }
}

/// @brief Reset the notification to NONE after a delay
void resetNotificationAfterDelay() {
  delay(4000); // Wait for 4 seconds
  if (WiFi.status() == WL_CONNECTED) {
    bool success = Database.set<String>(client, "/notification", "NONE");
    if (success) {
      Serial.println("Notification reset to NONE successfully.");
    }
    else {
      Serial.print("Failed to reset notification: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    Serial.println("Wi-Fi not connected. Skipping notification reset.");
  }
}

/// @brief Fetch the main password from Firebase
void fetchMainPassword() {
  Serial.println("Fetching main password from Firebase...");
  if (WiFi.status() == WL_CONNECTED) {
    String newPassword = Database.get<String>(client, "/main_password");
    if (client.lastError().code() == 0) {
      mainPassword = newPassword;
      Serial.print("Main password fetched: ");
      Serial.println(mainPassword);
    }
    else {
      Serial.print("Failed to fetch main password: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    Serial.println("Wi-Fi not connected.");
  }
}


/// @brief Fetch the lock state from Firebase
void fetchLockState() {
  Serial.println("Fetching lock state from Firebase...");
  if (WiFi.status() == WL_CONNECTED) {
    String lockState = Database.get<String>(client, "/lock_state");
    if (client.lastError().code() == 0) {
      if (lockState == "unlocked" && !doorUnlocked) {
        Serial.println("Door is unlocked by external source.");
        doorUnlocked = true;
        door_state = "Unlocked";
        unlockStartTime = millis(); // Start the unlock timer
      }
      else if (lockState == "locked" && doorUnlocked) {
        Serial.println("Door is locked by external source.");
        doorUnlocked = false;
        door_state = "Locked";
      }
    }
    else {
      Serial.print("Failed to fetch lock state: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    Serial.println("Wi-Fi not connected. Skipping lock state fetch.");
  }
}


/// @brief Fetch the message to be displayed on the OLED screen
void fetch_message() {
  if (WiFi.status() == WL_CONNECTED) {
    String message = Database.get<String>(client, "/display_message");
    if (client.lastError().code() == 0) {
      displayed_message = message;
      Serial.print("Display message: ");
      Serial.println(displayed_message);
    }
    else {
      Serial.print("Failed to read display_message: ");
      Serial.println(client.lastError().message());
    }
  }
  else {
    displayed_message = "NO WIFI";
    Serial.println("Wi-Fi not connected. Skipping message fetch.");
  }
}


/// @brief Fetch the temporary passwords from Firebase
void fetchTempPasswords() {
  Serial.println("Fetching temporary passwords from Firebase...");
  if (WiFi.status() == WL_CONNECTED) {
    String rawTempPasswords = Database.get<String>(client, "/temp_password");
    if (client.lastError().code() == 0 && rawTempPasswords.length() > 0) {
      tempPasswords.clear(); // Clear the previous temporary passwords

      // Create a JSON document with a size large enough for your Firebase data
      StaticJsonDocument<1024> doc;

      // Parse the JSON string
      DeserializationError error = deserializeJson(doc, rawTempPasswords);

      if (error) {
        Serial.print("Failed to parse temporary passwords: ");
        Serial.println(error.c_str());
        return;
      }

      // Iterate through the JSON object
      for (JsonPair p : doc.as<JsonObject>()) {
        const char* key = p.key().c_str();
        const JsonObject tempPasswordObj = p.value().as<JsonObject>();

        // Skip the placeholder entry
        if (String(key) == "placeholder") {
          continue;
        }

        // Extract and store the password
        if (tempPasswordObj.containsKey("password")) {
          String tempPassword = tempPasswordObj["password"].as<String>();
          tempPasswords.push_back(tempPassword);
          Serial.println("Temporary password added: " + tempPassword);
        }
      }

      Serial.println("Temporary passwords fetched successfully.");
    } else {
      Serial.print("Failed to fetch temporary passwords: ");
      Serial.println(client.lastError().message());
    }
  } else {
    Serial.println("Wi-Fi not connected. Skipping temporary password fetch.");
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////