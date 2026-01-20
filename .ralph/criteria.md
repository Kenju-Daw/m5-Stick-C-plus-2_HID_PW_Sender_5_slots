# Completion Criteria

## Build Criteria

| ID | Criterion | Command | Pass Condition |
|----|-----------|---------|----------------|
| B1 | Compile | `pio run` | Exit code 0 |
| B2 | Upload | `pio run -t upload` | Exit code 0 |
| B3 | No warnings | `pio run` | No compiler warnings |

## Runtime Criteria

| ID | Criterion | Test Method | Pass Condition |
|----|-----------|-------------|----------------|
| R1 | Boot | Power on | LCD shows UI |
| R2 | Serial | Send `HELP` | Help text returned |
| R3 | BLE Advertise | Windows BT scan | "M5-PassKey" visible |
| R4 | BLE Connect | Pair from Windows | Status shows "Connected" |
| R5 | Password Send | Press Button A | Text appears in Notepad |

## Verification Script

```bash
# Automated checks
pio run && echo "✅ B1: Build OK" || echo "❌ B1: Build FAILED"

# Manual checks (log results)
# R1-R5 require device testing
```

## Done When

```
B1 ✅ AND B2 ✅ AND R1 ✅ AND R2 ✅ AND R3 ✅
```

Minimum viable: B1 + B2 + R1 + R3
Full completion: All criteria
