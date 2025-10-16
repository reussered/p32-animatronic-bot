// P32 Shared Memory Implementation
// Simple read/write accessors with automatic mesh synchronization

#include "p32_shared_memory.hpp"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char* TAG = "P32_SHMEM";

// Internal memory block structure
typedef struct
{
    char name[MAX_BLOCK_NAME_SIZE];
    void* data;
    size_t size;
    bool allocated;
    bool dirty;  // Modified since last mesh sync
} shared_block_t;

// Global memory pool
static shared_block_t g_shared_blocks[MAX_SHARED_BLOCKS];
static int g_block_count = 0;
static size_t g_total_allocated = 0;

// ===== PUBLIC API =====

esp_err_t alloc_shared(const char *name, size_t size)
{
    if (name == NULL || size == 0)
    {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, size=%d", name, size);
        return ESP_ERR_INVALID_ARG;
    }

    if (strlen(name) >= MAX_BLOCK_NAME_SIZE)
    {
        ESP_LOGE(TAG, "Name too long: %s (max %d chars)", name, MAX_BLOCK_NAME_SIZE - 1);
        return ESP_ERR_INVALID_ARG;
    }

    // Check if name already exists
    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            ESP_LOGW(TAG, "Block '%s' already allocated", name);
            return ESP_OK;  // Not an error, just idempotent
        }
    }

    // Check if we have space
    if (g_block_count >= MAX_SHARED_BLOCKS)
    {
        ESP_LOGE(TAG, "Too many blocks (max %d)", MAX_SHARED_BLOCKS);
        return ESP_ERR_NO_MEM;
    }

    if (g_total_allocated + size > MAX_SHARED_MEMORY_SIZE)
    {
        ESP_LOGE(TAG, "Out of memory: need %d bytes, have %d/%d used",
                 size, g_total_allocated, MAX_SHARED_MEMORY_SIZE);
        return ESP_ERR_NO_MEM;
    }

    // Allocate memory
    void* data = malloc(size);
    if (data == NULL)
    {
        ESP_LOGE(TAG, "malloc failed for %d bytes", size);
        return ESP_ERR_NO_MEM;
    }

    // Initialize to zero
    memset(data, 0, size);

    // Register block
    strncpy(g_shared_blocks[g_block_count].name, name, MAX_BLOCK_NAME_SIZE - 1);
    g_shared_blocks[g_block_count].name[MAX_BLOCK_NAME_SIZE - 1] = '\0';
    g_shared_blocks[g_block_count].data = data;
    g_shared_blocks[g_block_count].size = size;
    g_shared_blocks[g_block_count].allocated = true;
    g_shared_blocks[g_block_count].dirty = false;

    g_total_allocated += size;
    g_block_count++;

    ESP_LOGI(TAG, "Allocated '%s': %d bytes (total: %d/%d)",
             name, size, g_total_allocated, MAX_SHARED_MEMORY_SIZE);

    return ESP_OK;
}

esp_err_t read_shared(const char *name, void *dest, size_t size)
{
    if (name == NULL || dest == NULL || size == 0)
    {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, dest=%p, size=%d",
                 name, dest, size);
        return ESP_ERR_INVALID_ARG;
    }

    // Find block
    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            // Verify size matches
            if (g_shared_blocks[i].size != size)
            {
                ESP_LOGE(TAG, "Size mismatch for '%s': expected %d, got %d",
                         name, g_shared_blocks[i].size, size);
                return ESP_ERR_INVALID_SIZE;
            }

            // Copy from global memory to local variable (NO MESH ACCESS - just memcpy)
            memcpy(dest, g_shared_blocks[i].data, size);

            ESP_LOGV(TAG, "Read '%s': %d bytes", name, size);
            return ESP_OK;
        }
    }

    ESP_LOGE(TAG, "Block not found: '%s'", name);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t write_shared(const char *name, const void *src, size_t size)
{
    if (name == NULL || src == NULL || size == 0)
    {
        ESP_LOGE(TAG, "Invalid parameters: name=%p, src=%p, size=%d",
                 name, src, size);
        return ESP_ERR_INVALID_ARG;
    }

    // Find block
    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            // Verify size matches
            if (g_shared_blocks[i].size != size)
            {
                ESP_LOGE(TAG, "Size mismatch for '%s': expected %d, got %d",
                         name, g_shared_blocks[i].size, size);
                return ESP_ERR_INVALID_SIZE;
            }

            // Copy from local variable to global memory
            memcpy(g_shared_blocks[i].data, src, size);

            // Mark as dirty (triggers mesh broadcast on next loop)
            g_shared_blocks[i].dirty = true;

            ESP_LOGV(TAG, "Write '%s': %d bytes (marked dirty)", name, size);
            return ESP_OK;
        }
    }

    ESP_LOGE(TAG, "Block not found: '%s'", name);
    return ESP_ERR_NOT_FOUND;
}

size_t get_shared_size(const char *name)
{
    if (name == NULL)
    {
        return 0;
    }

    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            return g_shared_blocks[i].size;
        }
    }

    return 0;
}

bool is_shared_dirty(const char *name)
{
    if (name == NULL)
    {
        return false;
    }

    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            return g_shared_blocks[i].dirty;
        }
    }

    return false;
}

void mark_shared_clean(const char *name)
{
    if (name == NULL)
    {
        return;
    }

    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            g_shared_blocks[i].dirty = false;
            ESP_LOGV(TAG, "Marked '%s' clean", name);
            return;
        }
    }
}

int get_dirty_blocks(const char **dirty_names, size_t *dirty_sizes,
                     void **dirty_data, int max_count)
{
    int count = 0;

    for (int i = 0; i < g_block_count && count < max_count; i++)
    {
        if (g_shared_blocks[i].allocated && g_shared_blocks[i].dirty)
        {
            dirty_names[count] = g_shared_blocks[i].name;
            dirty_sizes[count] = g_shared_blocks[i].size;
            dirty_data[count] = g_shared_blocks[i].data;
            count++;
        }
    }

    return count;
}

void* get_shared_ptr(const char *name)
{
    if (name == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated && strcmp(g_shared_blocks[i].name, name) == 0)
        {
            return g_shared_blocks[i].data;
        }
    }

    return NULL;
}

void dump_shared_memory(void)
{
    ESP_LOGI(TAG, "=== Shared Memory Dump ===");
    ESP_LOGI(TAG, "Blocks: %d/%d", g_block_count, MAX_SHARED_BLOCKS);
    ESP_LOGI(TAG, "Memory: %d/%d bytes", g_total_allocated, MAX_SHARED_MEMORY_SIZE);

    for (int i = 0; i < g_block_count; i++)
    {
        if (g_shared_blocks[i].allocated)
        {
            ESP_LOGI(TAG, "  [%d] '%s': %d bytes %s",
                     i,
                     g_shared_blocks[i].name,
                     g_shared_blocks[i].size,
                     g_shared_blocks[i].dirty ? "(DIRTY)" : "");
        }
    }

    ESP_LOGI(TAG, "========================");
}
