# Ralph Wiggum Loop - Development Methodology

## Overview

**Ralph Wiggum Loop** is an iterative AI development methodology popularized by Geoffrey Huntley in 2025-2026. It uses continuous AI agent loops to automate software development tasks through persistent iteration.

> "The AI agent works, attempts to stop, and if completion criteria are not met, the prompt is re-fed with updated context."

---

## Core Principles

### 1. Persistent Loop
The AI agent continuously iterates on a task until predefined completion criteria are satisfied.

```
┌─────────────────────────────────────────────────────────────┐
│                    RALPH WIGGUM LOOP                        │
│                                                             │
│   ┌──────────┐     ┌──────────┐     ┌──────────────┐       │
│   │  PROMPT  │────▶│ AI AGENT │────▶│ CHECK DONE?  │       │
│   └──────────┘     └──────────┘     └──────┬───────┘       │
│        ▲                                    │               │
│        │                                    ▼               │
│        │                           ┌───────────────┐        │
│        │              NO           │   CRITERIA    │        │
│        └───────────────────────────│     MET?      │        │
│                                    └───────┬───────┘        │
│                                            │ YES            │
│                                            ▼                │
│                                    ┌───────────────┐        │
│                                    │     DONE      │        │
│                                    └───────────────┘        │
└─────────────────────────────────────────────────────────────┘
```

### 2. Context Management
Progress is stored in **files and git history**, not in the AI's context window. This prevents "context rot" and allows fresh context each iteration.

### 3. Defined Completion Criteria
Success criteria must be **objective and verifiable**:
- ✅ All tests pass
- ✅ Build succeeds
- ✅ Lint passes
- ✅ Specific output tag detected

### 4. Autonomous Iteration
Unlike traditional "human-in-the-loop" coding, the AI self-corrects based on:
- Error messages
- Test failures
- Modified files
- Git diff history

---

## Implementation for This Project

### Completion Criteria for M5-PassKey

| Criterion | Check Command | Pass Condition |
|-----------|---------------|----------------|
| Build | `pio run` | Exit code 0 |
| Upload | `pio run -t upload` | Exit code 0 |
| Serial Test | Manual | Responds to commands |
| BLE Test | Manual | Pairs with Windows |

### Loop Script (Conceptual)

```bash
#!/bin/bash
# ralph_loop.sh - Ralph Wiggum Loop for M5-PassKey

MAX_ITERATIONS=10
ITERATION=0

while [ $ITERATION -lt $MAX_ITERATIONS ]; do
    # Run build
    pio run 2>&1 | tee build_output.txt
    
    if [ $? -eq 0 ]; then
        echo "✅ Build successful!"
        exit 0
    fi
    
    # Feed errors back to AI agent
    echo "❌ Build failed. Iteration $ITERATION"
    # AI agent would analyze build_output.txt and fix issues
    
    ITERATION=$((ITERATION + 1))
done

echo "⚠️ Max iterations reached"
exit 1
```

---

## Project Structure for Ralph Wiggum

```
ESP-GEM/
├── .ralph/                    # Ralph Wiggum Loop configuration
│   ├── prompt.md              # Main task prompt
│   ├── criteria.md            # Completion criteria
│   └── history.log            # Iteration history
├── docs/
│   ├── ralph_wiggum.md        # This file
│   ├── nvs_schema.md          # Database schema
│   └── testing.md             # Test procedures
├── src/                       # Source code
└── platformio.ini             # Build config
```

---

## Key Files for AI Context

When running in Ralph Wiggum mode, the AI should read these files at each iteration:

1. **task.md** - Current task checklist
2. **docs/nvs_schema.md** - Database schema
3. **docs/testing.md** - Test procedures
4. **build output** - Latest build errors
5. **git diff** - Recent changes

---

## Best Practices

### DO ✅
- Define clear, testable completion criteria
- Store all state in files (not AI context)
- Use git commits as checkpoints
- Set maximum iteration limits
- Log each iteration's changes

### DON'T ❌
- Use fuzzy/subjective criteria
- Rely on AI memory across iterations
- Skip error analysis
- Run infinite loops without limits
- Make breaking changes without tests

---

## References

- [Geoffrey Huntley's Ralph Plugin](https://github.com/ghuntley/ralph)
- [Dev.to: Ralph Wiggum Loop Explained](https://dev.to/)
- [Medium: AI Coding Loops](https://medium.com/)
