#include "password_manager.h"

PasswordManager::PasswordManager() : loaded(false) {
    // Initialize with default labels
    for (int i = 0; i < MAX_PASSWORD_SLOTS; i++) {
        labels[i] = "Slot " + String(i + 1);
        passwords[i] = "";
    }
}

void PasswordManager::init() {
    loadFromNVS();
    loaded = true;
    Serial.println("[PasswordManager] Initialized");
}

void PasswordManager::loadFromNVS() {
    preferences.begin(NVS_NAMESPACE, true); // Read-only mode
    
    for (int i = 0; i < MAX_PASSWORD_SLOTS; i++) {
        String pwdKey = getPasswordKey(i);
        String lblKey = getLabelKey(i);
        
        if (preferences.isKey(pwdKey.c_str())) {
            passwords[i] = preferences.getString(pwdKey.c_str(), "");
        }
        
        if (preferences.isKey(lblKey.c_str())) {
            labels[i] = preferences.getString(lblKey.c_str(), "Slot " + String(i + 1));
        }
        
        Serial.printf("[PasswordManager] Slot %d: %s (%s)\n", 
                      i, labels[i].c_str(), 
                      passwords[i].length() > 0 ? "configured" : "empty");
    }
    
    preferences.end();
}

void PasswordManager::saveToNVS(uint8_t slot) {
    if (slot >= MAX_PASSWORD_SLOTS) return;
    
    preferences.begin(NVS_NAMESPACE, false); // Read-write mode
    
    preferences.putString(getPasswordKey(slot).c_str(), passwords[slot]);
    preferences.putString(getLabelKey(slot).c_str(), labels[slot]);
    
    preferences.end();
    
    Serial.printf("[PasswordManager] Saved slot %d\n", slot);
}

String PasswordManager::getPassword(uint8_t slot) {
    if (slot >= MAX_PASSWORD_SLOTS) return "";
    return passwords[slot];
}

bool PasswordManager::setPassword(uint8_t slot, const String& password) {
    if (slot >= MAX_PASSWORD_SLOTS) return false;
    if (password.length() > MAX_PASSWORD_LENGTH) return false;
    
    passwords[slot] = password;
    saveToNVS(slot);
    return true;
}

String PasswordManager::getLabel(uint8_t slot) {
    if (slot >= MAX_PASSWORD_SLOTS) return "";
    return labels[slot];
}

bool PasswordManager::setLabel(uint8_t slot, const String& label) {
    if (slot >= MAX_PASSWORD_SLOTS) return false;
    if (label.length() > 20) return false;
    
    labels[slot] = label;
    saveToNVS(slot);
    return true;
}

uint8_t PasswordManager::getConfiguredSlots() {
    uint8_t count = 0;
    for (int i = 0; i < MAX_PASSWORD_SLOTS; i++) {
        if (passwords[i].length() > 0) count++;
    }
    return count;
}

bool PasswordManager::hasPassword(uint8_t slot) {
    if (slot >= MAX_PASSWORD_SLOTS) return false;
    return passwords[slot].length() > 0;
}

bool PasswordManager::clearSlot(uint8_t slot) {
    if (slot >= MAX_PASSWORD_SLOTS) return false;
    
    passwords[slot] = "";
    labels[slot] = "Slot " + String(slot + 1);
    saveToNVS(slot);
    return true;
}

void PasswordManager::clearAll() {
    preferences.begin(NVS_NAMESPACE, false);
    preferences.clear();
    preferences.end();
    
    for (int i = 0; i < MAX_PASSWORD_SLOTS; i++) {
        passwords[i] = "";
        labels[i] = "Slot " + String(i + 1);
    }
    
    Serial.println("[PasswordManager] All slots cleared");
}

String PasswordManager::getPasswordKey(uint8_t slot) {
    return "pwd_" + String(slot);
}

String PasswordManager::getLabelKey(uint8_t slot) {
    return "lbl_" + String(slot);
}
