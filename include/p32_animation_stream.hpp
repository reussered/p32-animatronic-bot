// P32 Animation Stream Protocol
// Sends computed animation frames to PC for display simulation

#ifndef P32_ANIMATION_STREAM_H
#define P32_ANIMATION_STREAM_H

#include "esp_err.h"
#include "p32_eye_display.hpp"
#include <stdint.h>

// Animation stream protocol
typedef enum {
    STREAM_PROTOCOL_SERIAL = 0,     // Send over UART/USB serial
    STREAM_PROTOCOL_HTTP_SERVER,    // Serve via HTTP REST API
    STREAM_PROTOCOL_WEBSOCKET,      // Real-time WebSocket updates
    STREAM_PROTOCOL_WEB_PAGE        // Full web page with live display
} stream_protocol_t;

// Animation frame message
typedef struct {
    char eye_id[16];                // "LEFT_EYE" or "RIGHT_EYE"
    uint32_t timestamp_ms;          // Current time
    uint32_t loop_count;            // Main loop counter
    char animation_name[32];        // Current animation
    eye_keyframe_t frame;           // Current frame data
    bool animation_active;          // Animation running?
} animation_frame_msg_t;

// Stream configuration
typedef struct {
    stream_protocol_t protocol;
    bool enabled;
    uint32_t update_interval_ms;    // How often to send frames
    uint32_t last_send_time;
    char pc_ip[16];                 // For WiFi protocols
    uint16_t pc_port;               // For WiFi protocols
} animation_stream_config_t;

// Function declarations
esp_err_t animation_stream_init(animation_stream_config_t* config, stream_protocol_t protocol);
esp_err_t animation_stream_send_frame(const char* eye_id, const eye_display_t* display, uint32_t loop_count);
esp_err_t animation_stream_send_json(const animation_frame_msg_t* msg);
esp_err_t animation_stream_send_binary(const animation_frame_msg_t* msg);
void animation_stream_cleanup(animation_stream_config_t* config);

// PC-side helpers
void animation_stream_print_pc_listener_code(void);

#endif // P32_ANIMATION_STREAM_H