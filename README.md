# Ralph Wiggum Loop - Project Collection

## What is Ralph Wiggum Loop?

**Ralph Wiggum Loop** is an iterative AI development methodology popularized by Geoffrey Huntley. It uses continuous AI agent loops to automate software development through persistent iteration until completion criteria are met.

Key principles:
- 🔄 **Persistent Loop** - AI iterates until done
- 📁 **File-Based Context** - Progress stored in files/git, not AI memory
- ✅ **Defined Criteria** - Objective, testable completion conditions
- 🤖 **Autonomous Iteration** - AI self-corrects from errors

> See [docs/ralph_wiggum.md](docs/ralph_wiggum.md) for full methodology documentation.

---

## Overview

This repository contains ESP32-based utility projects following the Ralph Wiggum Loop methodology. Each project is self-contained, well-documented, and designed for AI-assisted development.

---

## Projects

### 1. M5-PassKey (Current)
**Device**: M5 Stick C Plus 2  
**Function**: BLE HID Bluetooth Keyboard for password entry

| Feature | Description |
|---------|-------------|
| BLE HID | Appears as Bluetooth keyboard to host |
| Password Slots | 5 configurable password slots |
| NVS Storage | Persistent password storage |
| LCD UI | Connection status and slot display |
| Serial Config | Configure via USB Serial |

---

## Project Ideas (Upcoming)

| Project | Device | Description |
|---------|--------|-------------|
| M5-TOTP | M5 Stick C Plus 2 | Time-based OTP generator |
| M5-Macro | M5 Stack Core | Programmable macro keyboard |
| M5-Beacon | M5 Atom | BLE beacon transmitter |
| M5-Sensor | M5 Stick C Plus 2 | Environmental sensor with Gemini API |

---

## Development Principles

1. **Test-Driven Development (TDD)** - Write tests first, code second
2. **AI-Friendly Documentation** - All code heavily commented for AI assistance
3. **NVS Database Schema** - Documented data structures for each project
4. **Modular Design** - Reusable components across projects

---

## Device Compatibility

| Device | ESP32 Chip | Display | Buttons | Notes |
|--------|-----------|---------|---------|-------|
| M5 Stick C Plus 2 | ESP32-PICO-V3-02 | 135x240 LCD | A, B, PWR | Main dev device |
| M5 Stack Core | ESP32 | 320x240 LCD | A, B, C | Larger screen |
| M5 Atom | ESP32-PICO | None | 1 | Compact |

---

## Getting Started

### Prerequisites
- PlatformIO IDE
- USB-C cable
- M5Stack device

### Quick Start
```bash
# Clone and build
cd ESP-GEM
pio run

# Upload to device
pio run -t upload

# Monitor serial output
pio device monitor
```

---

## Documentation Structure

```
docs/
├── nvs_schema.md      # NVS database schema reference
├── api_reference.md   # API documentation
└── testing.md         # Test procedures
```
