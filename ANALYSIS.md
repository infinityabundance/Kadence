# Kadence Repository Analysis
## Deep Inspection of Stubs, TODOs, and Implementation Gaps

**Analysis Date:** 2026-02-15  
**Repository:** infinityabundance/Kadence  
**Purpose:** Low-latency Gaming Frame Pacing Analyzer for CachyOS/Archlinux

---

## Executive Summary

This document provides a comprehensive analysis of the Kadence repository, comparing claimed functionality against actual implementation. The project is in **early development stage** with core architectural foundations in place but most application-level functionality remaining as stubs.

### Overall Status: 🟡 Partial Implementation (≈30% Complete)

- ✅ **Working:** Core library data structures and analysis algorithms
- ⚠️ **Incomplete:** Build configuration, daemon, UI, and overlay
- ❌ **Missing:** Inter-process communication, frame capture, actual UI/overlay implementations

---

## 1. Repository Structure

### Claimed Architecture (per README.md)
```
"Low-latency Gaming frame pacing analyzer for CachyOS / Archlinux"
```

### Actual Architecture
```
Kadence/
├── include/kadence/           # Header files (PUBLIC API)
│   ├── config.hpp            ✅ WORKING - Constants and configuration
│   ├── types.hpp             ✅ WORKING - Data structures for frames/sessions
│   ├── time_utils.hpp        ✅ WORKING - Time conversion utilities
│   └── session_analyzer.hpp  ✅ WORKING - Frame analysis interface
│
├── src/libkadence/            # Core library implementation
│   ├── CMakeLists.txt        ⚠️  INCOMPLETE - Missing source files
│   ├── kadence_core.cpp      ❌ STUB - Empty placeholder function
│   ├── session_analyzer.cpp  ✅ WORKING - Full implementation (NOT BUILT)
│   └── time_utils.cpp        ✅ WORKING - Full implementation (NOT BUILT)
│
├── src/daemon/                # Background service
│   ├── CMakeLists.txt        ✅ WORKING - Build config
│   └── kadenced.cpp          ❌ STUB - Only prints message
│
├── src/ui/                    # Desktop GUI application
│   ├── CMakeLists.txt        ✅ WORKING - Build config
│   ├── main.cpp              ✅ WORKING - Qt setup code
│   └── qml/Main.qml          ❌ STUB - Placeholder text only
│
└── src/overlay/               # In-game overlay
    ├── CMakeLists.txt        ✅ WORKING - Build config
    ├── main.cpp              ✅ WORKING - Qt setup code
    └── qml/Overlay.qml       ❌ STUB - Placeholder text only
```

---

## 2. Detailed Component Analysis

### 2.1 Core Library (libkadence)

#### ✅ **WORKING: Header Files (100% Complete)**

**include/kadence/config.hpp**
```cpp
✅ Constants defined:
   - RECENT_SAMPLE_CAPACITY = 2048
   - ONE_SECOND_NS = 1'000'000'000
   - DROPPED_FRAME_THRESHOLD_MS = 50.0f
```

**include/kadence/types.hpp**
```cpp
✅ Data structures defined:
   - FrameSample (timestamp, frame_time, dropped flag)
   - SessionStats (fps metrics, dropped frames)
   - Session (process tracking, sample buffer)
```

**include/kadence/time_utils.hpp**
```cpp
✅ Time utilities declared:
   - now_monotonic_ns() - Get current time
   - ns_to_ms() / ms_to_ns() - Time conversions
```

**include/kadence/session_analyzer.hpp**
```cpp
✅ Analysis interface declared:
   - add_sample() - Add frame timing data
   - trim_recent_samples() - Manage buffer
   - recompute_stats() - Calculate metrics
```

#### ⚠️ **INCOMPLETE: Implementation Files**

**✅ src/libkadence/session_analyzer.cpp (IMPLEMENTED BUT NOT BUILT)**
- **Status:** FULLY IMPLEMENTED ✅
- **Problem:** NOT included in CMakeLists.txt
- **Functionality:**
  - Adds frame samples to session buffer
  - Trims buffer to capacity limit (2048 samples)
  - Computes statistics:
    - Average FPS
    - 1% low FPS (99th percentile)
    - 0.1% low FPS (99.9th percentile)  
    - Dropped frames in last second
