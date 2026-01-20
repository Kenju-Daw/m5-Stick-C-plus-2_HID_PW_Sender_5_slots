#include "ui_manager.h"

// Brightness levels: 25%, 50%, 75%, 100%
const uint8_t BRIGHTNESS_LEVELS[] = {64, 128, 192, 255};
const uint8_t NUM_BRIGHTNESS_LEVELS = 4;

UIManager::UIManager() 
    : lastConnected(false), lastSlot(255), lastLabel(""), 
      lastUpdate(0), animFrame(0),
      currentBrightness(DEFAULT_BRIGHTNESS), brightnessIndex(1),
      displaySleeping(false), displayDimmed(false), lastActivityTime(0) {
}

void UIManager::init() {
    // M5StickCPlus2 display is 240x135, use landscape
    M5.Display.setRotation(SCREEN_ROTATION);
    M5.Display.fillScreen(COLOR_BACKGROUND);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    M5.Display.setTextSize(1);
    
    // Set initial brightness
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
    currentBrightness = DEFAULT_BRIGHTNESS;
    lastActivityTime = millis();
    
    // Initial State
    setUIState(STATE_STEALTH);
    
    Serial.println("[UIManager] Display initialized");
}

void UIManager::setUIState(UIState state) {
    currentState = state;
    
    // Reset update trackers/timer
    lastUpdate = millis();
    lastSlot = 255;
    
    // Only clear if screen changes logic requires it
    // For specific screens we fill inside the helper
    if (state != STATE_CONNECTED && state != STATE_PAIRING_SUCCESS) {
         M5.Display.fillScreen(COLOR_BACKGROUND); 
    }

    switch (state) {
        case STATE_STEALTH:
            showStealthMode();
            break;
        case STATE_RECONNECTING:
            showWaitingAnimation(); 
            M5.Display.setTextColor(COLOR_WAITING, COLOR_BACKGROUND);
            M5.Display.setTextSize(2);
            M5.Display.setCursor(20, 40);
            M5.Display.print("Wake & Connect");
            M5.Display.setTextSize(1);
            M5.Display.setCursor(30, 80);
            M5.Display.print("Searching for PC...");
            break;
        case STATE_PAIRING_WAIT:
            showPairingMode();
            break;
        case STATE_SHOW_PIN:
             // PIN is handled by showPairingPin
            break;
        case STATE_PAIRING_SUCCESS:
            showPairingSuccess(); 
            break;
        case STATE_CONNECTED:
            // Standard UI will pick this up
            break;
    }
}

void UIManager::update(bool isConnected, uint8_t currentSlot, const String& slotLabel, bool hasPassword) {
    // SECURITY/TRANSITION LOGIC
    if (currentState == STATE_PAIRING_SUCCESS) {
         // Non-blocking wait for 2 seconds
         if (millis() - lastUpdate > 2000) {
             setUIState(STATE_CONNECTED);
         }
         return; 
    }
    
    // Ignore updates during locked states
    if (currentState == STATE_STEALTH || 
        currentState == STATE_PAIRING_WAIT || 
        currentState == STATE_SHOW_PIN ||
        currentState == STATE_RECONNECTING) {
        return; 
    }

    unsigned long now = millis();
    
    // Check if state actually changed
    bool stateChanged = (isConnected != lastConnected) || 
                        (currentSlot != lastSlot) || 
                        (slotLabel != lastLabel);
    
    // Throttle updates - only update if enough time passed OR state changed
    if (!stateChanged && (now - lastUpdate < UI_UPDATE_INTERVAL_MS)) {
        return;
    }
    
    // Only do full redraw if state changed (reduces flicker)
    if (stateChanged) {
        lastUpdate = now;
        lastConnected = isConnected;
        lastSlot = currentSlot;
        lastLabel = slotLabel;
        
        // Clear and redraw only when state changes
        M5.Display.fillScreen(COLOR_BACKGROUND);
        
        drawStatusBar(isConnected);
        drawMainContent(slotLabel, hasPassword, currentSlot);
        drawHintBar(isConnected, hasPassword);
    }
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

void UIManager::showLockingFeedback() {
    // Red flash with "LOCKED" text
    M5.Display.fillScreen(COLOR_DISCONNECTED); // Red background
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_DISCONNECTED);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(40, 60);
    M5.Display.print("LOCKED");
    delay(500);
    
    // Force redraw
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

void UIManager::showPairingPin(uint32_t pin) {
    M5.Display.fillScreen(COLOR_BACKGROUND);
    
    M5.Display.setTextColor(COLOR_ACCENT, COLOR_BACKGROUND);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 20);
    M5.Display.print("Enter on PC:");
    
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    M5.Display.setTextSize(4);
    M5.Display.setCursor(40, 60);
    M5.Display.printf("%06d", pin);
    
    // Force redraw after
    lastUpdate = 0;
    lastSlot = 255;
}

