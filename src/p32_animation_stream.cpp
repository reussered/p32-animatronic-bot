// P32 Animation Stream Implementation
// Sends animation frames to PC for real-time display

#include "p32_animation_stream.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ANIM_STREAM";
static animation_stream_config_t stream_config = {0};

// Initialize animation streaming
esp_err_t animation_stream_init(animation_stream_config_t* config, stream_protocol_t protocol) {
    if (!config) return ESP_ERR_INVALID_ARG;
    
    memcpy(&stream_config, config, sizeof(animation_stream_config_t));
    stream_config.protocol = protocol;
    stream_config.enabled = true;
    stream_config.update_interval_ms = 100; // 10 FPS default
    stream_config.last_send_time = 0;
    
    const char* protocol_name;
    switch (protocol) {
        case STREAM_PROTOCOL_SERIAL: protocol_name = "Serial/UART"; break;
        case STREAM_PROTOCOL_WIFI_UDP: protocol_name = "WiFi UDP"; break;
        case STREAM_PROTOCOL_WIFI_WEBSOCKET: protocol_name = "WiFi WebSocket"; break;
        default: protocol_name = "Unknown"; break;
    }
    
    ESP_LOGI(TAG, "Animation streaming initialized: %s protocol", protocol_name);
    
    // Print PC listener instructions
    animation_stream_print_pc_listener_code();
    
    return ESP_OK;
}

// Send animation frame to PC
esp_err_t animation_stream_send_frame(const char* eye_id, const eye_display_t* display, uint32_t loop_count) {
    if (!stream_config.enabled || !eye_id || !display) {
        return ESP_OK;
    }
    
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // Throttle updates based on interval
    if (current_time - stream_config.last_send_time < stream_config.update_interval_ms) {
        return ESP_OK;
    }
    
    // Build frame message
    animation_frame_msg_t msg = {0};
    strncpy(msg.eye_id, eye_id, sizeof(msg.eye_id) - 1);
    msg.timestamp_ms = current_time;
    msg.loop_count = loop_count;
    msg.animation_active = display->active;
    msg.frame = display->current_frame;
    
    if (display->current_animation) {
        strncpy(msg.animation_name, display->current_animation->name, sizeof(msg.animation_name) - 1);
    } else {
        strncpy(msg.animation_name, "none", sizeof(msg.animation_name) - 1);
    }
    
    // Send based on protocol
    esp_err_t ret = ESP_OK;
    switch (stream_config.protocol) {
        case STREAM_PROTOCOL_SERIAL:
            ret = animation_stream_send_json(&msg);
            break;
        case STREAM_PROTOCOL_WIFI_UDP:
        case STREAM_PROTOCOL_WIFI_WEBSOCKET:
            ESP_LOGW(TAG, "WiFi protocols not yet implemented, falling back to serial");
            ret = animation_stream_send_json(&msg);
            break;
    }
    
    stream_config.last_send_time = current_time;
    return ret;
}

// Send JSON frame over serial
esp_err_t animation_stream_send_json(const animation_frame_msg_t* msg) {
    if (!msg) return ESP_ERR_INVALID_ARG;
    
    // Format JSON frame for PC consumption
    printf("ANIM_FRAME:{\"eye\":\"%s\",\"time\":%lu,\"loop\":%lu,\"anim\":\"%s\",\"active\":%s,\"frame\":{\"openness\":%.3f,\"pupil\":%.3f,\"color\":\"#%06X\",\"expr\":%d}}\n",
           msg->eye_id,
           msg->timestamp_ms,
           msg->loop_count, 
           msg->animation_name,
           msg->animation_active ? "true" : "false",
           msg->frame.eye_openness,
           msg->frame.pupil_size,
           msg->frame.color,
           msg->frame.expression
    );
    
    return ESP_OK;
}

// Send binary frame (more efficient for high frame rates)
esp_err_t animation_stream_send_binary(const animation_frame_msg_t* msg) {
    if (!msg) return ESP_ERR_INVALID_ARG;
    
    // Binary protocol header: 0xAA 0x55 [size] [data] [checksum]
    uint8_t header[] = {0xAA, 0x55, sizeof(animation_frame_msg_t)};
    printf("%c%c%c", header[0], header[1], header[2]);
    
    // Send raw struct (be careful of alignment/endianness on PC side)
    fwrite(msg, sizeof(animation_frame_msg_t), 1, stdout);
    
    // Simple checksum
    uint8_t checksum = 0;
    uint8_t* data = (uint8_t*)msg;
    for (int i = 0; i < sizeof(animation_frame_msg_t); i++) {
        checksum ^= data[i];
    }
    printf("%c", checksum);
    
    return ESP_OK;
}

// Print PC-side listener code for user convenience
void animation_stream_print_pc_listener_code(void) {
    ESP_LOGI(TAG, "=== PC LISTENER CODE (Copy to Python script) ===");
    printf("\n# Python script to receive and display animations:\n");
    printf("import serial\nimport json\nimport tkinter as tk\nfrom tkinter import canvas\n\n");
    printf("# Open serial port (adjust COM port for your system)\n");
    printf("ser = serial.Serial('COM3', 115200, timeout=1)  # Windows\n");
    printf("# ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)  # Linux\n\n");
    printf("# Create display windows\n");
    printf("root = tk.Tk()\n");
    printf("root.title('P32 Goblin Eyes')\n");
    printf("left_canvas = tk.Canvas(root, width=200, height=200, bg='black')\n");
    printf("right_canvas = tk.Canvas(root, width=200, height=200, bg='black')\n");
    printf("left_canvas.pack(side=tk.LEFT)\n");
    printf("right_canvas.pack(side=tk.RIGHT)\n\n");
    printf("def update_eye(canvas, frame_data):\n");
    printf("    canvas.delete('all')\n");
    printf("    openness = frame_data['openness']\n");
    printf("    color = frame_data['color']\n");
    printf("    # Draw eye based on openness\n");
    printf("    if openness > 0.1:\n");
    printf("        eye_height = int(50 * openness)\n");
    printf("        canvas.create_oval(75, 100-eye_height//2, 125, 100+eye_height//2, fill=color)\n\n");
    printf("def read_animations():\n");
    printf("    line = ser.readline().decode().strip()\n");
    printf("    if line.startswith('ANIM_FRAME:'):\n");
    printf("        data = json.loads(line[11:])\n");
    printf("        canvas = left_canvas if data['eye'] == 'LEFT_EYE' else right_canvas\n");
    printf("        update_eye(canvas, data['frame'])\n");
    printf("    root.after(50, read_animations)\n\n");
    printf("read_animations()\n");
    printf("root.mainloop()\n");
    ESP_LOGI(TAG, "=== END PC LISTENER CODE ===");
}

// Cleanup
void animation_stream_cleanup(animation_stream_config_t* config) {
    if (config) {
        config->enabled = false;
        ESP_LOGI(TAG, "Animation streaming stopped");
    }
}