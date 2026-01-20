# Mjolnir - Secure HID Password Vault (M5 Stick C Plus 2)

**Mjolnir** is a secure, stealthy Bluetooth Low Energy (BLE) password vault built for the M5 Stick C Plus 2. It stores up to 5 passwords and types them into any computer (Windows, Mac, Linux, Android) at the press of a button.

![M5 Stick C Plus 2](https://static-cdn.m5stack.com/resource/docs/products/core/StickCPlus2/img-94a473cb-9be6-4a2b-88cd-f6db40f0d11c.webp)

## 🛡️ Key Features

- **🔐 5 Secure Slots**: Store and label up to 5 different passwords (e.g., Work, Personal, AWS).
- **👻 Stealth Mode**: The device remains **invisible** to Bluetooth scanners until you explicitly activate Pairing Mode.
- **🔢 PIN Pairing Protection**: Uses BLE Security (Man-In-The-Middle protection) requiring a 6-digit PIN matching, preventing unauthorized connections.
- **⚡ Smart Power Management**:
    - **Auto-Dim**: Screen dims to 12% brightness after 30s of inactivity.
    - **Auto-Sleep**: Display turns off after 60s. Wakes instantly on any button press.
    - **Brightness Control**: Long-press Button B to cycle brightness (25%, 50%, 75%, 100%).
- **⌨️ Universal Compatibility**: Works as a standard Generic HID Keyboard. No drivers required.
- **💾 Persistent Storage**: Passwords are saved in encrypted NVS (Non-Volatile Storage) and survive power cycles.
- **🔧 Easy Config**: Manage stored passwords via simple USB Serial commands.

## 🕹️ Controls

| Button | Action | Description |
| :--- | :--- | :--- |
| **Button A** (Front M5) | **Short Press** | **Type Password** for the current slot. |
| **Button B** (Side) | **Short Press** | **Next Slot** (Cycle through stored passwords). |
| **Button B** (Side) | **Long Press (>1s)** | **Change Brightness** (25% -> 50% -> 75% -> 100%). |
| **Power Button** | **Long Press (6s)** | Turn device ON/OFF. |

## 🚀 Getting Started

### 1. Installation
1. Install [PlatformIO](https://platformio.org/) (VSCode Extension).
2. Clone this repository:
   ```bash
   git clone https://github.com/Kenju-Daw/m5-Stick-C-plus-2_HID_PW_Sender_5_slots.git
   ```
3. Open the folder in VSCode.
4. Build and Upload:
   ```bash
   pio run -t upload
   ```

### 2. Configuration (Serial Mode)
Connect the device via USB and open a Serial Monitor (baud rate **115200**).
*Tip: You can use `pio device monitor`*

**Available Commands:**
```text
SET <slot> <password>   : Set password for slot (0-4)
LABEL <slot> <name>     : Set a friendly name for slot (0-4)
LIST                    : View all configured slots
CLEAR <slot>            : Delete password in slot
CLEARALL                : Factory reset all slots
HELP                    : Show this help menu
```

**Example:**
```text
> SET 0 SuperSecretPassword!
> LABEL 0 Google Account
> SET 1 Admin12345
> LABEL 1 Local Server
```

### 3. Pairing (First Time)
Because Mjolnir is stealthy, it won't show up in your Bluetooth list automatically.
1. **Wake** the device if it's sleeping.
2. The screen displays "Disconnected".
3. *(If not previously paired)* The device will advertise as **"Mjolnir"**.
4. On your PC, search for Bluetooth devices.
5. Select **Mjolnir**.
6. **PIN Check**: The M5 Stick will display a 6-digit PIN. Confirm it matches the one on your PC screen.
7. Once paired, the status changes to **"Connected"**.

## 📁 Project Structure

```
├── src/
│   ├── main.cpp              # Core application logic
│   ├── config.h              # Pin definitions & Global settings
│   ├── ui_manager.cpp        # Display & Graphics handling
│   ├── password_manager.cpp  # NVS Storage handling
│   └── ble_manager.cpp       # Bluetooth Security & HID logic
├── docs/
│   ├── EFFICIENCY_GUIDE.md   # Deep dive into Sleep/Power modes
│   ├── PROJECT_SKILLS.md     # Technical implementation details
│   └── ISSUES.md             # Bug tracker & Change log
├── platformio.ini            # Build Dependencies
└── README.md                 # This file
```

## 📜 License
MIT License.
