# NVS (Non-Volatile Storage) Schema Reference

## Overview

This document describes the NVS database schema used by M5-PassKey for persistent storage. NVS is ESP32's key-value storage system that survives power cycles.

---

## Namespace: `passkey`

All M5-PassKey data is stored under the `passkey` namespace.

### Password Storage

| Key | Type | Max Length | Description |
|-----|------|------------|-------------|
| `pwd_0` | String | 64 chars | Password for slot 0 |
| `pwd_1` | String | 64 chars | Password for slot 1 |
| `pwd_2` | String | 64 chars | Password for slot 2 |
| `pwd_3` | String | 64 chars | Password for slot 3 |
| `pwd_4` | String | 64 chars | Password for slot 4 |

### Label Storage

| Key | Type | Max Length | Description |
|-----|------|------------|-------------|
| `lbl_0` | String | 20 chars | Label for slot 0 |
| `lbl_1` | String | 20 chars | Label for slot 1 |
| `lbl_2` | String | 20 chars | Label for slot 2 |
| `lbl_3` | String | 20 chars | Label for slot 3 |
| `lbl_4` | String | 20 chars | Label for slot 4 |

---

## Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP32 Flash Memory                       │
│  ┌─────────────────────────────────────────────────────┐    │
│  │               NVS Partition                          │    │
│  │  ┌───────────────────────────────────────────────┐  │    │
│  │  │            Namespace: "passkey"                │  │    │
│  │  │                                                │  │    │
│  │  │   ┌──────────┬──────────┐                     │  │    │
│  │  │   │ pwd_0    │ "abc123" │  ← Password slot 0  │  │    │
│  │  │   ├──────────┼──────────┤                     │  │    │
│  │  │   │ lbl_0    │ "Work"   │  ← Label slot 0     │  │    │
│  │  │   ├──────────┼──────────┤                     │  │    │
│  │  │   │ pwd_1    │ "xyz789" │  ← Password slot 1  │  │    │
│  │  │   ├──────────┼──────────┤                     │  │    │
│  │  │   │ lbl_1    │ "Home"   │  ← Label slot 1     │  │    │
│  │  │   └──────────┴──────────┘                     │  │    │
│  │  └───────────────────────────────────────────────┘  │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

---

## API Usage

### Reading from NVS

```cpp
#include <Preferences.h>

Preferences preferences;
preferences.begin("passkey", true);  // true = read-only

String password = preferences.getString("pwd_0", "");  // "" = default
String label = preferences.getString("lbl_0", "Slot 1");

preferences.end();
```

### Writing to NVS

```cpp
preferences.begin("passkey", false);  // false = read-write

preferences.putString("pwd_0", "MySecretPassword");
preferences.putString("lbl_0", "Work PC");

preferences.end();
```

### Clearing Data

```cpp
preferences.begin("passkey", false);
preferences.clear();  // Clears all keys in namespace
preferences.end();
```

---

## Security Considerations

> [!WARNING]
> By default, NVS stores data in **plaintext**. For production use, consider:
> 1. Enabling NVS encryption in `menuconfig`
> 2. Using ESP32's flash encryption feature
> 3. Hashing passwords before storage (if feasible)

### Enable NVS Encryption

```bash
# In PlatformIO, add to platformio.ini:
build_flags = 
    -DCONFIG_NVS_ENCRYPTION=1
```

---

## Storage Limits

| Metric | Value | Notes |
|--------|-------|-------|
| Max key length | 15 chars | NVS limitation |
| Max string value | 4000 bytes | Per entry |
| Total NVS size | 20KB default | Can be resized |
| Write cycles | ~100,000 | Per sector |

---

## Testing NVS

### Manual Test via Serial

```
> SET 0 TestPassword123
OK: Password set for slot 0

> LABEL 0 Work PC
OK: Label set for slot 0: Work PC

> LIST
=== Password Slots ===
  Slot 0: Work PC         [SET] <-- CURRENT
  Slot 1: Slot 2          [EMPTY]
  Slot 2: Slot 3          [EMPTY]
  Slot 3: Slot 4          [EMPTY]
  Slot 4: Slot 5          [EMPTY]
```

### Persistence Test

1. SET password via Serial
2. Power cycle device
3. LIST should show password still configured
