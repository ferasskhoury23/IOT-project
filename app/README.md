# Smart Doorbell Flutter App

This directory contains the source code for the Smart Doorbell app, built using Flutter. The app integrates with Firebase and the ESP32-based IoT smart doorbell system, providing features like live video streaming, door locking/unlocking, and notifications.

## Features

- **Live Video Feed**: View the live stream from the ESP32-CAM directly within the app.
- **Door Lock/Unlock**: Remotely lock or unlock the door using the app.
- **Notifications**: Receive push notifications for doorbell presses and motion detection.
- **Resident Management**: Homeowners can add or remove residents.
- **Firebase Integration**: Real-time database for managing app and IoT device communication.

## Directory Structure

```
app/
├── android/                # Android-specific files
├── ios/                    # iOS-specific files
├── lib/                    # Main Dart source code
├── test/                   # Unit and widget tests
├── web/                    # Web-specific files (if applicable)
├── windows/                # Windows-specific files (if applicable)
├── macos/                  # macOS-specific files (if applicable)
├── pubspec.yaml            # Flutter project dependencies
├── firebase.json           # Firebase configuration
└── README.md               # This README file
```

## Prerequisites

Before running the app, ensure the following:

1. **Flutter SDK**:
   - Install Flutter by following the [official guide](https://flutter.dev/docs/get-started/install).
2. **Firebase Setup**:
   - Place the `google-services.json` file in `android/app/`.
   - Place the `GoogleService-Info.plist` file in `ios/Runner/`.
3. **Dependencies**:
   - Install project dependencies:
     ```bash
     flutter pub get
     ```

## Running the App

To run the app on your desired platform:

1. For Android:
   ```bash
   flutter run -d android
   ```

2. For iOS:
   ```bash
   flutter run -d ios
   ```

3. For Web:
   ```bash
   flutter run -d web
   ```

## Testing

Run the tests to verify the app functionality:
```bash
flutter test
```

## Notes

- Ensure the `google-services.json` and `GoogleService-Info.plist` files are not committed to the repository to avoid exposing sensitive credentials.
- Add these files manually before building the app.

## Contribution

Contributions are welcome! If you encounter any issues or have ideas for improvements:

1. Fork this repository.
2. Create a new branch for your changes:
   ```bash
   git checkout -b feature-name
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add feature-name"
   ```
4. Push the branch:
   ```bash
   git push origin feature-name
   ```
5. Open a pull request.

---

For more information or assistance, contact [ferasskhoury23@gmail.com].

