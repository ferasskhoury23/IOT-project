const int lockPin = 26; // GPIO pin connected to the LED
bool isLocked = true;


void setup() {
  Serial.begin(115200); // Initialize Serial Monitor for debugging
  pinMode(lockPin, OUTPUT); // Set the pin as an output
  lockDoor(); // Start with the door locked
}

void loop() {
  // Simulate locking and unlocking with a delay (for testing)
  delay(5000); // Wait 5 seconds
  toggleLock();
  delay(5000); // Wait 5 seconds
  toggleLock();
}

void lockDoor() {
  digitalWrite(lockPin, HIGH); // Turn LED on
  isLocked = true;
  Serial.println("Door is LOCKED");
}

void unlockDoor() {
  digitalWrite(lockPin, LOW); // Turn LED off
  isLocked = false;
  Serial.println("Door is UNLOCKED");
}

void toggleLock() {
  if (isLocked) {
    unlockDoor();
  } else {
    lockDoor();
  }
}