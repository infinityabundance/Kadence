# Kadence Architecture: Current vs Required

## Current Architecture (What Exists)

```
┌─────────────────────────────────────────────────────────────┐
│                    CURRENT STATE                            │
└─────────────────────────────────────────────────────────────┘

 ┌──────────────┐      ┌──────────────┐      ┌──────────────┐
 │  kadenced    │      │  kadence-ui  │      │kadence-overlay│
 │              │      │              │      │              │
 │ ❌ STUB      │      │ ❌ STUB      │      │ ❌ STUB      │
 │ (8 lines)    │      │ (14 lines QML│      │ (16 lines QML│
 └──────────────┘      └──────────────┘      └──────────────┘
       │                     │                      │
       │                     │                      │
       └─────────────────────┴──────────────────────┘
                             │
                             ▼
                  ┌─────────────────────┐
                  │   libkadence_core   │
                  │                     │
                  │  ⚠️  INCOMPLETE     │
                  │                     │
                  │  ✅ session_analyzer │ (NOT BUILT)
                  │  ✅ time_utils       │ (NOT BUILT)
                  │  ❌ kadence_core     │ (empty stub)
                  └─────────────────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │  Header Files   │
                    │                 │
                    │  ✅ types.hpp    │
                    │  ✅ config.hpp   │
                    │  ✅ time_utils.hpp│
                    │  ✅ session_analyzer.hpp│
                    └─────────────────┘

❌ NO IPC CONNECTIONS
❌ NO FRAME CAPTURE
❌ NO DATA FLOW
```

---

## Required Architecture (What Should Exist)

```
┌─────────────────────────────────────────────────────────────┐
│                   TARGET ARCHITECTURE                        │
└─────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│  Gaming Processes                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                │
│  │  Game 1  │  │  Game 2  │  │  Game 3  │                │
│  │ (OpenGL) │  │ (Vulkan) │  │ (Wine)   │                │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘                │
└───────┼─────────────┼─────────────┼────────────────────────┘
        │ Frame       │ Frame       │ Frame
        │ Events      │ Events      │ Events
        ▼             ▼             ▼
┌────────────────────────────────────────────────────────────┐
│  Frame Capture Layer (MISSING ❌)                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │
│  │ Compositor  │  │ LD_PRELOAD  │  │ Vulkan      │       │
│  │ Hooks       │  │ Interception│  │ Layer       │       │
│  └─────────────┘  └─────────────┘  └─────────────┘       │
└──────────────────────┬─────────────────────────────────────┘
                       │ Raw timing data
                       ▼
        ┌──────────────────────────────────┐
        │      kadenced (STUB ❌)           │
        │                                   │
        │  ┌────────────────────────────┐  │
        │  │  Process Monitor (MISSING) │  │
        │  │  - Scan /proc              │  │
        │  │  - Detect game launches    │  │
        │  │  - Track PID lifecycle     │  │
        │  └────────────────────────────┘  │
        │              │                    │
        │              ▼                    │
        │  ┌────────────────────────────┐  │
        │  │  Frame Timing (MISSING)    │  │
        │  │  - Capture frame deltas    │  │
        │  │  - Detect dropped frames   │  │
        │  │  - Timestamp collection    │  │
        │  └────────────────────────────┘  │
        │              │                    │
        │              ▼                    │
        │  ┌────────────────────────────┐  │
        │  │  SessionAnalyzer ✅        │  │ <- IMPLEMENTED
        │  │  (NOT BUILT ⚠️)            │  │    BUT NOT COMPILED!
        │  │  - add_sample()            │  │
        │  │  - compute stats           │  │
        │  │  - trim buffer             │  │
        │  └────────────────────────────┘  │
        │              │                    │
        │              ▼                    │
        │  ┌────────────────────────────┐  │
        │  │  IPC Server (MISSING ❌)   │  │
        │  │  - D-Bus service           │  │
        │  │  - Unix socket             │  │
        │  │  - Shared memory           │  │
        │  └────────────────────────────┘  │
        └───────────┬──────────────────────┘
                    │ Session data
         ┌──────────┴──────────┐
         │ IPC (MISSING ❌)     │
         │                     │
         ▼                     ▼
┌──────────────────┐  ┌──────────────────┐
│  kadence-ui      │  │ kadence-overlay  │
│  (STUB ❌)       │  │ (STUB ❌)        │
│                  │  │                  │
│ MISSING:         │  │ MISSING:         │
│ - Session list   │  │ - FPS display    │
│ - Metrics view   │  │ - Frame graph    │
│ - Charts         │  │ - Positioning    │
│ - Settings       │  │ - Transparency   │
│ - IPC client     │  │ - IPC client     │
└──────────────────┘  └──────────────────┘
         │                     │
         │ User views data     │ In-game overlay
         ▼                     ▼
    ┌─────────┐          ┌─────────┐
    │  User   │          │  Gamer  │
    └─────────┘          └─────────┘
```

