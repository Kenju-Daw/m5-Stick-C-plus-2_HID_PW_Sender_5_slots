/**
 * @file main.cpp
 * @brief M5 Stick C Plus 2 - BLE HID Password Sender
 * 
 * This application turns the M5 Stick C Plus 2 into a Bluetooth LE HID keyboard
 * that can send stored passwords with a button press.
 * 
 * ## Hardware
 * - M5 Stick C Plus 2 (ESP32-PICO-V3-02)
 * - Built-in LCD: 135x240 ST7789
 * - Button A: Front button (GPIO37)
 * - Button B: Side button (GPIO39)
 * 
 * ## Button Mapping
 * - Button A (short press): Send password for current slot
 * - Button B (short press): Cycle to next slot
 * 
 * ## Serial Commands (for configuration)
 * - SET <slot> <password>  : Set password for slot (0-4)
 * - LABEL <slot> <label>   : Set label for slot
 * - LIST                   : Show all configured slots
 * - CLEAR <slot>           : Clear a specific slot
 * - CLEARALL               : Clear all slots
 * - HELP                   : Show help message
 * 
 * @author ESP-GEM Project
 * @version 1.0.0
 */

#include <M5StickCPlus2.h>
#include <BleKeyboard.h>
#include "config.h"
#include "password_manager.h"
#include "ui_manager.h"
#include "version.h"

// Global objects
BleKeyboard bleKeyboard(DEVICE_NAME, DEVICE_MANUFACTURER, 100);
PasswordManager passwordManager;
UIManager uiManager;

// State variables
uint8_t currentSlot = DEFAULT_PASSWORD_SLOT;
bool wasConnected = false;
unsigned long lastUIUpdate = 0;

// Function declarations
void handleButtonA();
void handleButtonB();
void processSerialCommand();
void printHelp();
void listSlots();

