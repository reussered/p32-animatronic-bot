# ESP32-S3 GC9A01 Display Wiring Test
# Simple test to verify pin connections before full system test

#include <Arduino.h>

// Pin definitions - match your actual wiring (SINGLE LEFT EYE)
#define LEFT_CS   34
#define LEFT_DC   38
#define LEFT_BL   39
#define SHARED_RST 42

#define SPI_MOSI 35
#define SPI_MISO 37
#define SPI_SCLK 36

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ESP32-S3 GC9A01 Display Wiring Test ===");

  // Configure all pins as outputs
  pinMode(LEFT_CS, OUTPUT);
  pinMode(LEFT_DC, OUTPUT);
  pinMode(LEFT_BL, OUTPUT);
  pinMode(RIGHT_CS, OUTPUT);
  pinMode(RIGHT_DC, OUTPUT);
  pinMode(RIGHT_BL, OUTPUT);
  pinMode(SHARED_RST, OUTPUT);

  // SPI pins (should be configured by SPI library, but let's test)
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_SCLK, OUTPUT);

  Serial.println("All pins configured as outputs");

  // Test backlight control (most visible test)
  Serial.println("Testing backlight control...");

  // Turn on left backlight
  digitalWrite(LEFT_BL, HIGH);
  Serial.println("Left backlight ON - check if left display lights up");
  delay(2000);

  // Turn off left, turn on right
  digitalWrite(LEFT_BL, LOW);
  digitalWrite(RIGHT_BL, HIGH);
  Serial.println("Right backlight ON - check if right display lights up");
  delay(2000);

  // Both on
  digitalWrite(LEFT_BL, HIGH);
  Serial.println("Both backlights ON");
  delay(2000);

  // Both off
  digitalWrite(LEFT_BL, LOW);
  digitalWrite(RIGHT_BL, LOW);
  Serial.println("Both backlights OFF");

  Serial.println("Backlight test complete!");
  Serial.println("If backlights work, your wiring is good for basic control");
}

void loop() {
  // Simple blink test
  digitalWrite(LEFT_BL, HIGH);
  digitalWrite(RIGHT_BL, LOW);
  delay(500);

  digitalWrite(LEFT_BL, LOW);
  digitalWrite(RIGHT_BL, HIGH);
  delay(500);
}