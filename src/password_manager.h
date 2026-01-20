#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

class PasswordManager {
public:
    PasswordManager();
    
    // Initialize and load passwords from NVS
    void init();
    
    // Get password for a specific slot
    String getPassword(uint8_t slot);
    
    // Set password for a specific slot
    bool setPassword(uint8_t slot, const String& password);
    
    // Get label for a slot
    String getLabel(uint8_t slot);
    
    // Set label for a slot
    bool setLabel(uint8_t slot, const String& label);
    
    // Get number of configured slots
    uint8_t getConfiguredSlots();
    
    // Check if a slot has a password
    bool hasPassword(uint8_t slot);
    
    // Clear a slot
    bool clearSlot(uint8_t slot);
    
    // Clear all slots
    void clearAll();

private:
    Preferences preferences;
    String passwords[MAX_PASSWORD_SLOTS];
    String labels[MAX_PASSWORD_SLOTS];
    bool loaded;
    
    String getPasswordKey(uint8_t slot);
    String getLabelKey(uint8_t slot);
    void loadFromNVS();
    void saveToNVS(uint8_t slot);
};

#endif // PASSWORD_MANAGER_H
