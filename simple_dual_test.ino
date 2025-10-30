// Simple Dual Display Test for ESP32-S3
// Upload this directly to Arduino IDE or ESP-IDF

#include <Arduino.h>
#include <SPI.h>

// Alternative pins - avoiding damaged GPIO 13
#define CS_LEFT   15  // Left eye chip select
#define CS_RIGHT  17  // Right eye chip select (changed from 16)
#define MOSI_PIN  18  // Alternative MOSI pin (changed from 13)
#define SCLK_PIN  14  // Keep SCLK on 14
#define MISO_PIN  12  // Keep MISO on 12

// SPI settings
#define SPI_SPEED 40000000  // 40 MHz

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Dual Display Test Starting...");

// Initialize SPI with specific pins
  SPI.begin(SCLK_PIN, MISO_PIN, MOSI_PIN);

  // Set pin modes
  pinMode(CS_LEFT, OUTPUT);
  pinMode(CS_RIGHT, OUTPUT);
  digitalWrite(CS_LEFT, HIGH);   // Deselect
  digitalWrite(CS_RIGHT, HIGH);  // Deselect

  if (DC_PIN != -1) {
    pinMode(DC_PIN, OUTPUT);
    digitalWrite(DC_PIN, HIGH);
  }

  if (RST_PIN != -1) {
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, HIGH);
  }

  Serial.println("Testing LEFT display...");
  testDisplay(CS_LEFT, "LEFT");

  delay(2000);

  Serial.println("Testing RIGHT display...");
  testDisplay(CS_RIGHT, "RIGHT");

  Serial.println("Test complete! Check your displays.");
}

void testDisplay(int csPin, const char* name) {
  // Select display
  digitalWrite(csPin, LOW);
  delay(10);

  // Send some test data
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

  // Simple test pattern
  for (int i = 0; i < 100; i++) {
    SPI.transfer(i % 256);
  }

  SPI.endTransaction();

  // Deselect display
  digitalWrite(csPin, HIGH);

  Serial.printf("%s display test sent %d bytes\n", name, 100);
}

void loop() {
  // Flash both displays alternately
  digitalWrite(CS_LEFT, LOW);
  delay(500);
  digitalWrite(CS_LEFT, HIGH);

  digitalWrite(CS_RIGHT, LOW);
  delay(500);
  digitalWrite(CS_RIGHT, HIGH);

  Serial.println("Displays flashing...");
  delay(1000);
}