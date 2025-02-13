# Wiring Diagram

## Components
1. **OLED Display (128x64)**
   - Pins: VCC, GND, SCL, SDA
2. **Keypad (4x4)**
   - Pins: R1, R2, R3, R4, C1, C2, C3, C4
3. **Buttons (3)**
   - Pins: Button 1 (Green), Button 2 (Blue), Button 3 (White)
4. **NeoPixel LEDs (3 LEDs)**
   - Pins: VCC, GND, DIN (data input)

## ESP32 Pin Assignments

### **OLED Display**
| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC      | 3.3V      |
| GND      | GND       |
| SCL      | GPIO22    |
| SDA      | GPIO21    |

### **Keypad (4x4)**
| Keypad Pin | ESP32 Pin |
|------------|-----------|
| R1         | GPIO26    |
| R2         | GPIO25    |
| R3         | GPIO33    |
| R4         | GPIO32    |
| C1         | GPIO13    |
| C2         | GPIO12    |
| C3         | GPIO14    |
| C4         | GPIO27    |

### **Buttons**
| Button     | ESP32 Pin |
|------------|-----------|
| Button 1 (Green) | GPIO5     |
| Button 2 (Blue)  | GPIO18    |
| Button 3 (White) | GPIO19    |

(Note: Connect one pin of each button to the respective GPIO pin and the other pin to GND.)

### **NeoPixel LEDs**
| NeoPixel Pin | ESP32 Pin |
|--------------|-----------|
| VCC          | 3.3V      |
| GND          | GND       |
| DIN          | GPIO27    |



## Esp32-cam Wiring
We use FTDI to connect the controller with the computer via usb . the wiring should be as follows : 
| esp32-cam Pin | FTDI Pin |
|--------------|-----------|
| 5V           | VCC       |
| GND          | GND       |
| UOR          | TX        |
| UOT          | RX        |

Then connect the usb to the computer. 

## Wiring Example :
![Wiring Diagram](WiringDiagram.png)



## Power Supply
- Ensure the ESP32 and the esp32-cam are powered via USB or an external 5V power supply.
- Ensure the NeoPixel LEDs are not drawing too much current directly from the ESP32; use an external power source if required.

## Notes
- Add 10k pull-up resistors to the SDA and SCL lines of the OLED display if communication issues arise.