void UIManager::showPairingSuccess() {
    M5.Display.fillScreen(COLOR_CONNECTED);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_CONNECTED);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(30, 60);
    M5.Display.print("PAIRED!");
    // Delay handled by state machine
    
    // Force redraw
    lastUpdate = 0;
    lastSlot = 255;
}

void UIManager::showStealthMode() {
    M5.Display.fillScreen(COLOR_BACKGROUND);
    
    // Icon
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT_SECONDARY, COLOR_BACKGROUND);
    M5.Display.setCursor(10, 10);
    M5.Display.print("[STEALTH MODE]");
    
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(20, 50);
    M5.Display.print("Advertising");
    M5.Display.setCursor(50, 80);
    M5.Display.print("DISABLED");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_ACCENT, COLOR_BACKGROUND);
    M5.Display.setCursor(10, 110);
    M5.Display.print("Hold A+B to Pair");
    
    // Reset state to ensure full redraw when exiting
    lastUpdate = 0;
    lastSlot = 255;
}

void UIManager::showPairingMode() {
    M5.Display.fillScreen(COLOR_WAITING);
    M5.Display.setTextColor(COLOR_BACKGROUND, COLOR_WAITING);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(30, 40);
    M5.Display.print("PAIRING MODE");
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(40, 80);
    M5.Display.print("Discoverable...");
    
    delay(1000);
    lastUpdate = 0;
    lastSlot = 255;
}

// ============== Power Saving Methods ==============

void UIManager::setBrightness(uint8_t level) {
    currentBrightness = level;
    M5.Display.setBrightness(level);
    Serial.printf("[UI] Brightness set to %d\n", level);
}

void UIManager::cycleBrightness() {
    brightnessIndex = (brightnessIndex + 1) % NUM_BRIGHTNESS_LEVELS;
    uint8_t newBrightness = BRIGHTNESS_LEVELS[brightnessIndex];
    setBrightness(newBrightness);
    
    // Show brightness indicator briefly
    M5.Display.fillRect(SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 10, 100, 20, COLOR_ACCENT);
    M5.Display.setTextColor(COLOR_TEXT_PRIMARY, COLOR_ACCENT);
    M5.Display.setCursor(SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 5);
    M5.Display.printf("Brightness: %d%%", (newBrightness * 100) / 255);
    delay(500);
    
    // Force redraw
    lastUpdate = 0;
    lastSlot = 255;
    
    resetActivityTimer();
}

void UIManager::sleep() {
    if (!displaySleeping) {
        displaySleeping = true;
        M5.Display.setBrightness(0);
        M5.Display.sleep();
        Serial.println("[UI] Display sleeping");
    }
}

void UIManager::wake() {
    if (displaySleeping) {
        displaySleeping = false;
        displayDimmed = false;
        M5.Display.wakeup();
        M5.Display.setBrightness(currentBrightness);
        Serial.println("[UI] Display woke up");
        
        // Force full redraw
        lastUpdate = 0;
        lastSlot = 255;
    } else if (displayDimmed) {
        displayDimmed = false;
        M5.Display.setBrightness(currentBrightness);
        Serial.println("[UI] Display un-dimmed");
    }
    
    resetActivityTimer();
}

void UIManager::resetActivityTimer() {
    lastActivityTime = millis();
}

void UIManager::checkPowerTimeout() {
    if (displaySleeping) return;  // Already sleeping
    
    unsigned long elapsed = millis() - lastActivityTime;
    
    // Light Sleep (Screen Off) after LIGHT_SLEEP_MS
    if (elapsed >= LIGHT_SLEEP_MS) {
        sleep();
    }
    // Dim after DIM_TIMEOUT_MS
    else if (elapsed >= DIM_TIMEOUT_MS && !displayDimmed) {
        displayDimmed = true;
        M5.Display.setBrightness(DIM_BRIGHTNESS);
        Serial.println("[UI] Display dimmed");
    }
}

