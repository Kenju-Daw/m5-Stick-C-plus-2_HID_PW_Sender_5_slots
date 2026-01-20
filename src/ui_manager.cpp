#include "ui_manager.h"

UIManager::UIManager() 
    : lastConnected(false), lastSlot(255), lastLabel(""), 
      lastUpdate(0), animFrame(0) {
}

void UIManager::init() {
    // M5StickCPlus2 display is 240x135, use landscape
    M5.Display.setRotation(SCREEN_ROTATION);
    M5.Display.fillScreen(COLOR_BACKGROUND);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    M5.Display.setTextSize(1);
    
    Serial.println("[UIManager] Display initialized");
}

void UIManager::update(bool isConnected, uint8_t currentSlot, const String& slotLabel, bool hasPassword) {
    unsigned long now = millis();
    
    // Throttle updates to avoid flicker
    if (now - lastUpdate < UI_UPDATE_INTERVAL_MS) {
        // Only force update if state changed
        if (isConnected == lastConnected && currentSlot == lastSlot && slotLabel == lastLabel) {
            return;
        }
    }
    
    lastUpdate = now;
    lastConnected = isConnected;
    lastSlot = currentSlot;
    lastLabel = slotLabel;
    
    // Clear and redraw
    M5.Display.fillScreen(COLOR_BACKGROUND);
    
    drawStatusBar(isConnected);
    drawMainContent(slotLabel, hasPassword, currentSlot);
    drawHintBar(isConnected, hasPassword);
}

void UIManager::drawStatusBar(bool isConnected) {
    // Draw status bar background
    M5.Display.fillRect(0, 0, SCREEN_WIDTH, 22, 0x1082); // Dark blue-gray
    
    // Bluetooth icon
    uint16_t btColor = isConnected ? COLOR_CONNECTED : COLOR_WAITING;
    drawBluetoothIcon(5, 3, btColor);
    
    // Connection text
    M5.Display.setTextSize(1);
    M5.Display.setCursor(25, 7);
    M5.Display.setTextColor(btColor, 0x1082);
    M5.Display.print(isConnected ? "Connected" : "Waiting...");
    
    // Battery level
    drawBatteryLevel();
}

void UIManager::drawBatteryLevel() {
    int battLevel = M5.Power.getBatteryLevel();
    
    // Battery icon position (right side of status bar)
    int battX = SCREEN_WIDTH - 35;
    int battY = 5;
    
    // Draw battery outline
    M5.Display.drawRect(battX, battY, 25, 12, COLOR_TEXT_SECONDARY);
    M5.Display.fillRect(battX + 25, battY + 3, 3, 6, COLOR_TEXT_SECONDARY);
    
    // Fill based on level
    uint16_t battColor = battLevel > 20 ? COLOR_CONNECTED : COLOR_DISCONNECTED;
    int fillWidth = map(battLevel, 0, 100, 0, 23);
    M5.Display.fillRect(battX + 1, battY + 1, fillWidth, 10, battColor);
    
    // Battery percentage text
    M5.Display.setCursor(battX - 25, battY + 2);
    M5.Display.setTextColor(COLOR_TEXT_SECONDARY, 0x1082);
    M5.Display.printf("%d%%", battLevel);
}

void UIManager::drawMainContent(const String& slotLabel, bool hasPassword, uint8_t slotNum) {
    // Main content area: centered slot label
    int centerY = SCREEN_HEIGHT / 2;
    
    // Slot number badge
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_ACCENT, COLOR_BACKGROUND);
    
    String slotBadge = "[ " + String(slotNum + 1) + " ]";
    int badgeWidth = slotBadge.length() * 12;
    M5.Display.setCursor((SCREEN_WIDTH - badgeWidth) / 2, centerY - 25);
    M5.Display.print(slotBadge);
    
    // Slot label
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    
    int labelWidth = slotLabel.length() * 12;
    M5.Display.setCursor((SCREEN_WIDTH - labelWidth) / 2, centerY);
    M5.Display.print(slotLabel);
    
    // Password status indicator
    M5.Display.setTextSize(1);
    if (hasPassword) {
        M5.Display.setTextColor(COLOR_CONNECTED, COLOR_BACKGROUND);
        M5.Display.setCursor((SCREEN_WIDTH - 60) / 2, centerY + 25);
        M5.Display.print("********");
    } else {
        M5.Display.setTextColor(COLOR_TEXT_SECONDARY, COLOR_BACKGROUND);
        M5.Display.setCursor((SCREEN_WIDTH - 78) / 2, centerY + 25);
        M5.Display.print("(not set)");
    }
}

void UIManager::drawHintBar(bool isConnected, bool hasPassword) {
    // Hint bar at bottom
    int hintY = SCREEN_HEIGHT - 18;
    M5.Display.fillRect(0, hintY - 2, SCREEN_WIDTH, 20, 0x1082);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT_SECONDARY, 0x1082);
    
    String hint;
    if (!isConnected) {
        hint = "Pair via Bluetooth settings";
    } else if (!hasPassword) {
        hint = "Set password via Serial";
    } else {
        hint = "A:Send  B:Next Slot";
    }
    
    int hintWidth = hint.length() * 6;
    M5.Display.setCursor((SCREEN_WIDTH - hintWidth) / 2, hintY + 2);
    M5.Display.print(hint);
}

void UIManager::drawBluetoothIcon(int x, int y, uint16_t color) {
    // Simple Bluetooth "B" rune icon (16x16)
    M5.Display.drawLine(x + 4, y + 0, x + 4, y + 14, color);
    M5.Display.drawLine(x + 4, y + 0, x + 10, y + 5, color);
    M5.Display.drawLine(x + 10, y + 5, x + 4, y + 7, color);
    M5.Display.drawLine(x + 4, y + 7, x + 10, y + 9, color);
    M5.Display.drawLine(x + 10, y + 9, x + 4, y + 14, color);
}

void UIManager::showSendFeedback() {
    // Flash screen briefly
    M5.Display.fillScreen(COLOR_CONNECTED);
    delay(FEEDBACK_FLASH_MS);
    // Force redraw on next update
    lastUpdate = 0;
    lastSlot = 255;
}

void UIManager::showSlotChangeFeedback() {
    // Brief blue flash
    M5.Display.fillScreen(COLOR_ACCENT);
    delay(50);
    lastUpdate = 0;
    lastSlot = 255;
}

void UIManager::showError(const String& message) {
    M5.Display.fillScreen(COLOR_DISCONNECTED);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_DISCONNECTED);
    M5.Display.setCursor(10, SCREEN_HEIGHT / 2 - 10);
    M5.Display.print("ERROR");
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, SCREEN_HEIGHT / 2 + 10);
    M5.Display.print(message);
    
    delay(2000);
    lastUpdate = 0;
}

void UIManager::showWaitingAnimation() {
    // Simple dot animation
    animFrame = (animFrame + 1) % 4;
    String dots = "";
    for (int i = 0; i < animFrame; i++) dots += ".";
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_WAITING, COLOR_BACKGROUND);
    M5.Display.setCursor(SCREEN_WIDTH / 2 + 50, 7);
    M5.Display.print(dots + "   ");
}