---

## Data Flow (Current vs Required)

### ❌ Current Data Flow (NONE)
```
Game Process
    │
    ▼
  (nothing happens - no capture mechanism)
    │
    ▼
  Daemon exits immediately
    │
    ▼
  UI/Overlay show "stub" text only
```

### ✅ Required Data Flow
```
Game Process (60 FPS)
    │
    ├─ Frame 1 @ 16.67ms ────┐
    ├─ Frame 2 @ 16.70ms ────┤
    ├─ Frame 3 @ 52.00ms ────┤ (dropped frame)
    └─ Frame 4 @ 16.65ms ────┤
                             │
                             ▼
                    Frame Capture Layer
                    (LD_PRELOAD / hooks)
                             │
                             ▼
                    FrameSample {
                        timestamp_ns: 123456789,
                        frame_time_ms: 16.67,
                        dropped: false
                    }
                             │
                             ▼
                  ┌──────────────────┐
                  │   Daemon         │
                  │                  │
                  │ SessionAnalyzer  │
                  │ .add_sample()    │
                  │                  │
                  │ Computes:        │
                  │ - Avg FPS: 59.8  │
                  │ - 1% low: 45.2   │
                  │ - 0.1% low: 38.1 │
                  │ - Dropped: 1     │
                  └────────┬─────────┘
                           │
                           ├─────────────┐
                           │             │
                  ┌────────▼───────┐ ┌──▼──────────┐
                  │  IPC (D-Bus)   │ │ IPC (D-Bus) │
                  │                │ │             │
                  │ Signals:       │ │ Methods:    │
                  │ - FrameUpdate  │ │ - GetStats  │
                  │ - SessionStart │ │ - ListSessions│
                  └────────┬───────┘ └──┬──────────┘
                           │             │
              ┌────────────┴─────────────┴─────────┐
              │                                     │
              ▼                                     ▼
    ┌─────────────────┐                 ┌─────────────────┐
    │  UI (Desktop)   │                 │  Overlay (Game) │
    │                 │                 │                 │
    │  Session List:  │                 │  ┌───────────┐ │
    │  ▢ Game1 59.8fps│                 │  │ 59.8 FPS  │ │
    │  ▣ Game2 active │                 │  │ 1%: 45.2  │ │
    │                 │                 │  │ ▂▄▆█▇▅▃▂  │ │
    │  Detail View:   │                 │  └───────────┘ │
    │  ╔══════════════╗│                 └─────────────────┘
    │  ║ ▂▄▆█▇▅▃▂▁▃▅ ║│                 (Semi-transparent
    │  ║  Frame Graph ║│                  overlay on game)
    │  ╚══════════════╝│
    └─────────────────┘
```

---

## Component Status Matrix

```
┌─────────────────┬──────────┬──────────┬─────────┬──────────┐
│ Component       │ Designed │ Implemented│ Built  │ Working │
├─────────────────┼──────────┼──────────┼─────────┼──────────┤
│ Data Structures │    ✅    │    ✅     │   ✅    │    ✅    │
│ SessionAnalyzer │    ✅    │    ✅     │   ❌    │    ❌    │
│ Time Utils      │    ✅    │    ✅     │   ❌    │    ❌    │
│ Frame Capture   │    ❌    │    ❌     │   ❌    │    ❌    │
│ Process Monitor │    ❌    │    ❌     │   ❌    │    ❌    │
│ Daemon Core     │    ⚠️    │    ❌     │   ⚠️    │    ❌    │
│ IPC Layer       │    ❌    │    ❌     │   ❌    │    ❌    │
│ UI - Session List│   ❌    │    ❌     │   ⚠️    │    ❌    │
│ UI - Charts     │    ❌    │    ❌     │   ❌    │    ❌    │
│ UI - Settings   │    ❌    │    ❌     │   ❌    │    ❌    │
│ Overlay Display │    ⚠️    │    ❌     │   ⚠️    │    ❌    │
│ Documentation   │    ❌    │    ❌     │   N/A   │    ❌    │
│ Tests           │    ❌    │    ❌     │   N/A   │    ❌    │
└─────────────────┴──────────┴──────────┴─────────┴──────────┘

Legend:
✅ = Complete
⚠️ = Partial (exists but incomplete)
❌ = Missing
```

