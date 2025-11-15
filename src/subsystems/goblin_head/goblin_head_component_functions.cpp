#include "subsystems/goblin_head/goblin_head_component_functions.hpp"
#include "components/interfaces/spi_display_bus.hdr"
#include "core/memory/SharedMemory.hpp"
#include "with.hpp"
#include "esp_random.h"

// Shared state classes (auto-included in all components)
#include "BalanceCompensation.hpp"
#include "BehaviorControl.hpp"
#include "CollisionAvoidance.hpp"
#include "EmergencyCoordination.hpp"
#include "Environment.hpp"
#include "FrameProcessor.hpp"
#include "ManipulationControl.hpp"
#include "MicrophoneData.hpp"
#include "Mood.hpp"
#include "Personality.hpp"
#include "SensorFusion.hpp"
#include "SysTest.hpp"

// Shared type definitions (auto-included in all components)
#include "shared_headers/color_schema.hpp"
#include "shared_headers/PixelType.hpp"

// Auto-generated component aggregation file

// Subsystem-scoped static variables (shared across all components in this file)
static int display_width = 240;
static int display_height = 240;
static int bytes_per_pixel = 2;  // RGB565
static uint8_t* front_buffer = NULL;
static uint8_t* back_buffer = NULL;
static int display_size = 0;
static int current_row_count = 10;
static int max_display_height = INT_MAX;  // Min height across all displays
static char* color_schema = nullptr;

static bool debug = true;
// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01 component implementation
// Defines display parameters via gc9a01.hdr for upstream components
// Actual display I/O handled by lower-level driver (generic_spi_display)

#include "esp_log.h"

esp_err_t gc9a01_init(void)
{
    ESP_LOGI("gc9a01", "gc9a01 display initialized");
    return ESP_OK;
}

void gc9a01_act(void)
{
    // No-op: display I/O handled by lower layers
}
// --- End: config\components\hardware\gc9a01.src ---

// --- Begin: config\components\drivers\generic_spi_display.src ---
// generic_spi_display.src - Display output driver with debug routing
// If debug=true: sends buffer data to PC via network (for visualization)
// If debug=false: sends buffer data to physical GC9A01 displays via SPI DMA
// Uses adaptive row-based buffering: current_row_count varies based on DMA state

#include "esp_log.h"
#include "driver/spi_master.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "string.h"
#include "fcntl.h"
#include "errno.h"

static const char* TAG = "generic_spi_display";

// WiFi credentials for debug mode
#define WIFI_SSID "ATT7b9q3Ku"
#define WIFI_PASS "89cqy6d7jjd7"
#define SERVER_IP "192.168.1.79"  // Your PC's IP address
#define SERVER_PORT 5555

// Network connection state (for debug mode)
static struct {
    int socket_fd;
    bool connected_to_server;
    uint32_t frames_sent;
    bool wifi_connected;
} network_state = {
    .socket_fd = -1,
    .connected_to_server = false,
    .frames_sent = 0,
    .wifi_connected = false
};

// WiFi initialization flag (shared across all displays)
static bool wifi_already_initialized = false;

// Display buffer management
typedef struct {
    uint8_t* front_buffer;
    uint8_t* back_buffer;
    uint32_t buffer_size;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    bool in_use;
} display_buffer_slot_t;

#define MAX_DISPLAY_SLOTS 3  // Left eye, right eye, mouth
static display_buffer_slot_t display_buffers[MAX_DISPLAY_SLOTS] = {0};
static int next_slot_index = 0;
static int current_send_slot = 0;

// SPI DMA pipeline state (for production mode)
static struct {
    bool dma1_busy;  // PSRAM -> buffer transfer active
    bool dma2_busy;  // buffer -> display transfer active
    spi_device_handle_t display_spi_handle;
    bool ping_pong_state;
    int current_row[3];  // Track row position for up to 3 displays
    int last_display_id;  // Which display we're currently working on
} spi_state = {
    .dma1_busy = false,
    .dma2_busy = false,
    .display_spi_handle = NULL,
    .ping_pong_state = false,
    .current_row = {0, 0, 0},
    .last_display_id = -1
};

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi connecting...");
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        network_state.wifi_connected = false;
        network_state.connected_to_server = false;
        if (network_state.socket_fd >= 0)
        {
            close(network_state.socket_fd);
            network_state.socket_fd = -1;
        }
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi disconnected, reconnecting...");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "WiFi connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        network_state.wifi_connected = true;
    }
}

