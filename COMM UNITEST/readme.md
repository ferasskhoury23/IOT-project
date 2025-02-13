# COMM UNITEST Folder

This folder contains unit tests designed to validate the communication and integration between the various components of the Smart Doorbell IoT Project. It focuses on ensuring seamless data transfer and functionality across the system.

## Subfolders and Tests

1. **CameraToDatabase**
   - Tests the connection between the ESP32-CAM and the Firebase Realtime Database.
   - Ensures image data is correctly uploaded and stored in the database.

2. **light_from_the_app**
   - Verifies the app's ability to control the door lock status (locked/unlocked) by sending commands to the ESP32.
   - Tests the synchronization between the app and the ESP32.
   - Uses a light as a simulation to a door lock.

3. **light_from_webServer**
   - Focuses on controlling the door lock via commands sent from a web server.
   - Ensures the ESP32 processes and updates the lock state accurately.
   - Uses a light as a simulation to a door lock.

4. **read-show-message**
   - Validates the functionality to send and display messages on the OLED screen connected to the ESP32.
   - Ensures message data is properly transmitted from Firebase and displayed correctly.

5. **RealtimeDatabaseNoAuth**
   - Tests Firebase Realtime Database integration without authentication.
   - Verifies that updates to database fields like `lock_state` and `display_message` are processed correctly.

## Purpose

Running these tests ensures:
- Reliable communication between the hardware, Firebase, and the app.
- End-to-end integration across all components.
- Early detection of any connectivity or synchronization issues.


## Contribution
If you encounter any issues or have suggestions for improving the tests, feel free to contribute:
1. Fork the repository.
2. Create a new branch with your changes.
3. Submit a pull request.

---

For additional support, contact [ferasskhoury23@gmail.com].

