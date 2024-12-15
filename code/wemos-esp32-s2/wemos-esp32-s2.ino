
//FOR ESP32S2 Mini

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>
#include <LedControl.h>
// Pin definitions for the ultrasonic sensor on ESP32-S2
#define TRIG_PIN 12    // Set a GPIO pin for TRIG
#define ECHO_PIN 13    // Set a GPIO pin for ECHO
#define MAX_DISTANCE 400  // Maximum distance (in cm) for the sensor to measure

// Create an instance of the NewPing library
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// MAX7219 8-digit 7-segment display setup
#define DATA_IN   15  // Pin for Data In (DIN) on ESP32-S2
#define CS        14  // Pin for Chip Select (CS) on ESP32-S2
#define CLOCK     16  // Pin for Clock (CLK) on ESP32-S2
#define MAX_DEVICES 1 // If you have multiple 7-segment displays, adjust this number

LedControl lc = LedControl(DATA_IN, CLOCK, CS, MAX_DEVICES);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);

  // Initialize the MAX7219 display (clear it)
  for (int i = 0; i < MAX_DEVICES; i++) {
    lc.shutdown(i, false);       // Wake up the display
    lc.setIntensity(i, 8);        // Set brightness (0 is low, 15 is high)
    lc.clearDisplay(i);           // Clear the display at the beginning
  }
}

void loop() {
  String msg = "";
  String unit = "in";
  // Get the distance from the ultrasonic sensor
  unsigned int distance = sonar.ping_in();

  // Display the distance on the MAX7219 8-digit 7-segment display
  // Clear the MAX7219 before displaying new data
  for (int i = 0; i < MAX_DEVICES; i++) {
    lc.clearDisplay(i);
  }

  // Convert distance to string to display on 7-segment
  String distanceStr = String(distance);
  // Reverse the string if the display is upside down
  distanceStr = reverseString(distanceStr); // Reverse string function

  for (int i = 0; i < distanceStr.length(); i++) {
    // Display each character of the distance on the 7-segment display
    lc.setChar(0, i, distanceStr.charAt(i), false); // false to not display leading zeroes
  }

  // Delay for a moment before taking the next measurement
  delay(500);
}

// Function to reverse the string for inverted display
String reverseString(String str) {
  String reversedStr = "";
  for (int i = str.length() - 1; i >= 0; i--) {
    reversedStr += str.charAt(i);
  }
  return reversedStr;
}