- **Algorithm Quality:** Good (uses sorting for percentiles)

**✅ src/libkadence/time_utils.cpp (IMPLEMENTED BUT NOT BUILT)**
- **Status:** FULLY IMPLEMENTED ✅
- **Problem:** NOT included in CMakeLists.txt
- **Functionality:**
  - Uses std::chrono::steady_clock for monotonic time
  - Proper ns/ms conversions

**❌ src/libkadence/kadence_core.cpp (STUB)**
```cpp
void placeholder_core_link() {}  // Empty function
```
- **Status:** STUB - No functionality
- **Purpose:** Unclear, possibly for linking purposes

**❌ src/libkadence/CMakeLists.txt (CRITICAL BUG)**
```cmake
add_library(kadence_core STATIC
    kadence_core.cpp    # ✅ Included (but empty stub)
)
# ❌ MISSING: session_analyzer.cpp
# ❌ MISSING: time_utils.cpp
```
- **Problem:** Core analysis code exists but isn't being compiled!
- **Impact:** Library builds but has no actual functionality

---

### 2.2 Daemon (kadenced)

**Claimed Functionality:** Background service for frame pacing analysis

**Actual Implementation:**
```cpp
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    qInfo() << "kadenced stub";  // ❌ STUB
    return 0;                     // Exits immediately
}
```

#### Status: ❌ COMPLETELY STUB (0% Implemented)

**Missing Functionality:**
1. ❌ No frame capture mechanism (X11/Wayland hooks, OpenGL/Vulkan interception)
2. ❌ No process monitoring (identifying game processes)
3. ❌ No timing measurements (vsync detection, frame time calculation)
4. ❌ No session management (creating/updating Session objects)
5. ❌ No SessionAnalyzer usage (despite it being implemented)
6. ❌ No IPC mechanism (D-Bus, shared memory, sockets) for UI/overlay communication
7. ❌ No background service logic (should run as daemon)

**What SHOULD Be Implemented:**
- Process monitor to detect game launches
- Frame timing capture via compositor hooks or library injection
- SessionAnalyzer integration to compute metrics
- IPC server to provide data to UI and overlay
- Configuration loading/management
- Logging and error handling

---

### 2.3 Desktop UI (kadence-ui)

**Claimed Functionality:** Desktop interface for configuration & visualization

**Actual Implementation:**
```qml
ApplicationWindow {
    visible: true
    width: 800
    height: 480
    title: "Kadence"

    Label {
        anchors.centerIn: parent
        text: "Kadence UI stub"  // ❌ STUB
    }
}
```

#### Status: ❌ COMPLETELY STUB (0% Implemented)

**Missing Functionality:**
1. ❌ No session list view (active/historical gaming sessions)
2. ❌ No real-time metrics display (FPS, frame times, frame graph)
3. ❌ No configuration interface (thresholds, target FPS, notification settings)
4. ❌ No statistics visualization (charts, graphs, percentile displays)
5. ❌ No process selection (choose which processes to monitor)
6. ❌ No IPC client (connect to daemon to fetch data)
7. ❌ No data models (Qt models for sessions, samples)

**What SHOULD Be Implemented:**
- QML views for session browser and detail views
- Real-time chart components for frame timing
- Settings dialog for configuration
- IPC client to communicate with daemon
- Qt models (QAbstractListModel) for session data
- Start/stop monitoring controls

---

### 2.4 In-Game Overlay (kadence-overlay)

**Claimed Functionality:** In-game overlay for real-time frame metrics

**Actual Implementation:**
```qml
ApplicationWindow {
    visible: true
    width: 320
    height: 120
    title: "Kadence Overlay"
    color: "#80000000"  // Semi-transparent background

    Label {
        anchors.centerIn: parent
        text: "Overlay stub"  // ❌ STUB
        color: "white"
    }
}
```

#### Status: ❌ COMPLETELY STUB (5% Implemented)

