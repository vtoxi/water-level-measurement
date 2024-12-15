To use the **ATtiny13** microcontroller with a **PCF8574 I/O expander** and an ultrasonic sensor (HC-SR04), the approach is quite similar to the one used for the ESP01. However, there are a few differences in terms of pin mapping and library usage since the ATtiny13 is a smaller microcontroller with fewer pins and no native I2C support.

### **Hardware Overview:**
- The ATtiny13 does not have native I2C support, so we will use **Software I2C** (via the **TinyWireM** library) to communicate with the PCF8574.
- The ultrasonic sensor is connected to the ATtiny13 pins for triggering and echo measurement.
- The PCF8574 will handle the 7-segment display via I2C.

### **Wiring Connections:**

1. **Ultrasonic Sensor (HC-SR04) Wiring:**
   - **VCC** of ultrasonic sensor -> **VCC** (from ATtiny13)
   - **GND** of ultrasonic sensor -> **GND** (from ATtiny13)
   - **TRIG** of ultrasonic sensor -> **PB0** (GPIO0 of ATtiny13)
   - **ECHO** of ultrasonic sensor -> **PB1** (GPIO1 of ATtiny13)

2. **PCF8574 Wiring:**
   - **VCC** of PCF8574 -> **VCC** (from ATtiny13)
   - **GND** of PCF8574 -> **GND** (from ATtiny13)
   - **SDA** of PCF8574 -> **PB4** (SDA pin, Software I2C)
   - **SCL** of PCF8574 -> **PB3** (SCL pin, Software I2C)

### **Updated Code for ATtiny13 with PCF8574 and Ultrasonic Sensor:**

Before proceeding, you will need the following:
- **TinyWireM library**: This library provides Software I2C support for ATtiny microcontrollers.
- **NewPing library**: This library is used to manage the ultrasonic sensor.

#### **Installation of TinyWireM Library:**
1. Open Arduino IDE.
2. Go to **Sketch** -> **Include Library** -> **Manage Libraries**.
3. Search for **TinyWireM** and install it.

#### **Code:**

```cpp
#include <TinyWireM.h>   // Software I2C library for ATtiny13
#include <NewPing.h>

// Pin definitions for the ultrasonic sensor
#define TRIG_PIN 0    // PB0 for TRIG (GPIO0 on ATtiny13)
#define ECHO_PIN 1    // PB1 for ECHO (GPIO1 on ATtiny13)
#define MAX_DISTANCE 400  // Maximum distance (in cm) for the sensor to measure

// I2C address for PCF8574
#define PCF8574_ADDR 0x27  // Default I2C address for PCF8574 (adjust if needed)

// Create an instance of the NewPing library for the ultrasonic sensor
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Function to initialize I2C communication
void setup() {
  // Start I2C communication using TinyWireM
  TinyWireM.begin(); // Initialize software I2C on PB3 (SCL) and PB4 (SDA)
  
  // Start serial communication for debugging (optional, depends on available pins)
  Serial.begin(115200);
  
  // Initialize the PCF8574 (we don’t need special initialization)
  TinyWireM.beginTransmission(PCF8574_ADDR);
  TinyWireM.write(0x00);  // Set all pins to low initially
  TinyWireM.endTransmission();
}

void loop() {
  // Get the distance from the ultrasonic sensor
  unsigned int distance = sonar.ping_cm();

  // Convert distance to string to display on 7-segment
  String distanceStr = String(distance);

  // Display the distance on the 7-segment display via PCF8574
  displayOnPCF8574(distanceStr);

  // Delay for a moment before taking the next measurement
  delay(500);
}

// Function to display data on the 7-segment display through PCF8574
void displayOnPCF8574(String distanceStr) {
  // We will send each character of the distance to the 7-segment display
  for (int i = 0; i < distanceStr.length(); i++) {
    char digit = distanceStr.charAt(i);

    // Send the character to the display (convert to a 7-segment pattern)
    byte segments = get7SegmentPattern(digit);

    // Send the segment data to PCF8574 over I2C
    TinyWireM.beginTransmission(PCF8574_ADDR);
    TinyWireM.write(segments);  // Send the segment pattern
    TinyWireM.endTransmission();

    delay(100); // Delay to allow the display to update
  }
}

// Function to map a digit (0-9) to 7-segment display pattern
byte get7SegmentPattern(char digit) {
  switch(digit) {
    case '0': return 0x3F;  // 0x3F corresponds to segments a, b, c, e, f, g
    case '1': return 0x06;  // 0x06 corresponds to segments c, f
    case '2': return 0x5E;  // 0x5E corresponds to segments a, c, d, e, g
    case '3': return 0x4E;  // 0x4E corresponds to segments a, c, d, f, g
    case '4': return 0x66;  // 0x66 corresponds to segments b, c, d, f
    case '5': return 0x6C;  // 0x6C corresponds to segments a, b, d, f, g
    case '6': return 0x7C;  // 0x7C corresponds to segments a, b, d, e, f, g
    case '7': return 0x07;  // 0x07 corresponds to segments a, c, f
    case '8': return 0x7F;  // 0x7F corresponds to all segments
    case '9': return 0x6F;  // 0x6F corresponds to segments a, b, c, d, f, g
    default: return 0x00;  // Blank for non-numeric characters
  }
}
```

### **Explanation of the Code:**

1. **I2C Communication (TinyWireM):**
   - The `TinyWireM` library is used to simulate I2C on the ATtiny13. It is set up to use **PB3** as SCL and **PB4** as SDA.
   - The `TinyWireM.begin()` function initializes the I2C bus, and `TinyWireM.beginTransmission()` and `TinyWireM.write()` are used to communicate with the PCF8574.

2. **Ultrasonic Sensor:**
   - The ultrasonic sensor is connected to **PB0** (TRIG) and **PB1** (ECHO) on the ATtiny13.
   - The `NewPing` library handles the distance measurement.

3. **7-Segment Display Control:**
   - The `get7SegmentPattern()` function converts each digit (0-9) into a corresponding 7-segment display pattern, which is then sent to the PCF8574 over I2C.

4. **Distance Display:**
   - The distance is measured by the ultrasonic sensor, converted to a string, and then displayed on the 7-segment display using the PCF8574.

### **Important Notes:**

- **Pin Usage:**
  - The ATtiny13 has only a few GPIO pins, so we use **PB0** (GPIO0) for TRIG and **PB1** (GPIO1) for ECHO. **PB3** and **PB4** are used for the I2C communication (SCL and SDA).
  
- **Power Supply:**
  - Make sure to supply the ATtiny13 with **3.3V** or **5V** (depending on the version you're using) and ensure that the PCF8574 and ultrasonic sensor are powered accordingly.

- **TinyWireM Library**:
  - This library is for software-based I2C, which is necessary because the ATtiny13 doesn't have hardware I2C support.

With this setup, you can measure the distance using the ultrasonic sensor and display the result on a 7-segment display controlled by the PCF8574 via I2C.