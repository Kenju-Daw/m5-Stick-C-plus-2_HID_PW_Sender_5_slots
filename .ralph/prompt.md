# M5-PassKey Task Prompt

## Objective
Build a BLE HID Bluetooth Keyboard using M5 Stick C Plus 2 that sends stored passwords on button press.

## Completion Criteria

The task is complete when ALL of the following are true:

1. **Build passes**: `pio run` exits with code 0
2. **Upload succeeds**: `pio run -t upload` exits with code 0
3. **Device boots**: LCD displays UI after power-on
4. **BLE advertises**: "M5-PassKey" appears in Windows Bluetooth settings
5. **Serial responds**: `HELP` command returns usage info

## Current Status

See `task.md` for detailed checklist.

## Context Files

Read these files to understand the project:

- [README.md](../README.md) - Project overview
- [docs/nvs_schema.md](nvs_schema.md) - Database schema
- [docs/testing.md](testing.md) - Test procedures
- [src/main.cpp](../src/main.cpp) - Main application
- [src/config.h](../src/config.h) - Configuration

## Iteration Instructions

If build fails:
1. Read error output carefully
2. Identify the failing file and line
3. Fix the issue
4. Commit with descriptive message
5. Re-run build

If tests fail:
1. Check which test case failed
2. Review the relevant source file
3. Fix the logic/implementation
4. Verify manually if needed
