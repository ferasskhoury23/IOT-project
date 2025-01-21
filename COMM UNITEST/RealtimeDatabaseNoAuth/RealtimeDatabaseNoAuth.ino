#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>


#define WIFI_SSID "ICST"
#define WIFI_PASSWORD "arduino123"


#define DATABASE_URL "https://smart-doorbell-2025-default-rtdb.europe-west1.firebasedatabase.app"


// FirebaseClient objects
WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

void setup() {
  Serial.begin(115200);

  //connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Set insecure SSL for testing (not recommended for production)
  ssl.setInsecure();

  // Initialize Firebase
  initializeApp(client, app, getAuth(noAuth));
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  client.setAsyncResult(result);
  
  // Test Firebase connection: Write and Read Data
  testFirebase();
}

void testFirebase() {
  // Write data
  Serial.print("Writing to Firebase: ");
  if (Database.set<String>(client, "/test", "Hello from ESP32 4-jan-25")) {
    Serial.println("Write successful!");
  }
  else {
    Serial.print("Failed: ");
    Serial.println(client.lastError().message());
  }

  // Read data
  Serial.print("Reading from Firebase: ");
  String value = Database.get<String>(client, "/lock_state");
  if (client.lastError().code() == 0) {
    Serial.print("Value: ");
    Serial.println(value);
  }
  else {
    Serial.print("Failed: ");
    Serial.println(client.lastError().message());
  }
}


void loop() {
  // Add your real-time updates logic here
}
