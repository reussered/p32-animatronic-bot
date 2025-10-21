// HEAD (Slave) - GC9A01 display + ESP-NOW receiver
// Upload this to the HEAD ESP32-S3

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "test_shared_types.h"

// GC9A01 display pins (SPI)
#define TFT_CS   8
#define TFT_DC   9
#define TFT_RST  10
#define TFT_CLK  12   // SCK
#define TFT_MOSI 11   // MOSI

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240
#define CENTER_X      120
#define CENTER_Y      120

// ESP-NOW receive callback
void on_data_received(const uint8_t *mac, const uint8_t *data, int len) {
    mesh_packet_t *packet = (mesh_packet_t*)data;
    
    Serial.printf("[SIGNAL] Received '%s' (%d bytes) from ", packet->name, packet->size);
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", mac[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    
    // Update local global memory
    if (strcmp(packet->name, "g_Envir") == 0) {
        memcpy(&g_Envir, packet->data, packet->size);
        Serial.printf("   [RULER] Distance: %d cm %s\n", 
            g_Envir.distance_cm,
            g_Envir.target_detected ? "[TARGET]" : "");
        
    } else if (strcmp(packet->name, "g_MOOD") == 0) {
        memcpy(&g_MOOD, packet->data, packet->size);
        Serial.printf("   [HAPPY] Mood: HAPPY=%+3d, CURIOUS=%+3d, FEAR=%+3d\n",
            g_MOOD.happiness, g_MOOD.curiosity, g_MOOD.fear);
        
        // Render eye expression based on new mood!
        render_eye();
    }
    
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(1000);  // Wait for serial to stabilize
    
    Serial.println("\n\n=== HEAD (Slave) - Eye Expression Test ===");
    Serial.println("==========================================\n");
    
    // Initialize display
    Serial.println("Initializing GC9A01 display...");
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);
    
    // Draw "READY" text
    tft.setTextSize(2);
    tft.setTextColor(GC9A01A_WHITE);
    tft.setCursor(60, 110);
    tft.println("READY");
    
    Serial.println("OK Display initialized (240x240 circular)");
    
    // Initialize WiFi in station mode
    WiFi.mode(WIFI_STA);
    Serial.print("OK WiFi initialized - Head MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("  ^^^ COPY THIS MAC ADDRESS TO torso_test.cpp! ^^^");
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR ERROR: ESP-NOW init failed!");
        while(1) delay(1000);  // Halt
    }
    Serial.println("OK ESP-NOW initialized");
    
    // Register receive callback
    esp_now_register_recv_cb(on_data_received);
    Serial.println("OK Receive callback registered");
    
    Serial.println("\n[ROCKET] System ready, waiting for mesh data from torso...\n");
    
    delay(2000);  // Show "READY" for 2 seconds
    
    // Draw initial neutral eye
    g_MOOD.happiness = 0;
    g_MOOD.curiosity = 0;
    g_MOOD.fear = 0;
    render_eye();
}

void render_eye() {
    unsigned long start = millis();
    
    // Clear screen
    tft.fillScreen(GC9A01A_BLACK);
    
    // === Calculate Eye Properties from Mood ===
    
    // Eye color based on dominant mood
    uint16_t eye_color;
    const char* color_name;
    
    if (g_MOOD.happiness > 40) {
        eye_color = GC9A01A_GREEN;
        color_name = "GREEN";
    } else if (g_MOOD.curiosity > 40) {
        eye_color = GC9A01A_YELLOW;
        color_name = "YELLOW";
    } else if (g_MOOD.fear > 40) {
        eye_color = GC9A01A_RED;
        color_name = "RED";
    } else {
        eye_color = 0x4A69;  // Cyan-blue
        color_name = "BLUE";
    }
    
    // Pupil size based on curiosity (dilates when curious)
    int pupil_radius;
    const char* pupil_desc;
    
    if (g_MOOD.curiosity > 60) {
        pupil_radius = 50;
        pupil_desc = "VERY DILATED";
    } else if (g_MOOD.curiosity > 30) {
        pupil_radius = 35;
        pupil_desc = "DILATED";
    } else {
        pupil_radius = 25;
        pupil_desc = "NORMAL";
    }
    
    // Eye openness based on fear (wide when fearful, squinted when happy)
    int eye_radius = 100;
    const char* eye_desc;
    
    if (g_MOOD.fear > 50) {
        eye_radius = 110;
        eye_desc = "WIDE";
    } else if (g_MOOD.happiness > 50) {
        eye_radius = 80;
        eye_desc = "SQUINTED";
    } else {
        eye_radius = 100;
        eye_desc = "NORMAL";
    }
    
    // === Draw Eye ===
    
    // Sclera (white of eye)
    tft.fillCircle(CENTER_X, CENTER_Y, eye_radius, GC9A01A_WHITE);
    
    // Iris (colored part)
    tft.fillCircle(CENTER_X, CENTER_Y, pupil_radius + 20, eye_color);
    
    // Pupil (black center)
    tft.fillCircle(CENTER_X, CENTER_Y, pupil_radius, GC9A01A_BLACK);
    
    // Highlight (makes it look alive)
    tft.fillCircle(CENTER_X - 15, CENTER_Y - 15, 8, GC9A01A_WHITE);
    
    unsigned long render_time = millis() - start;
    
    Serial.printf("[EYE]  Eye rendered in %lu ms:\n", render_time);
    Serial.printf("   Color: %s, Pupil: %s (%dpx), Eye: %s (%dpx)\n",
        color_name, pupil_desc, pupil_radius, eye_desc, eye_radius);
}

void loop() {
    // ESP-NOW callbacks handle everything!
    // Just keep the loop alive
    delay(10);
}
