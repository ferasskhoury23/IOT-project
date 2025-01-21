#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <Base64.h>  // Include Base64 encoding library
#include "esp_camera.h"


#define WIFI_SSID "ICST"
#define WIFI_PASSWORD "arduino123"


#define DATABASE_URL "https://smart-doorbell-2025-default-rtdb.europe-west1.firebasedatabase.app" // Replace with your Firebase project URL

//Firebase objects
WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

//Camera pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool init_camera(){
  //initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QQVGA; // Use a smaller frame size
  config.jpeg_quality = 15;            // Increase compression
  config.fb_count = 1;
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera initialization failed!");
    return false;
  }
  Serial.println("Camera initialized successfully!");
  return true;
}


void init_firebase(){
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  ssl.setInsecure(); // For unsecured connection
  initializeApp(client, app, getAuth(noAuth)); // Initialize Firebase without authentication
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  client.setAsyncResult(result);
}


void connect_wifi(){
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to Wi-Fi. IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);
  
  connect_wifi();

  //initialize Firebase
  init_firebase();

  //init camera
  if( !init_camera()){
    exit(-1);
  }  
}

void loop() {
  // Capture an image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed!");
    return;
  }

  // Encode image buffer as Base64
  String imageBase64 = base64::encode((uint8_t *)fb->buf, fb->len);

  // Debug: Check Base64 length
  Serial.printf("Base64 length: %d bytes\n", imageBase64.length());

  // Upload the Base64 string to Firebase
  Serial.println("Uploading image to Firebase...");
  if (Database.set<String>(client, "/encoded_image", imageBase64)) {
    Serial.println("Encoded image uploaded to Firebase successfully!");
  }
  else {
    Serial.print("Firebase upload failed: ");
    Serial.println(client.lastError().message());
  }

  // Return the frame buffer to be reused
  esp_camera_fb_return(fb);
  delay(500);

}
