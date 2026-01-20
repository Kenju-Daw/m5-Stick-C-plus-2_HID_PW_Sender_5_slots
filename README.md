# M5 Stick C Plus 2 - HID Password Sender (5 Slots)

A Bluetooth LE HID keyboard device that sends stored passwords with a button press.

![M5 Stick C Plus 2](https://static-cdn.m5stack.com/resource/docs/products/core/StickCPlus2/img-94a473cb-9be6-4a2b-88cd-f6db40f0d11c.webp)

## Features

- 🔑 **5 Password Slots** - Store up to 5 different passwords
- 📱 **BLE HID Keyboard** - Appears as a Bluetooth keyboard to your PC
- 🖥️ **LCD Display** - Shows connection status, battery level, and current slot
- ⚡ **One-Button Send** - Press Button A to type your password instantly
- 💾 **Persistent Storage** - Passwords saved in NVS (survives power cycles)
- 🔧 **Serial Configuration** - Easy setup via USB serial commands

## Hardware

| Component | Details |
|-----------|---------|
| Device | M5 Stick C Plus 2 |
| MCU | ESP32-PICO-V3-02 |
| Display | 135x240 ST7789 LCD |
| Buttons | A (front), B (side), Power |

## Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- USB-C cable

### Build & Upload
```bash
# Clone the repository
git clone https://github.com/Kenju-Daw/m5-Stick-C-plus-2_HID_PW_Sender_5_slots.git
cd m5-Stick-C-plus-2_HID_PW_Sender_5_slots

# Build
pio run

# Upload to device
pio run -t upload

# Monitor serial output
pio device monitor
```

## Usage

### Button Controls
| Button | Action |
|--------|--------|
| **A** (front) | Send password for current slot |
| **B** (side) | Cycle to next slot |

### Serial Commands
Connect via serial monitor (115200 baud) and use:

```
SET <slot> <password>   - Set password (slot 0-4)
LABEL <slot> <name>     - Set slot label
LIST                    - Show all slots
CLEAR <slot>            - Clear a slot
CLEARALL                - Clear all slots
HELP                    - Show help
```

### Example Setup
```
SET 0 MySecretPassword123
LABEL 0 Work PC
SET 1 AnotherPassword456
LABEL 1 Home Server
LIST
```

## Pairing

1. Power on the M5 Stick C Plus 2
2. Open Bluetooth settings on your PC
3. Look for **"M5-PassKey"** and pair
4. The LCD will show "Connected" when paired
5. Open any text field and press Button A to send your password

## Project Structure

```
├── src/
│   ├── main.cpp              # Main application
│   ├── config.h              # Configuration
│   ├── password_manager.cpp  # NVS password storage
│   ├── password_manager.h
│   ├── ui_manager.cpp        # LCD display handling
│   └── ui_manager.h
├── docs/
│   ├── nvs_schema.md         # Database schema
│   └── testing.md            # Test procedures
├── platformio.ini            # Build configuration
└── README.md
```

## License

MIT License - feel free to use and modify!

## Credits

- [M5Stack](https://m5stack.com/) for the M5 Stick C Plus 2
- [ESP32 BLE Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard) library