void setup() {
    // Initialize M5StickCPlus2
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    delay(500);  // Longer delay for stability
    
    Serial.println();
    Serial.println("================================");
    Serial.println("  M5 PassKey - BLE Password Sender");
    Serial.printf("  Version: %s\n", VERSION_STRING);
    Serial.printf("  Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("================================");
    Serial.println();
    
    // Initialize password manager
    passwordManager.init();
    
    // Initialize UI
    uiManager.init();
    
    // Initialize BLE Keyboard
    Serial.println("[BLE] Starting Bluetooth keyboard...");
    bleKeyboard.begin();
    
    // Initial UI update
    uiManager.update(
        false, 
        currentSlot, 
        passwordManager.getLabel(currentSlot),
        passwordManager.hasPassword(currentSlot)
    );
    
    Serial.println("[SETUP] Ready! Type HELP for commands.");
    Serial.println();
}

void loop() {
    // Update M5 button states
    M5.update();
    
    bool isConnected = bleKeyboard.isConnected();
    
    // Connection state change notification
    if (isConnected != wasConnected) {
        wasConnected = isConnected;
        if (isConnected) {
            Serial.println("[BLE] Device connected!");
        } else {
            Serial.println("[BLE] Device disconnected");
        }
    }
    
    // Handle Button A - Send password
    if (M5.BtnA.wasPressed()) {
        handleButtonA();
    }
    
    // Handle Button B - Change slot
    if (M5.BtnB.wasPressed()) {
        handleButtonB();
    }
    
    // Process serial commands
    if (Serial.available()) {
        processSerialCommand();
    }
    
    // Update UI periodically
    if (millis() - lastUIUpdate > UI_UPDATE_INTERVAL_MS) {
        lastUIUpdate = millis();
        uiManager.update(
            isConnected,
            currentSlot,
            passwordManager.getLabel(currentSlot),
            passwordManager.hasPassword(currentSlot)
        );
    }
    
    // Small delay to prevent busy loop
    delay(10);
}

/**
 * @brief Handle Button A press - Send password
 */
void handleButtonA() {
    Serial.println("[BTN] Button A pressed");
    
    if (!bleKeyboard.isConnected()) {
        Serial.println("[BTN] Not connected - ignoring");
        uiManager.showError("Not connected");
        return;
    }
    
    if (!passwordManager.hasPassword(currentSlot)) {
        Serial.printf("[BTN] Slot %d has no password\n", currentSlot);
        uiManager.showError("No password set");
        return;
    }
    
    String password = passwordManager.getPassword(currentSlot);
    Serial.printf("[BTN] Sending password for slot %d (%d chars)\n", 
                  currentSlot, password.length());
    
    // Send the password
    bleKeyboard.print(password);
    
    // Optionally press Enter
    bleKeyboard.write(KEY_RETURN);
    
    // Visual feedback
    uiManager.showSendFeedback();
    
    Serial.println("[BTN] Password sent!");
}

/**
 * @brief Handle Button B press - Cycle to next slot
 */
void handleButtonB() {
    Serial.println("[BTN] Button B pressed");
    
    currentSlot = (currentSlot + 1) % MAX_PASSWORD_SLOTS;
    
    Serial.printf("[BTN] Switched to slot %d: %s\n", 
                  currentSlot, 
                  passwordManager.getLabel(currentSlot).c_str());
    
    uiManager.showSlotChangeFeedback();
}

/**
 * @brief Process commands from Serial monitor
 * 
 * Commands:
 * - SET <slot> <password>
 * - LABEL <slot> <label>
 * - LIST
 * - CLEAR <slot>
 * - CLEARALL
 * - HELP
 */
void processSerialCommand() {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) return;
    
    Serial.printf("> %s\n", input.c_str());
    
    // Parse command
    int firstSpace = input.indexOf(' ');
    String command = (firstSpace > 0) ? input.substring(0, firstSpace) : input;
    command.toUpperCase();
    String args = (firstSpace > 0) ? input.substring(firstSpace + 1) : "";
    
    if (command == "HELP") {
        printHelp();
    }
    else if (command == "LIST") {
        listSlots();
    }
    else if (command == "SET") {
        // SET <slot> <password>
        int spacePos = args.indexOf(' ');
        if (spacePos > 0) {
            int slot = args.substring(0, spacePos).toInt();
            String password = args.substring(spacePos + 1);
            
            if (slot >= 0 && slot < MAX_PASSWORD_SLOTS) {
                passwordManager.setPassword(slot, password);
                Serial.printf("OK: Password set for slot %d\n", slot);
            } else {
                Serial.println("ERROR: Invalid slot number (0-4)");
            }
        } else {
            Serial.println("ERROR: Usage: SET <slot> <password>");
        }
    }
    else if (command == "LABEL") {
        // LABEL <slot> <label>
        int spacePos = args.indexOf(' ');
        if (spacePos > 0) {
            int slot = args.substring(0, spacePos).toInt();
            String label = args.substring(spacePos + 1);
            
            if (slot >= 0 && slot < MAX_PASSWORD_SLOTS) {
                passwordManager.setLabel(slot, label);
                Serial.printf("OK: Label set for slot %d: %s\n", slot, label.c_str());
            } else {
                Serial.println("ERROR: Invalid slot number (0-4)");
            }
        } else {
            Serial.println("ERROR: Usage: LABEL <slot> <label>");
        }
    }
    else if (command == "CLEAR") {
        int slot = args.toInt();
        if (slot >= 0 && slot < MAX_PASSWORD_SLOTS) {
            passwordManager.clearSlot(slot);
            Serial.printf("OK: Slot %d cleared\n", slot);
        } else {
            Serial.println("ERROR: Invalid slot number (0-4)");
        }
    }
    else if (command == "CLEARALL") {
        passwordManager.clearAll();
        Serial.println("OK: All slots cleared");
    }
    else {
        Serial.printf("ERROR: Unknown command '%s'. Type HELP for usage.\n", command.c_str());
    }
}

/**
 * @brief Print help message
 */
void printHelp() {
    Serial.println();
    Serial.println("=== M5 PassKey Commands ===");
    Serial.println("  SET <slot> <password>  - Set password (slot 0-4)");
    Serial.println("  LABEL <slot> <label>   - Set label for slot");
    Serial.println("  LIST                   - Show all slots");
    Serial.println("  CLEAR <slot>           - Clear a slot");
    Serial.println("  CLEARALL               - Clear all slots");
    Serial.println("  HELP                   - Show this help");
    Serial.println();
    Serial.println("Example:");
    Serial.println("  SET 0 MySecretPassword123");
    Serial.println("  LABEL 0 Work PC");
    Serial.println();
}

/**
 * @brief List all configured password slots
 */
void listSlots() {
    Serial.println();
    Serial.println("=== Password Slots ===");
    for (int i = 0; i < MAX_PASSWORD_SLOTS; i++) {
        String label = passwordManager.getLabel(i);
        bool hasPass = passwordManager.hasPassword(i);
        String status = hasPass ? "[SET]" : "[EMPTY]";
        String current = (i == currentSlot) ? " <-- CURRENT" : "";
        
        Serial.printf("  Slot %d: %-15s %s%s\n", i, label.c_str(), status.c_str(), current.c_str());
    }
    Serial.println();
}