static esp_err_t setup_wifi(void)
{
    // Initialize NVS (required for WiFi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    
    // Configure WiFi
    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WiFi initialized, connecting to %s...", WIFI_SSID);
    return ESP_OK;
}

static esp_err_t connect_to_server(void)
{
    if (network_state.connected_to_server)
    {
        return ESP_OK;
    }
    
    if (!network_state.wifi_connected)
    {
        return ESP_ERR_NOT_FINISHED;
    }
    
    // Create socket
    network_state.socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (network_state.socket_fd < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket");
        return ESP_FAIL;
    }
    
    // Set socket to non-blocking for connect
    int flags = fcntl(network_state.socket_fd, F_GETFL, 0);
    fcntl(network_state.socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    // Attempt connection
    int ret = connect(network_state.socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == 0 || (ret < 0 && errno == EINPROGRESS))
    {
        // Set back to blocking mode
        fcntl(network_state.socket_fd, F_SETFL, flags);
        network_state.connected_to_server = true;
        ESP_LOGI(TAG, "Connected to display server at %s:%d", SERVER_IP, SERVER_PORT);
        return ESP_OK;
    }
    
    close(network_state.socket_fd);
    network_state.socket_fd = -1;
    return ESP_FAIL;
}

esp_err_t generic_spi_display_init(void) {
    // Register this display's buffers in the slot array
    if (next_slot_index < MAX_DISPLAY_SLOTS && front_buffer != NULL)
    {
        display_buffers[next_slot_index].front_buffer = front_buffer;
        display_buffers[next_slot_index].back_buffer = back_buffer;
        display_buffers[next_slot_index].buffer_size = display_width * display_height * bytes_per_pixel;
        display_buffers[next_slot_index].width = display_width;
        display_buffers[next_slot_index].height = display_height;
        display_buffers[next_slot_index].bpp = bytes_per_pixel;
        display_buffers[next_slot_index].in_use = true;
        
        ESP_LOGI(TAG, "Registered display slot %d: %dx%d, buffer=%p", 
                 next_slot_index, display_width, display_height, front_buffer);
        next_slot_index++;
    }
    
    // Track smallest display height across all displays
    if (display_height < max_display_height)
    {
        max_display_height = display_height;
    }
    
    if (debug)
    {
        // Debug mode: WiFi disabled, will output to serial monitor only
        ESP_LOGI(TAG, "Display driver init: DEBUG MODE (serial monitor output)");
        
        // WiFi code temporarily disabled
        // if (!wifi_already_initialized)
        // {
        //     esp_err_t ret = setup_wifi();
        //     if (ret != ESP_OK)
        //     {
        //         ESP_LOGE(TAG, "WiFi setup failed: %d", ret);
        //         return ret;
        //     }
        //     wifi_already_initialized = true;
        // }
    }
    else
    {
        // Production mode: setup SPI DMA to physical displays
        spi_state.dma1_busy = false;
        spi_state.dma2_busy = false;
        spi_state.ping_pong_state = false;
        ESP_LOGI(TAG, "Display driver init: PRODUCTION MODE (SPI to GC9A01)");
    }
    
    return ESP_OK;
}

void generic_spi_display_act(void) {
    static uint32_t act_call_count = 0;
    act_call_count++;
    
    if (act_call_count % 100 == 0)
    {
        ESP_LOGI(TAG, "Act called %u times, debug=%d, connected=%d, slot=%d", 
                 act_call_count, debug, network_state.connected_to_server, current_send_slot);
    }
    
    if (debug)
    {
        // === DEBUG MODE: Output to serial monitor ===
        
        // Network code temporarily disabled
        // if (!network_state.connected_to_server)
        // {
        //     static uint32_t last_connect_attempt = 0;
        //     uint32_t now = esp_log_timestamp();
        //     if (now - last_connect_attempt > 5000)
        //     {
        //         connect_to_server();
        //         last_connect_attempt = now;
        //     }
        //     return;
        // }
        
        // Rotate through display slots
        if (display_buffers[current_send_slot].front_buffer == NULL || 
            !display_buffers[current_send_slot].in_use)
        {
            // Skip to next slot or wrap around
            current_send_slot++;
            if (current_send_slot >= MAX_DISPLAY_SLOTS || 
                display_buffers[current_send_slot].front_buffer == NULL)
            {
                current_send_slot = 0;
            }
            
            // If still no valid buffer, bail
            if (display_buffers[current_send_slot].front_buffer == NULL)
            {
                return;
            }
        }
        
        display_buffer_slot_t* slot = &display_buffers[current_send_slot];
        
        // Output debug info to serial monitor instead of network
        ESP_LOGI(TAG, "[DEBUG] Slot %d: buffer=%p, size=%u bytes, dims=%dx%d, bpp=%d, current_row_count=%d",
                 current_send_slot, (void*)slot->front_buffer, slot->buffer_size,
                 slot->width, slot->height, slot->bpp, current_row_count);
        
        // Network send code temporarily disabled
        // Build and send packet header
        // typedef struct {
        //     uint32_t magic;
        //     uint32_t frame_number;
        //     uint32_t width;
        //     uint32_t height;
        //     uint32_t bytes_per_pixel;
        // } __attribute__((packed)) display_packet_header_t;
        // 
        // display_packet_header_t header;
        // header.magic = 0xDEADBEEF;
        // header.frame_number = network_state.frames_sent;
        // header.width = slot->width;
        // header.height = slot->height;
        // header.bytes_per_pixel = slot->bpp;
        // 
        // Debug output: show parameters being sent
        // ESP_LOGI(TAG, "[DEBUG] Sending: slot=%d, buffer=%p, size=%u bytes, dims=%dx%d, current_row_count=%d",
        //          current_send_slot, (void*)slot->front_buffer, slot->buffer_size, 
        //          slot->width, slot->height, current_row_count);
        // 
        // Send header
        // ssize_t sent_header = send(network_state.socket_fd, &header, sizeof(header), 0);
        // if (sent_header != sizeof(header))
        // {
        //     ESP_LOGE(TAG, "Failed to send header: %d", sent_header);
        // }
        // else
        // {
        //     Send full frame from front_buffer (already allocated in internal RAM!)
        //     ssize_t sent_data = send(network_state.socket_fd, slot->front_buffer, slot->buffer_size, 0);
        //     if (sent_data == slot->buffer_size)
        //     {
        //         network_state.frames_sent++;
        //         ESP_LOGI(TAG, "[DEBUG] Successfully sent frame %u from slot %d: %u bytes total",
        //                  network_state.frames_sent, current_send_slot, 
        //                  (uint32_t)(sizeof(header) + slot->buffer_size));
        //     }
        //     else
        //     {
        //         ESP_LOGE(TAG, "Failed to send data: %d (expected %u)", sent_data, slot->buffer_size);
        //     }
        // }
        
        // Move to next slot for next call
        current_send_slot++;
        if (current_send_slot >= MAX_DISPLAY_SLOTS || 
            display_buffers[current_send_slot].front_buffer == NULL)
        {
            current_send_slot = 0;
        }
    }
    else
    {
        // === PRODUCTION MODE: SPI DMA to physical displays ===
        if (front_buffer == NULL || back_buffer == NULL)
        {
            return;  // Buffers not allocated
        }
        
        // Identify which display is calling based on buffer pointer
        int display_id = 0;
        if (front_buffer != NULL)
        {
            // Simple hash based on pointer address to identify display
            display_id = ((uintptr_t)front_buffer >> 16) % 3;
        }
        
        spi_transaction_t *completed_trans;
        bool dma1_just_completed = false;
        bool dma2_just_completed = false;
        
        // Check DMA1 (PSRAM->buffer) completion
        if (spi_state.dma1_busy)
        {
            if (spi_device_get_trans_result(spi_state.display_spi_handle, &completed_trans, 0) == ESP_OK)
            {
                spi_state.dma1_busy = false;
                dma1_just_completed = true;
                ESP_LOGV(TAG, "DMA1 completed: display %d row %u", display_id, spi_state.current_row[display_id]);
            }
        }
        
        // Check DMA2 (buffer->display) completion  
        if (spi_state.dma2_busy)
        {
            if (spi_device_get_trans_result(spi_state.display_spi_handle, &completed_trans, 0) == ESP_OK)
            {
                spi_state.dma2_busy = false;
                dma2_just_completed = true;
                spi_state.ping_pong_state = !spi_state.ping_pong_state;
                ESP_LOGV(TAG, "DMA2 completed: sent to display %d", display_id);
            }
        }
        
        // Adaptive row count adjustment (shared across all displays)
        if (dma1_just_completed && dma2_just_completed)
        {
            // Both completed simultaneously - pipeline stalled, increase batch size
            if (current_row_count < max_display_height)
            {
                current_row_count++;
                ESP_LOGD(TAG, "Pipeline stall: increased row_count to %d", current_row_count);
            }
        }
        else if (spi_state.dma1_busy && spi_state.dma2_busy)
        {
            // Both still running - good parallelism, decrease batch size
            if (current_row_count > 1)
            {
                current_row_count--;
                ESP_LOGD(TAG, "Good parallelism: decreased row_count to %d", current_row_count);
            }
        }
        
        // Calculate how many rows to transfer for this specific display
        int rows_remaining = display_height - spi_state.current_row[display_id];
        int rows_to_send = (current_row_count < rows_remaining) ? current_row_count : rows_remaining;
        int buffer_size = display_width * rows_to_send * bytes_per_pixel;
        
        // Start DMA1 if available (load next chunk from PSRAM)
        if (!spi_state.dma1_busy && !spi_state.dma2_busy && rows_to_send > 0)
        {
            uint8_t* target_buffer = spi_state.ping_pong_state ? front_buffer : back_buffer;
            uint32_t row_offset = spi_state.current_row[display_id] * display_width * bytes_per_pixel;
            
            static spi_transaction_t dma1_trans;
            memset(&dma1_trans, 0, sizeof(dma1_trans));
            dma1_trans.length = buffer_size * 8;
            dma1_trans.rx_buffer = target_buffer + row_offset;
            
            if (spi_device_queue_trans(spi_state.display_spi_handle, &dma1_trans, 0) == ESP_OK)
            {
                spi_state.dma1_busy = true;
                spi_state.last_display_id = display_id;
            }
        }
        
        // Start DMA2 if available (send to display)
        if (!spi_state.dma2_busy && spi_state.dma1_busy && spi_state.last_display_id == display_id)
        {
            uint8_t* source_buffer = spi_state.ping_pong_state ? back_buffer : front_buffer;
            
            static spi_transaction_t dma2_trans;
            memset(&dma2_trans, 0, sizeof(dma2_trans));
            dma2_trans.length = buffer_size * 8;
            dma2_trans.tx_buffer = source_buffer;
            
            if (spi_device_queue_trans(spi_state.display_spi_handle, &dma2_trans, 0) == ESP_OK)
            {
                spi_state.dma2_busy = true;
                spi_state.current_row[display_id] += rows_to_send;
                if (spi_state.current_row[display_id] >= display_height)
                {
                    spi_state.current_row[display_id] = 0;  // Wrapped to next frame
                }
            }
        }
    }
}
// --- End: config\components\drivers\generic_spi_display.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_eye.src ---
// goblin_eye component implementation
// Generic goblin eye rendering using mood-based color effects
// Note: display_width, display_height, bytes_per_pixel are injected by use_fields system

#include "esp_log.h"
#include "shared/mood.hpp"
#include "core/memory/SharedMemory.hpp"

// Goblin emotion intensity multiplier - goblins show emotions STRONGLY (1.5x)
static constexpr float GOBLIN_EMOTION_INTENSITY = 1.5f;

// Mood tracking for optimization
static Mood lastMood;
static bool mood_initialized = false;

// Mood-to-color mapping for goblin eyes
static const MoodColorEffect goblin_mood_effects[Mood::componentCount] = {
    // ANGER: Red tint, reduces green/blue
    MoodColorEffect(0.8f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY),
    // FEAR: Blue tint, pale
    MoodColorEffect(-0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY, 0.6f * GOBLIN_EMOTION_INTENSITY),
    // HAPPINESS: Yellow/warm tint
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // SADNESS: Desaturate
    MoodColorEffect(-0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.1f * GOBLIN_EMOTION_INTENSITY),
    // CURIOSITY: Green tint
    MoodColorEffect(0.1f * GOBLIN_EMOTION_INTENSITY, 0.7f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY),
    // AFFECTION: Purple/warm tint
    MoodColorEffect(0.4f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY),
    // IRRITATION: Orange-red tint
    MoodColorEffect(0.6f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY),
    // CONTENTMENT: Warm, slightly yellow
    MoodColorEffect(0.3f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // EXCITEMENT: Bright, all colors up
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY)
};

esp_err_t goblin_eye_init(void) 
{
    ESP_LOGI("goblin_eye", "Initializing goblin eye mood processing (intensity: %.1fx)", GOBLIN_EMOTION_INTENSITY);
    
    // Clear mood tracking
    lastMood.clear();
    mood_initialized = false;
    
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // Get current global mood from shared memory
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr)
    {
        return;
    }
    
    // Check if buffer is available (allocated by positioned component like goblin_left_eye)
    if (front_buffer == NULL || display_size == 0)
    {
        return;
    }
    
    // Check if mood changed (optimization - only render when mood changes)
    if (lastMood != *mood_ptr || !mood_initialized)
    {
        // Calculate pixel count from buffer size
        const uint32_t pixel_count = display_size / bytes_per_pixel;
        
        // Apply mood effects to display buffer
        adjustMood<Pixel_RGB565>(
            front_buffer,
            pixel_count,
            *mood_ptr,
            goblin_mood_effects
        );
        
        lastMood = *mood_ptr;
        mood_initialized = true;
        
        ESP_LOGD("goblin_eye", "Applied mood effects to buffer (%u pixels)", pixel_count);
    }
}
// --- End: config\bots\bot_families\goblins\head\goblin_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_left_eye.src ---
// goblin_left_eye.src - Allocate display buffer for left eye
// Component chain: goblin_left_eye (allocate) -> goblin_eye (render) -> generic_spi_display (send)
// Note: display_width, display_height, bytes_per_pixel auto-assigned by use_fields in init() and act()

