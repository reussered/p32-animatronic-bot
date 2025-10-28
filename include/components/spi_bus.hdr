#ifndef SPI_BUS_HPP
#define SPI_BUS_HPP

#include "esp_err.h"
#include "driver/spi_master.h"

// ============================================================================
// SPI Bus Component - VSPI Bus Management
// Handles all SPI pin configuration and bus initialization
// ============================================================================

// SPI Bus Device Registration Array
extern int spi_bus_device[7];  // Array to track registered SPI devices
extern int spi_dev_ptr;        // Pointer for device registration
extern int spi_act_ptr;        // Pointer for act sequencing

// Current SPI device for act function
extern int spi_device;

// SPI Bus Pin Configuration - Shared across all SPI devices
extern int spi_bus_mosi_pin;
extern int spi_bus_miso_pin;
extern int spi_bus_sck_pin;
extern int spi_bus_reset_pin;

// SPI Device Pins - Unique per device
extern int spi_device_cs_pins[4];  // chip_select_1 to chip_select_4
extern int spi_device_dc_pins[4];  // data_command_1 to data_command_4

// Current device pins (set by positioned components for their GC9A01 instances)
extern int spi_bus_current_cs_pin;
extern int spi_bus_current_dc_pin;

// Current device handle (set by positioned components for their GC9A01 instances)
extern spi_device_handle_t spi_bus_current_device_handle;

// SPI Bus Handle
extern spi_host_device_t spi_bus_host;
extern spi_bus_config_t spi_bus_config;
extern spi_device_handle_t spi_device_handles[4];

esp_err_t spi_bus_init(void);
void spi_bus_act(void);

// Device registration function
int spi_bus_register_device(void);

#endif // SPI_BUS_HPP