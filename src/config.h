#ifndef CONFIG_H
#define CONFIG_H

// Device Configuration
#define DEVICE_NAME "M5-PassKey"
#define DEVICE_MANUFACTURER "M5Stack"

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
#define UI_UPDATE_INTERVAL_MS 100
#define FEEDBACK_FLASH_MS 200
#define LONG_PRESS_MS 1000

// NVS Namespace
#define NVS_NAMESPACE "passkey"

#endif // CONFIG_H
