
// FOR ESP01 with PCF8574

#include <Wire.h>
#include <NewPing.h>

// Pin definitions for the ultrasonic sensor
#define TRIG_PIN 2    // GPIO2 for TRIG
#define ECHO_PIN 3    // GPIO3 for ECHO
#define MAX_DISTANCE 400  // Maximum distance (in cm) for the sensor to measure

// Create an instance of the NewPing library
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// I2C address for PCF8574
#define PCF8574_ADDR 0x27  // Default I2C address for PCF8574 (adjust if needed)

// Initialize the I2C communication
void setup() {
  // Start I2C communication (SDA = GPIO0, SCL = GPIO1)
  Wire.begin(0, 1); // ESP01 uses GPIO0 (SDA) and GPIO1 (SCL)

  // Start serial communication for debugging
  Serial.begin(115200);

  // Initialize the PCF8574 (we don't need any specific initialization, just use I2C functions)
  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0x00);  // Set all pins to low initially
  Wire.endTransmission();
}

void loop() {
  // Get the distance from the ultrasonic sensor
  unsigned int distance = sonar.ping_in();

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
    Wire.beginTransmission(PCF8574_ADDR);
    Wire.write(segments);  // Send the segment pattern
    Wire.endTransmission();

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
