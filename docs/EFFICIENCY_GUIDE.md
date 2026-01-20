# Efficiency and Power Management Guide

This document explains the power management strategies used in the **Mjolnir (M5 Stick C Plus 2)** project and provides suggestions for further efficiency.

## Understanding ESP32 Sleep Modes

The ESP32 offers several power modes. Choosing the right one depends on the balance between power savings and responsiveness.

### 1. Active Mode
- **Status**: Wifi/BT on, CPU running.
- **Current**: ~80mA - 240mA (depends on RF usage).
- **Mjolnir Usage**: Used when typing passwords or connecting.

### 2. Modem Sleep (Automatic)
- **Status**: Wifi/BT radio switched off between beacon intervals, CPU running.
- **Current**: ~20mA - 40mA.
- **Mjolnir Usage**: Automatically managed by the ESP32 BLE stack when connected but idle.

### 3. Light Sleep
- **Status**: CPU paused, RAM retained, Peripherals paused. Digital peripherals (like UART/Timers) can wake the system.
- **Current**: ~0.8mA.
- **Wake-up Time**: Instant (< 1ms). Resumes execution exactly where it left off.
- **Mjolnir Usage**: This is the **current "Sleep" implementation**.
    - When the screen turns off after 60s, the device can enter Light Sleep.
    - Waking up via any GPIO (Button A/B) is simple.
    - **Note**: The display backlight is the biggest consumer (~10-50mA). Turning it off is the first and most effective step.

### 4. Deep Sleep
- **Status**: CPU off, Most RAM lost, Peripherals off. Only RTC (Real Time Controller) domain is active.
- **Current**: ~10µA - 150µA (extremely low).
- **Wake-up Time**: Slow. The device **reboots** upon waking (runs `setup()` again).
- **Data Retention**: Only data stored in `RTC_DATA_ATTR` variables or NVS is saved.
- **Mjolnir Usage Strategy**:
    - **Deep Sleep** gives the best battery life for long-term storage (days/weeks).
    - **Trade-off**: Since it resets the device, the BLE connection is lost. The PC will see the keyboard disconnect.
    - **Recommendation**: Use Deep Sleep only when the user explicitly turns the device "OFF" (e.g., via long-press Power button) or after a very long timeout (e.g., 5-10 minutes) where disconnecting is acceptable.

## Current Efficiency Implementation

The project currently uses a state-machine approach to save power:

1.  **Dimming (30s)**: PWM duty cycle reduced to 12%. Saves backlight power.
2.  **Display Off (60s)**: Backlight off. Logic continues running.
3.  **Future Improvement (Recommended)**:
    - Instead of just turning the display off, explicitly call `esp_light_sleep_start()` to pause the CPU.
    - Configure GPIO wake-up sources for Buttons A and B.

## Recommendations for Maximum Battery Life

1.  **Reduce CPU Frequency**: Drop from 240MHz to 80MHz or even 40MHz when not actively encrypting/sending.
    - *Impact*: Linear reduction in CPU power.
2.  **Aggressive Modem Sleep**: Ensure `esp_ble_tx_power_set` is optimized (lowest necessary power).
3.  **Deep Sleep for "Off" State**:
    - Implement a "Shutdown" feature.
    - Connect Power Button (AXP192 interrupt) or hold Button A to wake.
    - This allows the device to sit in a bag for months without dying.

## Comparison Table

| Mode | Current Consumption | Bluetooth Connection | Wake-up Style | Recommended For |
| :--- | :--- | :--- | :--- | :--- |
| **Active** | 80-100 mA | Stable | N/A | Active usage |
| **Display Off** | ~30-40 mA | Stable | Instant | Short pauses (< 1 min) |
| **Light Sleep** | ~1 mA | Paused (Keep-alive) | Instant (Resume) | Short breaks (1-5 mins) |
| **Deep Sleep** | ~0.01 mA | **Disconnected** | **Reboot** | Long storage (> 10 mins) |