---

## Integration Points (All Missing ❌)

### 1. Daemon ↔ Frame Capture
**Status:** ❌ Not implemented  
**Required:**
- Hook into compositor (Wayland/X11) for vsync events
- OR inject into game process (LD_PRELOAD)
- OR parse performance counters
- Extract frame timing data

### 2. Daemon ↔ libkadence
**Status:** ⚠️ Library exists but not used  
**Required:**
- Create SessionAnalyzer instance
- Call add_sample() for each captured frame
- Read computed statistics

### 3. Daemon ↔ UI (IPC)
**Status:** ❌ Not implemented  
**Required:**
- D-Bus service or Unix socket server
- Methods: get_sessions(), get_stats(pid)
- Signals: session_started, session_ended, stats_updated

### 4. Daemon ↔ Overlay (IPC)
**Status:** ❌ Not implemented  
**Required:**
- Same as UI, but optimized for low latency
- Real-time frame update signals (30-60 Hz)
- Minimal overhead (<1ms per update)

### 5. UI ↔ User
**Status:** ❌ Not implemented  
**Required:**
- QML views for session browsing
- Real-time chart updates
- Settings dialogs
- Start/stop controls

### 6. Overlay ↔ Gamer
**Status:** ❌ Not implemented  
**Required:**
- Always-on-top, frameless window
- FPS counter, frame graph
- Minimal CPU/GPU overhead
- Hotkey support

---

## Critical Path to Minimal Viable Product (MVP)

```
Step 1: Fix Build ⚠️ (1 hour)
   │
   ├─ Add session_analyzer.cpp to CMakeLists.txt
   ├─ Add time_utils.cpp to CMakeLists.txt
   └─ Verify library builds
   │
   ▼
Step 2: Implement Basic Frame Capture (2-3 days)
   │
   ├─ Choose capture method (compositor logs recommended)
   ├─ Implement process detection
   └─ Extract frame timestamps
   │
   ▼
Step 3: Integrate SessionAnalyzer (1 day)
   │
   ├─ Create analyzer instances
   ├─ Feed captured frames to add_sample()
   └─ Read computed statistics
   │
   ▼
Step 4: Implement Basic IPC (1 day)
   │
   ├─ D-Bus service setup
   ├─ Expose get_stats() method
   └─ Emit stats_updated signal
   │
   ▼
Step 5: Implement Basic UI (2 days)
   │
   ├─ Session list view
   ├─ Connect to daemon via IPC
   └─ Display real-time FPS
   │
   ▼
Step 6: Implement Basic Overlay (1 day)
   │
   ├─ FPS display widget
   ├─ Connect to daemon via IPC
   └─ Configure window flags
   │
   ▼
MVP Complete (7-8 days total) 🎉
```

---

## Files That Need Creation (Not Exist)

```
src/libkadence/
  ❌ frame_capture.hpp/cpp       (Frame capture interface)
  ❌ process_monitor.hpp/cpp     (Process detection)
  ❌ ipc_server.hpp/cpp          (D-Bus/socket server)

src/daemon/
  ❌ daemon_core.hpp/cpp         (Main daemon logic)
  ❌ session_manager.hpp/cpp     (Session lifecycle)

src/ui/
  ❌ qml/SessionList.qml         (Session browser)
  ❌ qml/SessionDetail.qml       (Metrics view)
  ❌ qml/SettingsDialog.qml      (Configuration)
  ❌ models/SessionModel.cpp     (Qt model for sessions)
  ❌ ipc/DaemonClient.cpp        (IPC client)

src/overlay/
  ❌ qml/FPSDisplay.qml          (FPS counter widget)
  ❌ qml/FrameGraph.qml          (Mini graph widget)
  ❌ ipc/DaemonClient.cpp        (IPC client)

tests/
  ❌ test_session_analyzer.cpp   (Unit tests)
  ❌ test_time_utils.cpp         (Unit tests)
  ❌ test_ipc.cpp                (Integration tests)

docs/
  ❌ BUILD.md                    (Build instructions)
  ❌ ARCHITECTURE.md             (Design documentation)
  ❌ API.md                      (API reference)
```

---

## Summary

**What Exists:** Solid foundation (data structures + analysis algorithm)  
**What's Missing:** Everything else (90% of functionality)  
**Critical Gap:** No frame capture, no IPC, no application logic  
**Time to MVP:** 7-8 days of focused development  
**Biggest Challenge:** Frame capture implementation (varies by approach)

See ANALYSIS.md for detailed technical analysis.
See STATUS.md for quick reference matrix.
