# All Hardware - Libraries Used

This directory contains the hardware code for the project, including:
1. **cameraFinal**: Code for the ESP32-CAM.
2. **hardware_code_final**: Code for the ESP32.

Below is a list of all Arduino/ESP libraries used in this project:

## Libraries

### Common Libraries
1. **WiFi.h**
   - For connecting the ESP32 to Wi-Fi networks.
2. **FirebaseClient.h**
   - For interacting with Firebase Realtime Database.
3. **WiFiClientSecure.h**
   - For secure communication over HTTPS.
4. **ArduinoJson.h**
   - For parsing and generating JSON data.
5. **Wire.h**
   - For I2C communication with peripherals.

### OLED Display Libraries
1. **Adafruit_GFX.h**
   - Core graphics library for Adafruit displays.
2. **Adafruit_SH110X.h**
   - For controlling SH1106 OLED displays.

### Keypad Library
1. **Keypad.h**
   - For handling the 4x4 keypad input.

### NeoPixel LEDs
1. **Adafruit_NeoPixel.h**
   - For controlling NeoPixel (WS2812) RGB LEDs.

### Sound
1. **sounds.h**
   - Custom header file for handling sound-related functionalities (like doorbell sounds).

## Installation Instructions
To ensure the code compiles and runs correctly, make sure all the above libraries are installed in your Arduino IDE or PlatformIO environment.

### Arduino IDE
1. Open Arduino IDE.
2. Go to `Sketch -> Manage Libraries`.
3. Search for each library listed above and install it.

## Notes
- Ensure you are using compatible versions of the libraries with your ESP32/ESP32-CAM boards.
- For custom libraries like sounds.h, ensure the file is included in the correct directory within the project.