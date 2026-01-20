#pragma once
#include <M5StickCPlus2.h>
#include <BLEDevice.h>
#include "ui_manager.h"

// External reference to the UI Manager
extern UIManager uiManager;

class SecurityCallbacks : public BLESecurityCallbacks {
public:
    uint32_t onPassKeyRequest() {
        ESP_LOGI(LOG_TAG, "PassKeyRequest");
        return 123456;
    }

    void onPassKeyNotify(uint32_t pass_key) {
        ESP_LOGI(LOG_TAG, "The passkey Notify number:%d", pass_key);
        // Show the PIN on the display
        uiManager.setUIState(UIManager::STATE_SHOW_PIN); // Lock UI to PIN mode
        uiManager.showPairingPin(pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key) {
        ESP_LOGI(LOG_TAG, "The passkey yes/no number:%d", pass_key);
        return true;
    }

    bool onSecurityRequest() {
        ESP_LOGI(LOG_TAG, "SecurityRequest");
        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
        ESP_LOGI(LOG_TAG, "Starting BLE work!");
        if (cmpl.success) {
            Serial.println("Pairing success");
            // Use State Machine to show success screen non-blocking
            uiManager.setUIState(UIManager::STATE_PAIRING_SUCCESS);
        } else {
            Serial.println("Pairing failed");
            uiManager.showError("Pairing Failed");
            // If failed, return to stealth or pairing wait? 
            // Better to let main timeout handle it or go to stealth
            uiManager.setUIState(UIManager::STATE_STEALTH);
        }
    }
};