#include "esp_log.h"
#include "esp_heap_caps.h"

// Eye position (left eye relative to skull center)
struct LeftEyePosition {
    int16_t x;      // -50 = left of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} left_eye_position = {-50, 30, -35};

esp_err_t goblin_left_eye_init(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    
    // Calculate buffer size based on display parameters
    display_size = display_width * display_height * bytes_per_pixel;
    
    ESP_LOGI("goblin_left_eye", "Allocating display buffer for left eye (%u bytes, %dx%d)", 
             display_size, display_width, display_height);
    
    // Allocate front buffer (DMA-capable internal RAM)
    front_buffer = (uint8_t*)heap_caps_malloc(display_size, MALLOC_CAP_DMA);
    if (!front_buffer)
    {
        ESP_LOGE("goblin_left_eye", "Failed to allocate %u bytes for front buffer", display_size);
        return ESP_ERR_NO_MEM;
    }
    
    // Allocate back buffer (DMA-capable internal RAM)
    back_buffer = (uint8_t*)heap_caps_malloc(display_size, MALLOC_CAP_DMA);
    if (!back_buffer)
    {
        ESP_LOGE("goblin_left_eye", "Failed to allocate %u bytes for back buffer", display_size);
        free(front_buffer);
        front_buffer = NULL;
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize both buffers with neutral color (dark green iris)
    uint16_t neutral = 0x0400;  // RGB565: (0, 8, 0)
    uint16_t* front_u16 = (uint16_t*)front_buffer;
    uint16_t* back_u16 = (uint16_t*)back_buffer;
    
    uint32_t pixel_count = display_size / bytes_per_pixel;
    for (uint32_t i = 0; i < pixel_count; i++)
    {
        front_u16[i] = neutral;
        back_u16[i] = neutral;
    }
    
    ESP_LOGI("goblin_left_eye", "Display buffers allocated (position: %d,%d,%d mm)",
             left_eye_position.x, left_eye_position.y, left_eye_position.z);
    
    return ESP_OK;
}

void goblin_left_eye_act(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    
    // Buffer management handled by component chain:
    // - goblin_eye.src will render mood effects into front_buffer
    // - generic_spi_display.src will send to hardware or debug server
}
// --- End: config\bots\bot_families\goblins\head\goblin_left_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_mouth_display.src ---
// goblin_mouth_display.src - Mouth display (currently disabled - not enough RAM)
// Note: display_width, display_height, bytes_per_pixel auto-assigned by use_fields

#include "esp_log.h"

// Mouth position (relative to skull center)
struct MouthPosition {
    int16_t x;      // 0 = center
    int16_t y;      // -80 = below center
    int16_t z;      // 0 = front of face
} mouth_position = {0, -80, 0};

esp_err_t goblin_mouth_display_init(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    // Mouth display currently disabled - 480x320 RGB666 (460KB) exceeds available RAM
    
    ESP_LOGW("goblin_mouth_display", "Mouth display disabled (would need %d bytes for %dx%d)",
             display_width * display_height * bytes_per_pixel, display_width, display_height);
    
    return ESP_OK;
}

void goblin_mouth_display_act(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    // Currently disabled - implement chunked rendering when ready
}
// --- End: config\bots\bot_families\goblins\head\goblin_mouth_display.src ---

// --- Begin: config\components\templates\goblin_mouth_mood_display.src ---
// goblin_mouth_mood_display stub - rendering logic placeholder
#include "esp_log.h"

esp_err_t goblin_mouth_mood_display_init(void) {
    ESP_LOGI("goblin_mouth_mood_display", "Init (stub)");
    return ESP_OK;
}

void goblin_mouth_mood_display_act(void) {
    // TODO: Implement mouth rendering with mood effects
}
// --- End: config\components\templates\goblin_mouth_mood_display.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_right_eye.src ---
// goblin_right_eye.src - Right eye uses same buffer as left (shared processing)
// Component chain: goblin_right_eye -> goblin_eye (shared) -> generic_spi_display
// Note: display_width, display_height, bytes_per_pixel auto-assigned by use_fields

#include "esp_log.h"

// Eye position (right eye relative to skull center)
struct RightEyePosition {
    int16_t x;      // +50 = right of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} right_eye_position = {50, 30, -35};

esp_err_t goblin_right_eye_init(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    // Right eye shares the buffer allocated by goblin_left_eye (both 240x240 RGB565)
    
    ESP_LOGI("goblin_right_eye", "Right eye configured (shares buffer, %dx%d)",
             display_width, display_height);
    
    return ESP_OK;
}

void goblin_right_eye_act(void)
{
    // NOTE: display_width, display_height, bytes_per_pixel are auto-assigned above by generator
    // Buffer processing handled by shared goblin_eye component
}
// --- End: config\bots\bot_families\goblins\head\goblin_right_eye.src ---

// --- Begin: config\components\interfaces\spi_display_bus.src ---
// spi_display_bus component implementation
// Dedicated SPI bus for display devices with dynamic pin assignment

#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"
// Removed: #include "components/spi_display_bus.hdr" - .hdr content aggregated into .hpp

#include <stddef.h>

static constexpr size_t SPI_DISPLAY_SLOT_COUNT = 32U;
static spi_display_pinset_t spi_display_slots[SPI_DISPLAY_SLOT_COUNT];
spi_display_pinset_t cur_spi_display_pin;

static spi_display_pinset_t shared_display_pins;

// Helper to claim the next available GPIO from the provided assignable list
static int get_next_assignable(const int *assignable, size_t assignable_count) {
    for (size_t i = 0; i < assignable_count; ++i) {
        const int candidate = assignable[i];
        bool already_claimed = false;
        for (size_t j = 0; j < assigned_pins_count; ++j) {
            if (assigned_pins[j] == candidate) {
                already_claimed = true;
                break;
            }
        }

        if (!already_claimed) {
            if (assigned_pins_count >= (sizeof(assigned_pins) / sizeof(assigned_pins[0]))) {
                ESP_LOGE("spi_display_bus", "Assigned pin buffer exhausted");
                esp_system_abort("Assigned pin buffer exhausted");
            }

            assigned_pins[assigned_pins_count++] = candidate;
            return candidate;
        }
    }

    ESP_LOGE("spi_display_bus", "No assignable pins remain for SPI display bus");
    esp_system_abort("SPI display bus ran out of assignable pins");
    return -1; // Unreachable, abort will terminate execution
}

esp_err_t spi_display_bus_init(void) {
    ESP_LOGI("spi_display_bus", "=== SPI_DISPLAY_BUS_INIT STARTED ===");

    // Locate the first available slot for a display device
    size_t slot = 0;
    while (slot < SPI_DISPLAY_SLOT_COUNT && spi_display_slots[slot].cs != -1) {
        ++slot;
    }

    if (slot >= SPI_DISPLAY_SLOT_COUNT) {
        ESP_LOGE("spi_display_bus", "No remaining SPI display device slots available");
        return ESP_FAIL;
    }

    // Assign shared pins (MOSI, CLK, RESET) once for the bus
    const bool shared_unassigned = (shared_display_pins.mosi < 0);
    if (shared_unassigned) {
        shared_display_pins.mosi = get_next_assignable(spi_assignable, spi_assignable_count);
        shared_display_pins.clk = get_next_assignable(spi_assignable, spi_assignable_count);
        shared_display_pins.rst = get_next_assignable(spi_assignable, spi_assignable_count);

        spi_bus_config_t bus_cfg = {
            .mosi_io_num = shared_display_pins.mosi,
            .miso_io_num = -1,
            .sclk_io_num = shared_display_pins.clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 240 * 240 * 2 + 8,
        };

        const esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        ESP_LOGI("spi_display_bus", "SPI bus initialize result: %s", esp_err_to_name(ret));
        if (ret != ESP_OK) {
            ESP_LOGE("spi_display_bus", "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI("spi_display_bus",
                 "Shared SPI display pins assigned MOSI:%d CLK:%d RST:%d",
                 shared_display_pins.mosi,
                 shared_display_pins.clk,
                 shared_display_pins.rst);
    }

    spi_display_pinset_t pins = shared_display_pins;
    pins.cs = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.dc = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.bl = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.handle = nullptr;

    spi_display_slots[slot] = pins;

    ESP_LOGI("spi_display_bus",
             "Display slot %u assigned pins MOSI:%d CLK:%d CS:%d DC:%d BL:%d RST:%d",
             static_cast<unsigned>(slot),
             pins.mosi,
             pins.clk,
             pins.cs,
             pins.dc,
             pins.bl,
             pins.rst);

    return ESP_OK;
}

void spi_display_bus_act(void) {
    static int slot = 0;

    if (spi_display_slots[0].cs == -1) {
        slot = 0;
        cur_spi_display_pin = spi_display_pinset_t();
        return;
    }

    if (slot < 0 || static_cast<size_t>(slot) >= SPI_DISPLAY_SLOT_COUNT ||
        spi_display_slots[static_cast<size_t>(slot)].cs == -1) {
        slot = 0;
    }

    cur_spi_display_pin = spi_display_slots[static_cast<size_t>(slot)];

    ++slot;
    if (static_cast<size_t>(slot) >= SPI_DISPLAY_SLOT_COUNT ||
        spi_display_slots[static_cast<size_t>(slot)].cs == -1) {
        slot = 0;
    }
}
// --- End: config\components\interfaces\spi_display_bus.src ---
