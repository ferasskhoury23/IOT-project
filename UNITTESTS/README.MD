# UNITEST Folder

This folder contains unit tests designed to validate the functionality of individual hardware components in the Smart Doorbell IoT Project. These tests ensure that each hardware component operates as expected before integration into the complete system.

## Subfolders and Tests

1. **CameraWebServer**:  
   Contains unit tests for the ESP32-CAM's camera server functionality. Ensures proper streaming and image capturing.

2. **HCSR04**:  
   Tests for the ultrasonic distance sensor module (HC-SR04). Validates distance measurements and response accuracy.

3. **audio_amplifier_button**:  
   Unit tests for the audio amplifier and button modules. Ensures button interactions and audio amplification work as expected.

4. **audio_test**:  
   Dedicated to testing audio output functionality. Verifies clear and accurate audio generation.

5. **display_128x64_i2c**:  
   Tests for the OLED display module using the I2C communication protocol. Ensures proper display rendering and message updates.

6. **keypad_test**:  
   Tests the functionality of the keypad module. Validates key inputs and their integration with the door lock system.


## Purpose

Running these tests ensures:
- Each hardware component is fully functional.
- Smooth integration with other components in the system.
- Early detection of hardware-related issues.

## Contribution

If you encounter any issues or have suggestions for improving the unit tests, feel free to contribute:
1. Fork the repository.
2. Create a new branch with your changes.
3. Submit a pull request.

---

For additional support, contact [ferasskhoury23@gmail.com].
