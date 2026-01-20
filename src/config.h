#ifndef CONFIG_H
#define CONFIG_H

// Device Configuration
#define DEVICE_NAME "Mjolnir"
#define DEVICE_MANUFACTURER "Thor"

// Password Slots
#define MAX_PASSWORD_SLOTS 5
#define DEFAULT_PASSWORD_SLOT 0
#define MAX_PASSWORD_LENGTH 64

// Display Configuration
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define SCREEN_ROTATION 1

// UI Colors (RGB565)
#define COLOR_BACKGROUND 0x0000  // Black
#define COLOR_CONNECTED 0x07E0   // Green
#define COLOR_DISCONNECTED 0xF800 // Red
#define COLOR_WAITING 0xFFE0     // Yellow
#define COLOR_TEXT_PRIMARY 0xFFFF // White
#define COLOR_TEXT_SECONDARY 0x7BEF // Light Gray
#define COLOR_ACCENT 0x001F      // Blue

// Timing
#define UI_UPDATE_INTERVAL_MS 500   // Reduced from 100ms to prevent flicker
#define FEEDBACK_FLASH_MS 200
#define LONG_PRESS_MS 1000
#define PASSWORD_COOLDOWN_MS 1500   // Cooldown after sending password

// Power Saving
#define DEFAULT_BRIGHTNESS 128      // 50% brightness (0-255)
#define DIM_BRIGHTNESS 32           // Dimmed brightness
#define DIM_TIMEOUT_MS 30000        // Dim after 30 seconds
#define SLEEP_TIMEOUT_MS 60000      // Sleep display after 60 seconds

// Security Configuration
#define RECONNECT_WINDOW_MS 60000   // 60s advertising window on wake/unlock
#define PIN_DISPLAY_MS 30000        // Keep PIN visible for 30s


// NVS Namespace
#define NVS_NAMESPACE "passkey"

#endif // CONFIG_H
