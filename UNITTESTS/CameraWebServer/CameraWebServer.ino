


#include "esp_camera.h"
#include <WiFi.h>

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"


// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "BeSpot9B8C_2.4";
const char* password = "9C029B8C";


void startCameraServer();
static void setupLedFlash(int pin);

void setup() {
  // Delay for stability after power-up
  delay(2000);

  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nStarted setup!");

  // Camera configuration
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // 20MHz XCLK
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // JPEG for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12; // Set quality (lower is better quality)
  config.fb_count = 1; // Default frame buffers

  // Adjust frame buffer count if PSRAM is present
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA; // Limit size without PSRAM
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // Get sensor reference for further settings
  sensor_t* s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);      // Flip vertically
    s->set_brightness(s, 1); // Increase brightness
    s->set_saturation(s, -2); // Decrease saturation
  }
  s->set_framesize(s, FRAMESIZE_QVGA); // Set lower frame size initially

  // Setup LED Flash (if available)
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  // Connect to WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Start the camera web server
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // Main loop does nothing; the web server handles tasks
  delay(10000);
}

void setupLedFlash(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Turn off LED initially
}
