# Mjolnir - Implemented Skills & Technical Capabilities

This document details the specific technical capabilities ("Skills") implemented in the Mjolnir project. This serves as a reference for future development and agent context.

## 1. Bluetooth Low Energy (BLE) Security
**Objective**: Prevent unauthorized connections and MITM attacks.

- **Man-in-the-Middle (MITM) Protection**:
    - Enabled `ESP_LE_AUTH_REQ_SC_MITM_BOND` capabilities.
    - Device Capabilities set to `ESP_IO_CAP_OUT` (Display Only) or `ESP_IO_CAP_IO` (DisplayYesNo) to force PIN comparison/display.
- **Passkey (PIN) Pairing**:
    - Generates a dynamic 6-digit PIN.
    - Displays PIN on the ST7789 screen.
    - User must enter/confirm this PIN on the host device (Windows/Mac).
- **Stealth Mode**:
    - Controls `esp_ble_gap_config_adv_data` to stop advertising when not in "Pairing Mode".
    - Device is invisible to scanners unless explicitly activated by the user.

## 2. Advanced User Interface (M5StickC Plus 2)
**Objective**: maximize usability with limited inputs (3 buttons).

- **Input Handling**:
    - **Short Press**: Standard action (Send Password, Next Slot).
    - **Long Press Detection**: Implemented logic to differentiate click vs. hold (>800ms). Used for brightness control and mode switching.
    - **Debouncing**: Software debouncing for reliable input.
- **Display Management**:
    - **PWM Brightness Control**: Uses LEDC (LED Controller) peripheral to adjust backlight brightness (25%, 50%, 75%, 100%).
    - **State-Based Rendering**: "Dirty" flag rendering system (`ui_update_needed`) to eliminate flicker by only redrawing changed pixels.

## 3. Persistent Storage (NVS)
**Objective**: Secure data retention across reboots.

- **NVS (Non-Volatile Storage)**:
    - Uses ESP32 `nvs_flash` partition.
    - Strings stored with keys `p0`...`p4` (Passwords) and `n0`...`n4` (Names).
    - Checks for free space and initializes partition automatically.

## 4. Power Efficiency State Machine
**Objective**: Extend battery life during inactivity.

- **Activity Timer**: Tracks time since last user interaction.
- **States**:
    - `ACTIVE`: Full brightness, full CPU.
    - `IDLE_DIM`: Reduced brightness (12%) after 30s.
    - `SLEEP`: Display off after 60s.
- **Wake Sources**: configured GPIO interrupts on Button A (G37) and Button B (G39).
