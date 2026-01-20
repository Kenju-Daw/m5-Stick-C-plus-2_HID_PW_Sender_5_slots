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
#include "BleKeyboardWithCaps.h"
#include <BLEDevice.h>
#include <BLESecurity.h>
#include "esp_gap_ble_api.h"
#include "SecurityCallbacks.h"
#include "config.h"
#include "password_manager.h"
#include "ui_manager.h"
#include "version.h"

// Global objects
BleKeyboardWithCaps bleKeyboard(DEVICE_NAME, DEVICE_MANUFACTURER, 100);
PasswordManager passwordManager;
UIManager uiManager;

// Security Callbacks
SecurityCallbacks* securityCallbacks;

// State variables
uint8_t currentSlot = DEFAULT_PASSWORD_SLOT;
bool wasConnected = false;
unsigned long lastUIUpdate = 0;

// Security State
bool isAdvertising = false;
unsigned long advertisingStartTime = 0;
const unsigned long ADVERTISING_TIMEOUT_MS = 30000; // 30 seconds pairing window

// Function declarations
void handleButtonA();
void handleButtonB();
void handleButtonALongPress();
void processSerialCommand();
void printHelp();
void listSlots();
void startPairingMode();
void startWhitelistAdvertising();
void restoreWhitelist();
void enterDeepSleep();

void setup() {
    // Initialize M5StickCPlus2
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    delay(500);  // Longer delay for stability
    
    Serial.println();
    Serial.println("================================");
    Serial.println("  Mjolnir - BLE Password Sender");
    Serial.printf("  Version: %s\n", VERSION_STRING);
    Serial.printf("  Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("================================");
    Serial.println();
    
    // Initialize password manager
    passwordManager.init();
    
    // Initialize UI
    uiManager.init();
    
    // --- SECURITY SETUP ---
    Serial.println("[SEC] Initializing BLE Security...");
    
    // 1. Manually Init BLE Device first
    BLEDevice::init(DEVICE_NAME);
    
    // 2. Configure Encryption levels
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
    
    // 3. Set Custom Security Callbacks
    securityCallbacks = new SecurityCallbacks();
    BLEDevice::setSecurityCallbacks(securityCallbacks);
    
    // 4. Start BleKeyboard (starts advertising automatically)
    Serial.println("[BLE] Starting Bluetooth keyboard...");
    bleKeyboard.begin();
    
    // 5. Configure Security Parameters (must happen after BLE init)
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT); // Display Only (We show PIN)
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    
    // 6. AUTO-CONNECT MODE: Start Whitelist Advertising immediately
    Serial.println("[SEC] Starting Auto-Connect (Whitelist Advertising)");
    
    // We reuse the whitelist start logic but need to ensure it doesn't timeout instantly if loop hasn't started
    // Actually, calling startWhitelistAdvertising() sets everything up perfectly.
    startWhitelistAdvertising();
    
    Serial.println("[SETUP] Ready! Auto-connecting or Hold A+B to Pair.");
    Serial.println();
}

