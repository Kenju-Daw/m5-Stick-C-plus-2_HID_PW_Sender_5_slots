# M5-PassKey Testing Guide

## Test-Driven Development Approach

This document outlines the testing procedures for M5-PassKey following TDD principles.

---

## Unit Tests (Conceptual)

Since ESP32 Arduino doesn't have native unit testing, we use a manual test checklist approach with clear pass/fail criteria.

---

## Test Checklist

### TC-001: Device Boot
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Power on M5 Stick | LCD displays UI | ☐ |
| 2 | Check Serial output | Version banner shown | ☐ |
| 3 | Observe LCD | Shows "Waiting..." status | ☐ |

### TC-002: BLE Advertising
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Open Windows Bluetooth settings | See "M5-PassKey" device | ☐ |
| 2 | Device should be discoverable | Listed as keyboard | ☐ |

### TC-003: BLE Pairing
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Click "Pair" on Windows | Pairing succeeds | ☐ |
| 2 | Check M5 Stick LCD | Shows "Connected" in green | ☐ |
| 3 | Check Serial output | "[BLE] Device connected!" logged | ☐ |

### TC-004: Password Storage (NVS)
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Send `SET 0 TestPass123` via Serial | "OK: Password set for slot 0" | ☐ |
| 2 | Send `LIST` via Serial | Slot 0 shows [SET] | ☐ |
| 3 | Power cycle device | - | ☐ |
| 4 | Send `LIST` via Serial | Slot 0 still shows [SET] | ☐ |

### TC-005: Password Sending
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Open Notepad on Windows | Cursor in text area | ☐ |
| 2 | Press Button A on M5 Stick | "TestPass123" + Enter in Notepad | ☐ |
| 3 | Observe M5 LCD | Green flash feedback | ☐ |
| 4 | Check Serial output | "[BTN] Password sent!" logged | ☐ |

### TC-006: Slot Cycling
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Note current slot on LCD | Shows slot number | ☐ |
| 2 | Press Button B | Slot increments | ☐ |
| 3 | Press Button B 4 more times | Wraps back to slot 0 | ☐ |

### TC-007: Empty Slot Protection
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Select empty slot (Button B) | LCD shows "(not set)" | ☐ |
| 2 | Press Button A | Error shown: "No password set" | ☐ |
| 3 | Nothing typed in Notepad | No output sent | ☐ |

### TC-008: Disconnected Protection  
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Turn off Bluetooth on Windows | - | ☐ |
| 2 | M5 LCD shows "Waiting..." | Yellow status | ☐ |
| 3 | Press Button A | Error: "Not connected" | ☐ |

### TC-009: Reconnection
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Turn Bluetooth back on | - | ☐ |
| 2 | Wait for auto-reconnect | LCD shows "Connected" | ☐ |
| 3 | Press Button A | Password sent successfully | ☐ |

### TC-010: Label Configuration
| Step | Action | Expected Result | Pass |
|------|--------|-----------------|------|
| 1 | Send `LABEL 0 Work PC` | "OK: Label set..." | ☐ |
| 2 | Select slot 0 | LCD shows "Work PC" | ☐ |

---

## Automated Build Test

```bash
# Run in project directory
pio run

# Expected: BUILD SUCCESSFUL
```

---

## Test Matrix

| Test ID | Boot | BLE | NVS | Buttons | LCD |
|---------|------|-----|-----|---------|-----|
| TC-001 | ✓ | | | | ✓ |
| TC-002 | | ✓ | | | |
| TC-003 | | ✓ | | | ✓ |
| TC-004 | | | ✓ | | |
| TC-005 | | ✓ | ✓ | ✓ | ✓ |
| TC-006 | | | | ✓ | ✓ |
| TC-007 | | | | ✓ | ✓ |
| TC-008 | | ✓ | | ✓ | ✓ |
| TC-009 | | ✓ | | ✓ | ✓ |
| TC-010 | | | ✓ | | ✓ |

---

## Known Issues Log

| ID | Issue | Status | Notes |
|----|-------|--------|-------|
| - | - | - | No issues yet |
