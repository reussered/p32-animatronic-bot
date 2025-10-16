// P32 Shared Memory API - Simple Read/Write Interface with Automatic Synchronization
// 
// This provides a clean accessor API that eliminates manual memcpy/memcmp polling.
// Components allocate named memory blocks and use read()/write() to access them.
// write() automatically triggers ESP-NOW mesh synchronization.

#ifndef P32_SHARED_MEMORY_H
#define P32_SHARED_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of named memory blocks
#define MAX_SHARED_BLOCKS 64

// Maximum size for a single block name
#define MAX_BLOCK_NAME_SIZE 32

// Maximum total shared memory (64 KB default)
#define MAX_SHARED_MEMORY_SIZE (64 * 1024)

/**
 * Allocate a named block of shared memory
 * 
 * ONLY call during component init() functions!
 * 
 * @param name String identifier (max 31 chars, convention: "g_NAME")
 * @param size Size in bytes
 * @return ESP_OK on success, ESP_ERR_NO_MEM if full, ESP_ERR_INVALID_ARG if invalid params
 */
esp_err_t alloc_shared(const char *name, size_t size);

/**
 * Read a named shared memory block into local variable
 * 
 * Copies data from global memory into destination buffer
 * Size MUST match allocated size (enforced at runtime)
 * 
 * Safe to call from any component act() function
 * NO MESH NETWORK ACCESS - just fast memcpy from global pool
 * 
 * @param name String identifier (convention: "g_NAME")
 * @param dest Pointer to destination buffer (local variable)
 * @param size Size of destination (must match allocated size)
 * @return ESP_OK on success
 *         ESP_ERR_NOT_FOUND if name not allocated
 *         ESP_ERR_INVALID_SIZE if size mismatch
 *         ESP_ERR_INVALID_ARG if invalid parameters
 */
esp_err_t read_shared(const char *name, void *dest, size_t size);

/**
 * Write local variable to named shared memory block
 * 
 * Copies data from source buffer into global memory
 * Size MUST match allocated size (enforced at runtime)
 * Automatically marks block as dirty for mesh broadcast
 * 
 * Safe to call from any component act() function
 * TRIGGERS AUTOMATIC MESH SYNCHRONIZATION
 * 
 * @param name String identifier (convention: "g_NAME")
 * @param src Pointer to source buffer (local variable)
 * @param size Size of source (must match allocated size)
 * @return ESP_OK on success
 *         ESP_ERR_NOT_FOUND if name not allocated
 *         ESP_ERR_INVALID_SIZE if size mismatch
 *         ESP_ERR_INVALID_ARG if invalid parameters
 */
esp_err_t write_shared(const char *name, const void *src, size_t size);

/**
 * Get size of a named block
 * 
 * @param name String identifier
 * @return Size in bytes, or 0 if not found
 */
size_t get_shared_size(const char *name);

/**
 * Check if block has been modified since last sync
 * 
 * Used internally by mesh system
 * 
 * @param name String identifier
 * @return true if modified, false otherwise
 */
bool is_shared_dirty(const char *name);

/**
 * Mark block as clean (synced)
 * 
 * Used internally by mesh system after broadcast
 * 
 * @param name String identifier
 */
void mark_shared_clean(const char *name);

/**
 * Get list of all dirty blocks (for mesh broadcast)
 * 
 * Used internally by mesh system
 * 
 * @param dirty_names Output array of name pointers
 * @param dirty_sizes Output array of sizes
 * @param dirty_data Output array of data pointers
 * @param max_count Maximum entries in arrays
 * @return Number of dirty blocks found
 */
int get_dirty_blocks(const char **dirty_names, size_t *dirty_sizes, 
                     void **dirty_data, int max_count);

/**
 * Get raw pointer to shared memory block
 * 
 * INTERNAL USE - mesh system needs direct access for ESP-NOW transmission
 * 
 * @param name String identifier
 * @return Pointer to data, or NULL if not found
 */
void* get_shared_ptr(const char *name);

/**
 * Debug: Print all allocated blocks
 */
void dump_shared_memory(void);

#ifdef __cplusplus
}
#endif

#endif // P32_SHARED_MEMORY_H