void loop() {
    M5.update();
    
    bool isConnected = bleKeyboard.isConnected();
    bool btnA_Pressed = M5.BtnA.wasPressed();
    bool btnB_Pressed = M5.BtnB.wasPressed();
    bool btnB_Held = M5.BtnB.pressedFor(LONG_PRESS_MS);
    bool anyInteraction = btnA_Pressed || btnB_Pressed; // Wake trigger

    // --- 1. HANDLE PAIRING MODE ACTIVATION (Hold A+B) ---
    if (!isConnected && M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
         if (!isAdvertising || uiManager.getUIState() != UIManager::STATE_PAIRING_WAIT) {
             startPairingMode(); // Force Open Advertising
         }
         advertisingStartTime = millis(); // Keep resetting timer while holding
         delay(100);
         return; 
    }

    // --- 2. HANDLE WAKE (Any Button) ---
    if (anyInteraction && uiManager.isSleeping()) {
        uiManager.wake();
        
        // If disconnected and stealth, try to reconnect
        if (!isConnected && !isAdvertising) {
             startWhitelistAdvertising();
        }
        
        // Prevent immediate action processing if we just woke up
        delay(100); 
        return; 
    }
    
    // --- 3. HANDLE ADVERTISING TIMEOUT (Stealth/Reconnect) ---
    if (isAdvertising) {
        bool isPairing = (uiManager.getUIState() == UIManager::STATE_PAIRING_WAIT);
        unsigned long timeout = isPairing ? ADVERTISING_TIMEOUT_MS : RECONNECT_WINDOW_MS;
        
        if (millis() - advertisingStartTime > timeout) {
            Serial.println("[SEC] Advertising Timeout -> Deep Sleep");
            BLEDevice::getAdvertising()->stop();
            isAdvertising = false;
            // Timeout from Reconnect/Pairing -> Go to sleep directly
            enterDeepSleep();
        }
    }
    // --- 4. HANDLE DISCONNECTED TIMEOUT (Stealth Mode) ---
    else if (!isConnected) {
        // If not advertising and not connected (Stealth Mode)
        // We use the last UI update or activity time
        unsigned long idleTime = millis() - uiManager.getLastActivityTime();
        if (idleTime > STEALTH_TIMEOUT_MS) {
             Serial.println("[PWR] Stealth Timeout -> Deep Sleep");
             enterDeepSleep();
        }
    }

    // --- 5. HANDLE CONNECTION CHANGES ---
    if (isConnected != wasConnected) {
        wasConnected = isConnected;
        uiManager.resetActivityTimer();
        if (isConnected) {
            Serial.println("[BLE] Device connected!");
            isAdvertising = false; 
            uiManager.setUIState(UIManager::STATE_CONNECTED);
            uiManager.update(true, currentSlot, passwordManager.getLabel(currentSlot), passwordManager.hasPassword(currentSlot));
        } else {
            Serial.println("[BLE] Device disconnected -> Stealth Mode");
            BLEDevice::getAdvertising()->stop(); // Ensure stops
            isAdvertising = false;
            uiManager.setUIState(UIManager::STATE_STEALTH);
        }
    }
    
    // --- 6. CHECK FOR CONNECTED DEEP SLEEP (Idle Timeout) ---
    if (isConnected) {
        unsigned long idleTime = millis() - uiManager.getLastActivityTime();
        if (idleTime > DEEP_SLEEP_MS) {
            Serial.println("[PWR] Idle Timeout -> Deep Sleep");
            enterDeepSleep();
        }
    }
    
    // --- 7. NORMAL OPERATION (Connected) ---
    if (isConnected) {
        static bool longPressTriggered = false;

        // Handle Button A - Short: Password, Long: Lock PC
        if (M5.BtnA.pressedFor(LONG_PRESS_MS)) {
             if (!longPressTriggered) {
                 Serial.println("[BTN] Button A LONG PRESS -> Lock PC");
                 handleButtonALongPress();
                 longPressTriggered = true;
             }
        }
        else if (M5.BtnA.wasReleased()) {
             if (!longPressTriggered) {
                 // Only send password if it wasn't a long press
                 handleButtonA();
             }
             longPressTriggered = false; // Reset for next interaction
        }
        else if (btnA_Pressed) {
            // Do NOTHING on initial press, wait for release or hold
        }
        
        if (btnB_Held) {
            uiManager.cycleBrightness();
            while (M5.BtnB.isPressed()) { M5.update(); delay(10); } 
        } else if (btnB_Pressed) {
            handleButtonB();
        }
        
        // Standard UI Update
        if (!uiManager.isSleeping()) {
            uiManager.update(
                isConnected,
                currentSlot,
                passwordManager.getLabel(currentSlot),
                passwordManager.hasPassword(currentSlot)
            );
        }
        
        // Reset activity on interaction
        if (anyInteraction) uiManager.resetActivityTimer();
    }
    
    // Process serial commands
    if (Serial.available()) {
        processSerialCommand();
        uiManager.resetActivityTimer();
    }
    
    // Power management check
    uiManager.checkPowerTimeout();
    
    delay(10);
}

void startPairingMode() {
    Serial.println("[SEC] Starting PAIRING Mode (Open Advertising)");
    BLEDevice::getAdvertising()->stop(); // Stop current if any
    
    // OPEN Advertising (Allow new connections)
    BLEDevice::getAdvertising()->setScanFilter(false, false); 
    
    BLEDevice::getAdvertising()->start();
    isAdvertising = true;
    advertisingStartTime = millis();
    uiManager.setUIState(UIManager::STATE_PAIRING_WAIT);
    uiManager.wake();
}

