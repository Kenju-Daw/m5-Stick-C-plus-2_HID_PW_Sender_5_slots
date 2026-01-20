#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <M5StickCPlus2.h>
#include "config.h"

/**
 * @brief UI Manager for M5 Stick C Plus 2 LCD Display
 * 
 * Handles all display rendering including:
 * - Connection status indicator (top bar)
 * - Battery level display
 * - Current password slot label
 * - Visual feedback for actions
 * 
 * Display Layout (135x240 pixels, landscape):
 * +----------------------------------+
 * | [BT Icon] Connected    [Batt%]  |  <- Status bar (20px)
 * +----------------------------------+
 * |                                  |
 * |         [ Slot Label ]          |  <- Main area
 * |                                  |
 * +----------------------------------+
 * |        Press A to Send          |  <- Hint bar (20px)
 * +----------------------------------+
 */
class UIManager {
public:
    UIManager();
    
    /**
     * @brief Initialize the display
     * Sets rotation, clears screen, draws initial UI
     */
    void init();
    
    /**
     * @brief Update the display with current state
     * @param isConnected BLE connection status
     * @param currentSlot Currently selected password slot
     * @param slotLabel Label of the current slot
     * @param hasPassword Whether current slot has a password configured
     */
    void update(bool isConnected, uint8_t currentSlot, const String& slotLabel, bool hasPassword);
    
    /**
     * @brief Show visual feedback when password is sent
     * Flashes the screen briefly
     */
    void showSendFeedback();
    
    /**
     * @brief Show visual feedback when slot changed
     */
    void showSlotChangeFeedback();

    /**
     * @brief Show locking feedback (Red FLASH + LOCKED)
     */
    void showLockingFeedback();
    
    /**
     * @brief Show error message
     * @param message Error message to display
     */
    void showError(const String& message);
    
    /**
     * @brief Show pairing PIN (MITM protection)
     * @param pin The 6-digit PIN to display
     */
    void showPairingPin(uint32_t pin);

    /**
     * @brief Show pairing success message
     */
    void showPairingSuccess();

    /**
     * @brief Show "Stealth Mode" screen (Advertising OFF)
     */
    void showStealthMode();
    
    /**
     * @brief Show "Pairing Mode" screen (Advertising ON)
     */
    void showPairingMode();
    
    /**
     * @brief Show "waiting for connection" animation
     */
    void showWaitingAnimation();
    
    /**
     * @brief Set display brightness
     * @param level Brightness 0-255
     */
    void setBrightness(uint8_t level);
    
    /**
     * @brief Cycle through brightness levels (25%, 50%, 75%, 100%)
     */
    void cycleBrightness();
    
    /**
     * @brief Turn off display (sleep mode)
     */
    void sleep();
    
    /**
     * @brief Wake display from sleep
     */
    void wake();
    
    /**
     * @brief Reset activity timer (call on any user interaction)
     */
    void resetActivityTimer();
    
    /**
     * @brief Check for timeout and dim/sleep if needed
     */
    void checkPowerTimeout();
    
    /**
     * @brief Check if display is currently sleeping
     */
    /**
     * @brief Check if display is currently sleeping
     */
    bool isSleeping() { return displaySleeping; }

    // UI States
    enum UIState {
        STATE_STEALTH,      // Logo/Stealth screen
        STATE_RECONNECTING, // "Reconnecting..." (Whitelist)
        STATE_PAIRING_WAIT, // "Pairing Mode"
        STATE_SHOW_PIN,     // Showing PIN (Sticky)
        STATE_PAIRING_SUCCESS, // Success (non-blocking)
        STATE_CONNECTED,    // Main Interface
    };
    
    void setUIState(UIState state);
    UIState getUIState() { return currentState; }

private:
    UIState currentState;
    bool lastConnected;
    // ... rest of private members (lastSlot, lastLabel etc)
    uint8_t lastSlot;
    String lastLabel;
    unsigned long lastUpdate;
    uint8_t animFrame;
    
    // Power saving state
    uint8_t currentBrightness;
    uint8_t brightnessIndex;
    bool displaySleeping;
    bool displayDimmed;
    unsigned long lastActivityTime;
    
    void drawStatusBar(bool isConnected);
    void drawBatteryLevel();
    void drawMainContent(const String& slotLabel, bool hasPassword, uint8_t slotNum);
    void drawHintBar(bool isConnected, bool hasPassword);
    void drawBluetoothIcon(int x, int y, uint16_t color);
};

#endif // UI_MANAGER_H