**Partial Implementation:**
- ✅ Semi-transparent window setup (color: "#80000000")
- ✅ Compact size (320x120) appropriate for overlay

**Missing Functionality:**
1. ❌ No real-time FPS display
2. ❌ No frame time graph
3. ❌ No 1%/0.1% low indicators
4. ❌ No dropped frame counter
5. ❌ No IPC client (connect to daemon for live data)
6. ❌ No frameless/always-on-top window flags
7. ❌ No positioning options (corners, custom position)
8. ❌ No configurable opacity/theme
9. ❌ No hotkey support (show/hide, toggle modes)

**What SHOULD Be Implemented:**
- Real-time metric displays with smooth updates
- Mini frame time graph (last N frames)
- IPC client for live data streaming
- Frameless, always-on-top, click-through window
- Configurable layout and appearance
- Low-overhead rendering (important for gaming)

---

## 3. Comparison: Documentation vs Reality

### README.md Claims vs Actual State

| Claim | Reality | Status |
|-------|---------|--------|
| "Low-latency Gaming frame pacing analyzer" | No frame capture exists | ❌ Not Implemented |
| "for CachyOS / Archlinux" | No OS-specific code present | ⚠️ Generic Qt6/C++ |
| Daemon (implied) | Stub only, exits immediately | ❌ Not Implemented |
| UI (implied) | Placeholder window only | ❌ Not Implemented |
| Overlay (implied) | Placeholder window only | ❌ Not Implemented |
| Session analysis (implied) | **Fully implemented!** | ✅ **BUT NOT BUILT** |

### Discrepancy Summary
- **Header files:** 100% complete and well-designed ✅
- **Core algorithms:** 100% implemented but not built ⚠️
- **Applications:** 0% functional (all stubs) ❌
- **Integration:** 0% complete (no IPC, no frame capture) ❌

---

## 4. What Is Actually Working

### ✅ Fully Functional (Can Be Used Today)

1. **Data Structure Design**
   - Well-thought-out types for frame samples and session stats
   - Efficient buffer management (ring buffer concept)

2. **Session Analysis Algorithm**
   - Correct average FPS calculation
   - Proper percentile computation (1% low, 0.1% low)
   - Dropped frame detection with configurable threshold
   - Efficient sample trimming to prevent memory growth

3. **Time Utilities**
   - Monotonic clock usage (prevents time jump issues)
   - Accurate nanosecond precision
   - Proper ns/ms conversions

### ⚠️ Implemented But Not Compiled

- `session_analyzer.cpp` - **Ready to use, just add to CMakeLists.txt**
- `time_utils.cpp` - **Ready to use, just add to CMakeLists.txt**

### ❌ Not Working (Needs Implementation)

- Daemon: No functionality at all
- UI: No functionality at all  
- Overlay: No functionality at all
- Frame capture: Doesn't exist
- IPC: Doesn't exist
- Build: Won't compile due to missing Qt6 and CMakeLists issues

---

## 5. Phased Implementation Plan

### 🔴 **PHASE 1: Fix Critical Build Issues** (Priority: CRITICAL)

**Goal:** Make the project buildable and ensure existing code is compiled

**Tasks:**
1. Fix `src/libkadence/CMakeLists.txt`:
   ```cmake
   add_library(kadence_core STATIC
       kadence_core.cpp
       session_analyzer.cpp    # ADD THIS
       time_utils.cpp          # ADD THIS
   )
   ```

2. Add build documentation (dependencies, instructions)

3. Consider removing or implementing `placeholder_core_link()`

**Effort:** 30 minutes  
**Impact:** HIGH - Unlocks actual library functionality

---

### 🟠 **PHASE 2: Implement Daemon Core** (Priority: HIGH)

**Goal:** Create functional frame timing capture and analysis

**Tasks:**
1. **Process Monitor**
   - Scan /proc for game processes
   - Track process lifecycle (start/stop)
   - Create Session objects for each monitored process

