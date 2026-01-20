# Issue Tracker - M5 Stick C Plus 2 HID Password Sender

## Open Issues

(None currently)

---

### ✅ BUG-004: Display does not wake up when connected
**Status**: Fixed  
**Priority**: High  
**Resolved**: 2026-01-21

**Fix Applied**:  
- Removed `!isConnected` condition from wake handler in `main.cpp`
- Ensures display wakes up on button press even if BLE is connected


---

## Closed Issues

### ✅ BUG-001: Enter Key Repeats Continuously After Password Send
**Status**: Fixed  
**Priority**: High  
**Resolved**: 2026-01-20

**Fix Applied**:  
Added 1.5 second cooldown after password send to prevent repeated Enter key presses.

---

### ✅ BUG-002: Display Flickers Constantly
**Status**: Fixed  
**Priority**: Medium  
**Resolved**: 2026-01-20

**Fix Applied**:  
- Only redraw screen when state actually changes
- Increased UI update interval from 100ms to 500ms

---

### ✅ FEATURE-001: Power Saving - Display Brightness Control
**Status**: Implemented  
**Priority**: Medium  
**Resolved**: 2026-01-20

**Implementation**:  
- Button B long-press cycles brightness (25%, 50%, 75%, 100%)
- Default brightness set to 50%
- Brightness indicator shown on change

---

### ✅ FEATURE-002: Display Auto-Off / Sleep Mode
**Status**: Implemented  
**Priority**: Medium  
**Resolved**: 2026-01-20

**Implementation**:  
- Auto-dim to 12% after 30 seconds of inactivity
- Auto-sleep (display off) after 60 seconds
- Wake on any button press
- Activity timer resets on interaction

---

### ✅ BUG-003: Caps Lock Inverts Characters
**Status**: Fixed  
**Priority**: High  
**Resolved**: 2026-01-20

**Fix Applied**:  
- Implemented `BleKeyboardWithCaps` class to detect host Caps Lock state
- Intercepts HID LED output reports from host
- Automatically inverts character case (a->A, A->a) when host Caps Lock is ON

