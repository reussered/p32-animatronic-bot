// TORSO (Master) - Distance sensor + Mood personality + ESP-NOW broadcaster
// Upload this to the TORSO ESP32-S3

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "test_shared_types.h"

// HC-SR04 ultrasonic sensor pins
#define TRIG_PIN 18
#define ECHO_PIN 19

// Head ESP32 MAC address - GET THIS FROM HEAD DEVICE!
// Run head_test first, it will print its MAC address
// Then update this array with the correct values
uint8_t head_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // CHANGE THIS!

esp_now_peer_info_t peerInfo;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Wait for serial to stabilize
    
    Serial.println("\n\n=== TORSO (Master) - Distance -> Mood Test ===");
    Serial.println("================================================\n");
    
    // Initialize HC-SR04 sensor
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    Serial.println("OK HC-SR04 sensor initialized (TRIG=18, ECHO=19)");
    
    // Initialize WiFi in station mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);
    Serial.print("OK WiFi initialized - Torso MAC: ");
    Serial.println(WiFi.macAddress());
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR ERROR: ESP-NOW init failed!");
        while(1) delay(1000);  // Halt
    }
    Serial.println("OK ESP-NOW initialized");
    
    // Register head as peer
    memcpy(peerInfo.peer_addr, head_mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("ERROR ERROR: Failed to add head peer!");
        Serial.println("  Did you update head_mac[] with the correct MAC address?");
        Serial.println("  Run head_test.cpp first to get the head MAC address!");
        while(1) delay(1000);  // Halt
    }
    Serial.println("OK Head peer registered");
    
    // Initialize global state
    g_Envir.distance_cm = 255;
    g_Envir.target_detected = false;
    g_MOOD.happiness = 0;
    g_MOOD.curiosity = 0;
    g_MOOD.fear = 0;
    
    Serial.println("\n[ROCKET] System ready! Move your face near the sensor...\n");
}

uint8_t measure_distance() {
    // Send trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Measure echo pulse duration
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
    
    if (duration == 0) {
        return 255;  // No detection
    }
    
    // Calculate distance in cm (speed of sound: 343 m/s)
    uint8_t distance_cm = duration * 0.0343 / 2;
    
    return (distance_cm > 255) ? 255 : distance_cm;
}

void broadcast_state(const char *name, void *data, size_t size) {
    mesh_packet_t packet;
    strncpy(packet.name, name, 31);
    packet.name[31] = '\0';
    memcpy(packet.data, data, size);
    packet.size = size;
    
    esp_err_t result = esp_now_send(head_mac, (uint8_t*)&packet, sizeof(packet));
    
    if (result == ESP_OK) {
        Serial.printf("  OK Broadcast '%s' (%d bytes)\n", name, size);
    } else {
        Serial.printf("  ERROR Broadcast '%s' FAILED (error %d)\n", name, result);
    }
}

void loop() {
    // === 1. SENSOR: Measure Distance ===
    uint8_t distance = measure_distance();
    bool changed = (distance != g_Envir.distance_cm);
    
    g_Envir.distance_cm = distance;
    g_Envir.target_detected = (distance < 100);  // Within 1 meter
    
    if (changed) {
        Serial.printf("[RULER] Distance: %3d cm %s\n", 
            distance, 
            g_Envir.target_detected ? "[TARGET] (TARGET DETECTED)" : "");
        
        // Broadcast environment state
        broadcast_state("g_Envir", &g_Envir, sizeof(Environment));
    }
    
    // === 2. PERSONALITY: Update Mood Based on Distance ===
    Mood oldMood = g_MOOD;
    
    if (g_Envir.target_detected) {
        if (distance < 30) {
            // Very close (< 30cm) -> Curious + Happy
            g_MOOD.curiosity = 80;
            g_MOOD.happiness = 60;
            g_MOOD.fear = 0;
        } else if (distance < 60) {
            // Close (30-60cm) -> Moderately curious
            g_MOOD.curiosity = 50;
            g_MOOD.happiness = 30;
            g_MOOD.fear = 10;
        } else {
            // Far (60-100cm) -> Slightly curious
            g_MOOD.curiosity = 20;
            g_MOOD.happiness = 10;
            g_MOOD.fear = 30;
        }
    } else {
        // No target -> Neutral/bored
        g_MOOD.curiosity = 0;
        g_MOOD.happiness = 0;
        g_MOOD.fear = 0;
    }
    
    // Only broadcast if mood changed
    if (memcmp(&g_MOOD, &oldMood, sizeof(Mood)) != 0) {
        Serial.printf("[HAPPY] Mood updated: HAPPY=%+3d, CURIOUS=%+3d, FEAR=%+3d\n",
            g_MOOD.happiness, g_MOOD.curiosity, g_MOOD.fear);
        
        // Broadcast mood state
        broadcast_state("g_MOOD", &g_MOOD, sizeof(Mood));
        Serial.println();
    }
    
    delay(100);  // 10 Hz loop rate
}
