#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

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

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// FirebaseClient objects
WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

// Timers for periodic tasks
unsigned long lastMessageCheck = 0;
unsigned long lastLockStateCheck = 0;
const unsigned long messageInterval = 5000; // Check message every 5 seconds
const unsigned long lockStateInterval = 3000; // Check lock state every 3 seconds

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize OLED display
  if (!display.begin(I2C_ADDRESS, true)) {  // Pass I2C address and reset flag
    Serial.println("OLED initialization failed");
    while (true); // Halt if OLED initialization fails
  }
  display.display();
  delay(1000);
  display.clearDisplay();

  // Initialize Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  ssl.setInsecure();
  initializeApp(client, app, getAuth(noAuth));
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  client.setAsyncResult(result);
}

void showMessage(String message) {
  display.clearDisplay();
  display.setTextSize(1);         // Normal text size
  display.setTextColor(SH110X_WHITE); // White text color
  display.setCursor(0, 0);        // Start at top-left corner
  display.println("Message:");
  display.println(message);       // Print message to display
  display.display();              // Send buffer to display
}

void loop() {
  // Check display_message periodically
  if (millis() - lastMessageCheck >= messageInterval) {
    lastMessageCheck = millis();

    // Read display_message from Firebase
    String message = Database.get<String>(client, "/display_message");
    if (client.lastError().code() == 0) {
      Serial.print("Display message: ");
      Serial.println(message);
      showMessage(message);
    } else {
      Serial.print("Failed to read display_message: ");
      Serial.println(client.lastError().message());
    }
  }

  // Check lock_state periodically
  if (millis() - lastLockStateCheck >= lockStateInterval) {
    lastLockStateCheck = millis();

    // Read lock_state from Firebase
    String lockState = Database.get<String>(client, "/lock_state");
    if (client.lastError().code() == 0) {
      Serial.print("Lock state: ");
      Serial.println(lockState);
    } else {
      Serial.print("Failed to read lock_state: ");
      Serial.println(client.lastError().message());
    }
  }
}
