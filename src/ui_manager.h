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
     * @brief Show error message
     * @param message Error message to display
     */
    void showError(const String& message);
    
    /**
     * @brief Show "waiting for connection" animation
     */
    void showWaitingAnimation();

private:
    bool lastConnected;
    uint8_t lastSlot;
    String lastLabel;
    unsigned long lastUpdate;
    uint8_t animFrame;
    
    void drawStatusBar(bool isConnected);
    void drawBatteryLevel();
    void drawMainContent(const String& slotLabel, bool hasPassword, uint8_t slotNum);
    void drawHintBar(bool isConnected, bool hasPassword);
    void drawBluetoothIcon(int x, int y, uint16_t color);
};

#endif // UI_MANAGER_H
