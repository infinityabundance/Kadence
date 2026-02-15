# Kadence - Implementation Status Matrix

## Quick Reference: What Works vs What Doesn't

| Component | Claimed | Implemented | Built | Working | Status |
|-----------|---------|-------------|-------|---------|--------|
| **Core Library** | | | | | |
| ├─ config.hpp | ✅ | ✅ | ✅ | ✅ | 🟢 COMPLETE |
| ├─ types.hpp | ✅ | ✅ | ✅ | ✅ | 🟢 COMPLETE |
| ├─ time_utils.hpp/cpp | ✅ | ✅ | ❌ | ❌ | 🟡 IMPL, NOT BUILT |
| ├─ session_analyzer.hpp/cpp | ✅ | ✅ | ❌ | ❌ | 🟡 IMPL, NOT BUILT |
| └─ kadence_core.cpp | - | ⚠️ | ✅ | ⚠️ | 🟡 EMPTY STUB |
| **Daemon** | | | | | |
| └─ kadenced | ✅ | ❌ | ✅ | ❌ | 🔴 STUB ONLY |
| **Desktop UI** | | | | | |
| └─ kadence-ui | ✅ | ❌ | ✅ | ❌ | 🔴 STUB ONLY |
| **Overlay** | | | | | |
| └─ kadence-overlay | ✅ | ❌ | ✅ | ❌ | 🔴 STUB ONLY |

### Legend
- 🟢 **COMPLETE**: Fully implemented and working
- 🟡 **PARTIAL**: Implemented but has issues (not built, or empty)
- 🔴 **STUB**: Only placeholder code, no functionality

---

## Detailed Status

### 🟢 Working Code (Can Use Today)
1. **SessionAnalyzer algorithm** (76 lines of C++)
   - Frame sample collection
   - Average FPS calculation
   - 1% low and 0.1% low FPS (percentile-based)
   - Dropped frame detection
   - Sample buffer management (2048 capacity)

2. **Time utilities** (22 lines of C++)
   - Monotonic clock access
   - Nanosecond to millisecond conversion
   - Millisecond to nanosecond conversion

3. **Data structures** (types.hpp)
   - FrameSample, SessionStats, Session

4. **Configuration constants** (config.hpp)
   - Buffer sizes, thresholds, time constants

### 🟡 Partially Working

1. **libkadence CMakeLists.txt** - Builds but missing files
   ```cmake
   # Current:
   add_library(kadence_core STATIC
       kadence_core.cpp          # ⚠️ Empty stub
   )
   
   # Should be:
   add_library(kadence_core STATIC
       kadence_core.cpp
       session_analyzer.cpp      # ❌ MISSING
       time_utils.cpp            # ❌ MISSING
   )
   ```

### 🔴 Not Working (Stubs Only)

1. **kadenced.cpp** (8 lines)
   ```cpp
   int main(int argc, char *argv[]) {
       QCoreApplication app(argc, argv);
       qInfo() << "kadenced stub";    // ❌ STUB
       return 0;
   }
   ```
   **Missing:** Process monitoring, frame capture, IPC, analysis integration

2. **Main.qml** (14 lines)
   ```qml
   Label {
       text: "Kadence UI stub"       // ❌ STUB
   }
   ```
   **Missing:** Session list, metrics display, charts, IPC client, settings

3. **Overlay.qml** (16 lines)
   ```qml
   Label {
       text: "Overlay stub"          // ❌ STUB
   }
   ```
   **Missing:** FPS display, frame graph, IPC client, always-on-top config

---

## Code Statistics

```
Total Files: 13 source files
Total Lines: ~280 lines (excluding headers)

Working Code:      ~98 lines  (35%)
Stub/Placeholder:  ~16 lines  (6%)
Application Setup: ~166 lines (59%)
```

### Lines by Component
| Component | Lines | % Working |
|-----------|-------|-----------|
| session_analyzer.cpp | 76 | 100% ✅ |
| time_utils.cpp | 22 | 100% ✅ |
| kadenced.cpp | 8 | 0% ❌ |
| kadence-ui (main.cpp + qml) | 36 | 10% ⚠️ |
| kadence-overlay (main.cpp + qml) | 38 | 10% ⚠️ |
| kadence_core.cpp | 5 | 0% ❌ |

---

## Critical Issues

### 🔴 Issue #1: Core Library Not Built Correctly
**Impact:** HIGH - Implemented code is unused  
**Location:** `src/libkadence/CMakeLists.txt`  
**Problem:** session_analyzer.cpp and time_utils.cpp not in build  
**Fix:** Add 2 lines to CMakeLists.txt  
**Effort:** 1 minute

### 🔴 Issue #2: No Frame Capture
**Impact:** CRITICAL - Can't analyze frames without capturing them  
**Location:** Daemon  
**Problem:** No mechanism to intercept/measure frame timing  
**Fix:** Implement compositor hooks OR GL/Vulkan layer  
**Effort:** 1-2 days