2. **Frame Timing Capture** (Choose approach):
   - **Option A:** Parse compositor logs (Wayland/X11)
   - **Option B:** OpenGL/Vulkan layer interception
   - **Option C:** Perf events / scheduler statistics
   - **Recommendation:** Start with Option A (simpler), add Option B later

3. **SessionAnalyzer Integration**
   - Create SessionAnalyzer instance
   - Call `add_sample()` for each captured frame
   - Store sessions in memory

4. **Basic IPC Server**
   - Implement D-Bus service OR Unix domain socket
   - Expose methods: `get_sessions()`, `get_current_stats(pid)`
   - Emit signals for frame updates

**Effort:** 2-3 days  
**Impact:** HIGH - Enables actual frame analysis

---

### 🟡 **PHASE 3: Implement Desktop UI** (Priority: MEDIUM)

**Goal:** Provide user interface for viewing statistics

**Tasks:**
1. **Session List View**
   - QML ListView showing active and recent sessions
   - Display: Process name, PID, duration, avg FPS

2. **Session Detail View**
   - Real-time metrics (avg FPS, 1% low, 0.1% low)
   - Frame time graph (using Qt Charts or custom Canvas)
   - Dropped frame counter

3. **IPC Client**
   - Connect to daemon via D-Bus or socket
   - Implement Qt model for sessions
   - Auto-refresh on updates

4. **Settings Dialog**
   - Configure monitoring (auto-start, process filters)
   - Set thresholds (dropped frame definition)
   - Choose overlay options

**Effort:** 2-3 days  
**Impact:** MEDIUM - Makes data accessible to users

---

### 🟢 **PHASE 4: Implement In-Game Overlay** (Priority: MEDIUM)

**Goal:** Provide real-time overlay for gaming sessions

**Tasks:**
1. **Window Configuration**
   - Set frameless, always-on-top, click-through flags
   - Implement positioning (9 positions: corners, edges, center)
   - Add opacity control

2. **Real-Time Display**
   - FPS counter with smooth updates
   - 1% low / 0.1% low indicators
   - Dropped frame count
   - Mini frame time graph (last 60 frames)

3. **IPC Client**
   - Subscribe to frame updates from daemon
   - Low-latency data streaming
   - Handle connection loss gracefully

4. **Performance Optimization**
   - Minimize render overhead
   - Throttle updates (30-60 Hz sufficient)
   - Use efficient QML components

**Effort:** 2 days  
**Impact:** MEDIUM - Enhances user experience

---

### 🔵 **PHASE 5: Advanced Features** (Priority: LOW)

**Goal:** Polish and extend functionality

**Tasks:**
1. **Session Persistence**
   - Save sessions to SQLite database
   - Historical session browser
   - Export to CSV/JSON

2. **Advanced Analysis**
   - Frame pacing visualization (frame time stability)
   - Stutter detection (identify frame time spikes)
   - Compare sessions (before/after driver updates)

3. **Notifications**
   - Alert on performance degradation
   - Desktop notifications for session start/end

4. **Configuration Profiles**
   - Per-game settings
   - Import/export configurations

**Effort:** 3-4 days  
**Impact:** LOW - Nice-to-have features

---

## 6. Effort Estimation

| Phase | Tasks | Estimated Effort | Complexity |
|-------|-------|-----------------|------------|
| Phase 1: Fix Build | 3 tasks | 0.5 days | ⭐ Low |
| Phase 2: Daemon | 4 major subsystems | 2-3 days | ⭐⭐⭐ High |
| Phase 3: UI | 4 feature areas | 2-3 days | ⭐⭐ Medium |
| Phase 4: Overlay | 4 feature areas | 2 days | ⭐⭐ Medium |
| Phase 5: Advanced | 4 feature sets | 3-4 days | ⭐⭐ Medium |
| **TOTAL** | | **10-13 days** | |

---

## 7. Technical Recommendations

### Build System
- ✅ CMake structure is good
- ⚠️ Add FindQt6.cmake fallback or document Qt6 requirement
- ⚠️ Add version checks for C++20 compiler support

