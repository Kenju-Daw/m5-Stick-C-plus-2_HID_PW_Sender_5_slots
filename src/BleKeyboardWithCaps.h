#pragma once
#include <BleKeyboard.h>

/**
 * @brief Extended BleKeyboard class to detect host Caps Lock state
 * 
 * This class overrides the onWrite callback to intercept LED status reports
 * from the host (Windows/Mac/Linux). This allows us to know if Caps Lock
 * is active and adjust our typing accordingly.
 */
class BleKeyboardWithCaps : public BleKeyboard {
private:
    bool _capsLock = false;

public:
    BleKeyboardWithCaps(std::string deviceName = "ESP32 Keyboard", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100)
        : BleKeyboard(deviceName, deviceManufacturer, batteryLevel) {}

    /**
     * @brief Called when the host writes to a characteristic (e.g. LED status)
     * 
     * The LED Output report (Report ID 1) logic:
     * Bit 0: Num Lock
     * Bit 1: Caps Lock
     * Bit 2: Scroll Lock
     * Bit 3: Compose
     * Bit 4: Kana
     */
    void onWrite(BLECharacteristic* me) override {
        // Retrieve the value sent by the host
        std::string valueStr = me->getValue();
        if (valueStr.length() > 0) {
            uint8_t value = (uint8_t)valueStr[0];
            
            // update caps lock state (Bit 1)
            bool newCapsLock = (value & 0x02) != 0;
            
            if (_capsLock != newCapsLock) {
                _capsLock = newCapsLock;
                // Debug log (via Serial since we can't easily use ESP_LOG here without setup)
                // We'll leave it silent for now or use the base class log
            }
        }
        
        // Use the base class implementation for logging
        BleKeyboard::onWrite(me);
    }

    /**
     * @return true if Caps Lock is currently active on the host
     */
    bool isCapsLock() const {
        return _capsLock;
    }
};