### 🔴 Issue #3: No IPC
**Impact:** CRITICAL - Components can't communicate  
**Location:** All components  
**Problem:** Daemon can't send data to UI/overlay  
**Fix:** Implement D-Bus service or Unix sockets  
**Effort:** 1 day

### 🟡 Issue #4: All Applications Are Stubs
**Impact:** HIGH - No user-facing functionality  
**Location:** Daemon, UI, Overlay  
**Problem:** Only placeholder code exists  
**Fix:** Implement application logic per phased plan  
**Effort:** 6-8 days

---

## Repository Files

### Headers (include/kadence/)
```
config.hpp              ✅  13 lines  - Constants
types.hpp               ✅  37 lines  - Data structures  
time_utils.hpp          ✅  13 lines  - Time API
session_analyzer.hpp    ✅  21 lines  - Analysis API
```

### Core Library (src/libkadence/)
```
kadence_core.cpp        ⚠️   5 lines  - Empty stub
session_analyzer.cpp    ✅  76 lines  - FULL IMPLEMENTATION
time_utils.cpp          ✅  22 lines  - FULL IMPLEMENTATION
CMakeLists.txt          ⚠️  16 lines  - Missing source files
```

### Daemon (src/daemon/)
```
kadenced.cpp            ❌   8 lines  - Stub
CMakeLists.txt          ✅  12 lines  - Build config OK
```

### UI (src/ui/)
```
main.cpp                ✅  22 lines  - Qt setup OK
qml/Main.qml            ❌  14 lines  - Stub
CMakeLists.txt          ✅  23 lines  - Build config OK
```

### Overlay (src/overlay/)
```
main.cpp                ✅  22 lines  - Qt setup OK
qml/Overlay.qml         ❌  16 lines  - Stub  
CMakeLists.txt          ✅  21 lines  - Build config OK
```

---

## Missing Features by Component

### Daemon (kadenced)
- [ ] Process monitor (scan /proc, detect games)
- [ ] Frame capture (compositor hooks / GL interception)
- [ ] Timing measurement (vsync detection, frame deltas)
- [ ] SessionAnalyzer integration
- [ ] IPC server (D-Bus / sockets)
- [ ] Configuration loading
- [ ] Logging system
- [ ] Signal handling (graceful shutdown)

### Desktop UI (kadence-ui)
- [ ] Session list view (QML ListView)
- [ ] Session detail view (metrics, graph)
- [ ] Real-time updates (QML models)
- [ ] Frame time chart (Qt Charts / Canvas)
- [ ] Settings dialog (monitoring, thresholds)
- [ ] IPC client (connect to daemon)
- [ ] Start/stop controls
- [ ] Session export (CSV/JSON)

### Overlay (kadence-overlay)
- [ ] FPS counter display
- [ ] 1% low / 0.1% low indicators
- [ ] Dropped frame counter
- [ ] Mini frame time graph
- [ ] IPC client (live data stream)
- [ ] Window flags (frameless, always-on-top)
- [ ] Position configuration (9 positions)
- [ ] Opacity control
- [ ] Hotkey support (show/hide)
- [ ] Performance optimization (low overhead)

---

## Build Status

### Current Build
```bash
❌ Cannot build: Qt6 not found
⚠️  Would build incomplete library (missing analyzer code)
✅ CMake structure is correct
✅ C++20 standard configured
```

### Dependencies (Not Documented)
```
Required:
- CMake 3.20+
- C++20 compiler (GCC 10+, Clang 10+)
- Qt6 (Core, Gui, Qml, Quick, QuickControls2)

Recommended:
- D-Bus development libraries (for IPC)
- X11/Wayland development libraries (for frame capture)
```

---

## Next Steps Priority

### 🔴 CRITICAL (Do First)
1. Fix `src/libkadence/CMakeLists.txt` - Add missing source files
2. Document build dependencies and instructions
3. Test that library builds and links correctly

### 🟠 HIGH (Do Next)
4. Design IPC mechanism (recommend D-Bus)
5. Implement daemon process monitor
6. Implement frame capture (start with compositor logs)
7. Connect SessionAnalyzer to daemon

### 🟡 MEDIUM (Do After Core Works)
8. Implement UI session list and detail views
9. Implement overlay FPS display
10. Connect UI and overlay to daemon via IPC

### 🟢 LOW (Polish)
11. Add session persistence (database)
12. Add advanced visualizations
13. Add configuration profiles
14. Add unit tests

---

## Comparison to README.md

### What README Says
> "Low-latency Gaming frame pacing analyzer for CachyOS / Archlinux"

### What Actually Exists
- ✅ Data structures for frame analysis
- ✅ Frame pacing analysis algorithm  
- ❌ No frame capture
- ❌ No gaming integration
- ❌ No user interface
- ⚠️ Generic Qt6/C++ (not CachyOS-specific)

### Documentation vs Reality Gap
**Large disparity** between claimed functionality and actual implementation. The repository contains solid foundational code (30% complete) but all user-facing components are stubs (0% functional).

---

**Generated:** 2026-02-15  
**See also:** ANALYSIS.md for detailed technical analysis