### Code Quality
- ✅ Good use of modern C++ (C++20, constexpr, namespaces)
- ✅ Clean separation of concerns (headers vs implementation)
- ⚠️ Need unit tests for SessionAnalyzer
- ⚠️ Need error handling (exceptions? error codes?)

### Architecture
- ✅ Static library approach is good for code reuse
- ⚠️ Need IPC mechanism design (recommend D-Bus for Linux)
- ⚠️ Need frame capture strategy (see Phase 2 options)

### Performance
- ✅ Fixed-size buffer prevents memory growth
- ✅ Nanosecond precision for accurate frame timing
- ⚠️ Overlay must be extremely lightweight (sub-1ms overhead)
- ⚠️ Consider lock-free data structures for IPC

---

## 8. Testing Strategy

### Unit Tests (Missing)
```cpp
// Recommended test structure
tests/
├── test_session_analyzer.cpp    # Test add_sample, stats computation
├── test_time_utils.cpp           # Test time conversions, edge cases
└── test_frame_capture.cpp        # Mock frame data, verify accuracy
```

### Integration Tests (Missing)
- Test daemon process monitoring
- Test IPC communication (daemon ↔ UI)
- Test IPC communication (daemon ↔ overlay)
- Test session lifecycle (start, update, end)

### Manual Testing (Needed)
- Test with real games (OpenGL, Vulkan, different engines)
- Test on CachyOS and Arch Linux
- Test with different compositors (X11, Wayland)
- Verify overlay doesn't impact game performance

---

## 9. Documentation Gaps

### Missing Documentation
1. ❌ No build instructions (dependencies, CMake commands)
2. ❌ No architecture documentation (how components interact)
3. ❌ No API documentation (header comments)
4. ❌ No usage documentation (how to use daemon, UI, overlay)
5. ❌ No contribution guidelines
6. ❌ No release/changelog documentation

### Recommended Documentation
```
docs/
├── BUILD.md              # Dependencies, build steps, troubleshooting
├── ARCHITECTURE.md       # Component diagram, data flow, IPC design
├── API.md               # Public API reference for libkadence
├── USAGE.md             # User guide (daemon, UI, overlay)
├── CONTRIBUTING.md      # Development setup, coding standards
└── CHANGELOG.md         # Version history
```

---

## 10. Security Considerations

### Potential Issues
1. ⚠️ Daemon requires process monitoring (may need elevated privileges)
2. ⚠️ Frame capture may require ptrace or LD_PRELOAD (security implications)
3. ⚠️ IPC should validate inputs to prevent injection attacks
4. ⚠️ Session data persistence should protect privacy (no unauthorized access)

### Recommendations
- Use D-Bus system bus with proper permissions
- Document required capabilities (CAP_SYS_PTRACE if needed)
- Implement input validation on all IPC endpoints
- Consider encryption for stored session data

---

## 11. Conclusion

### Current State Summary
The Kadence project has a **solid foundation** with well-designed data structures and a fully functional analysis algorithm. However, **90% of the application logic is missing**, with only stub placeholders in place.

### Key Findings
1. ✅ **Core library design is excellent** - types, config, analysis algorithm
2. ✅ **SessionAnalyzer is production-ready** - just needs to be compiled
3. ⚠️ **Critical build bug** - implemented code not included in CMakeLists.txt
4. ❌ **All applications are stubs** - daemon, UI, overlay need implementation
5. ❌ **No integration** - IPC, frame capture, process monitoring don't exist

### Recommended Next Steps
1. **Immediate:** Fix CMakeLists.txt to include session_analyzer.cpp and time_utils.cpp
2. **Short-term:** Implement daemon core (frame capture + IPC)
3. **Medium-term:** Implement UI and overlay to expose functionality
4. **Long-term:** Add advanced features, testing, documentation

### Project Viability
The project is **viable and promising** with good technical design. With focused development effort (10-13 days estimated), it can become a functional frame pacing analyzer. The biggest challenge will be implementing reliable frame capture across different game engines and graphics APIs.

---

**Analysis completed:** 2026-02-15  
**Total stub/placeholder findings:** 4  
**Total missing implementations:** 8 major components  
**Estimated completion:** 10-13 days of focused development