void restoreWhitelist() {
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num == 0) {
        Serial.println("[SEC] No bonded devices found.");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list) {
        Serial.println("[SEC] Failed to allocate memory for bond list");
        return;
    }

    esp_err_t err = esp_ble_get_bond_device_list(&dev_num, dev_list);
    if (err == ESP_OK) {
        Serial.printf("[SEC] restoring %d bonded devices to whitelist...\n", dev_num);
        for (int i = 0; i < dev_num; i++) {
            BLEAddress addr(dev_list[i].bd_addr);
            BLEDevice::whiteListAdd(addr);
            Serial.printf("[SEC] Added to Whitelist: %s\n", addr.toString().c_str());
        }
    } else {
        Serial.println("[SEC] Failed to get bond list");
    }
    
    free(dev_list);
}

void startWhitelistAdvertising() {
    Serial.println("[SEC] Starting RECONNECT Mode (Whitelist Advertising)");
    BLEDevice::getAdvertising()->stop(); // Stop current if any
    
    // 1. Restore bonded devices to internal whitelist
    restoreWhitelist();
    
    // 2. WHITELIST Advertising (Only previously bonded devices)
    // setScanFilter(scanRequestWhitelist, connectWhitelist)
    BLEDevice::getAdvertising()->setScanFilter(true, true);
    
    BLEDevice::getAdvertising()->start();
    isAdvertising = true;
    advertisingStartTime = millis();
    uiManager.setUIState(UIManager::STATE_RECONNECTING);
    uiManager.wake();
    uiManager.wake();
}

void enterDeepSleep() {
    Serial.println("[PWR] Entering Deep Sleep...");
    
    // Visual indicator
    M5.Display.fillScreen(COLOR_BACKGROUND);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_TEXT_SECONDARY, COLOR_BACKGROUND);
    M5.Display.setCursor(80, 60);
    M5.Display.print("Zzz...");
    delay(1000);
    
    M5.Display.setBrightness(0);
    M5.Display.sleep();
    
    // Configure wake sources: Button A (GP37) or B (GP39)
    // Buttons pulled HIGH, wake on LOW
    esp_sleep_enable_ext1_wakeup(((1ULL << 37) | (1ULL << 39)), ESP_EXT1_WAKEUP_ALL_LOW);
    
    esp_deep_sleep_start();
}

/**
 * @brief Handle Button A LONG press - Lock PC (Win+L)
 */
void handleButtonALongPress() {
    if (!bleKeyboard.isConnected()) {
        uiManager.showError("Not connected");
        return;
    }

    Serial.println("[BTN] Sending Win+L (Lock PC)");
    
    // Send Windows + L
    bleKeyboard.press(KEY_LEFT_GUI);
    bleKeyboard.press('l');
    delay(100);
    bleKeyboard.releaseAll();
    
    uiManager.showLockingFeedback();
}

/**
 * @brief Handle Button A press - Send password
 */
void handleButtonA() {
    // Cooldown protection to prevent repeated sends
    static unsigned long lastSendTime = 0;
    unsigned long now = millis();
    
    if (now - lastSendTime < PASSWORD_COOLDOWN_MS) {
        Serial.println("[BTN] Cooldown active - ignoring");
        return;
    }
    
    // Set cooldown IMMEDIATELY to prevent race condition
    lastSendTime = now;
    
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
    
    // Type password at human speed (~400 WPM = ~33 chars/sec = 30ms per char)
    for (int i = 0; i < password.length(); i++) {
        char c = password[i];

        // Smart Caps Lock handling: If host Caps Lock is ON, we must invert 
        // the case of letters we send so the host receives the intended case.
        if (bleKeyboard.isCapsLock()) {
            if (c >= 'a' && c <= 'z') {
                c = c - 32; // toupper: 'a' -> 'A' (Host Caps -> 'a')
            } else if (c >= 'A' && c <= 'Z') {
                c = c + 32; // tolower: 'A' -> 'a' (Host Caps -> 'A')
            }
        }

        bleKeyboard.write(c);
        delay(8);  // ~8ms delay (approx 125 chars/sec or 1500 WPM)
    }
    
    // Wait a moment before pressing Enter
    delay(100);
    
    // Press and RELEASE Enter explicitly (prevents stuck key)
    bleKeyboard.press(KEY_RETURN);
    delay(50);  // Hold for 50ms
    bleKeyboard.release(KEY_RETURN);
    
    // Ensure all keys released
    bleKeyboard.releaseAll();
    
    Serial.println("[BTN] Password sent!");
    
    // Visual feedback
    uiManager.showSendFeedback();
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
