#include <Keypad.h>

// Define the rows and columns of the keypad
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Change to 4 if you are using a 4x4 keypad

// Define the keys on your keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Connect keypad ROW0, ROW1, ROW2, ROW3 to GPIO pins
byte rowPins[ROWS] = {13, 27, 12, 14}; // R1, R2, R3, R4 (update if needed)

// Connect keypad COL0, COL1, COL2 to GPIO pins
byte colPins[COLS] = {25, 26, 33}; // C1, C2, C3 (update for your connections)

// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200); // Initialize serial communication
  Serial.println("Keypad Test Start");
}

void loop() {
  char key = keypad.getKey(); // Check for keypress

  if (key) { // If a key is pressed
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }
}
