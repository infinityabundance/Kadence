# Quick Fix Guide - Critical Build Issue

## 🔴 CRITICAL: Core Library Not Building Correctly

### Problem
The repository contains fully implemented and working code for `SessionAnalyzer` and `TimeUtils`, but this code is **NOT being compiled** due to a bug in the CMakeLists.txt file.

### Impact
- ❌ Library builds successfully but has no functionality
- ❌ Daemon cannot use the analysis code (even though it exists)
- ❌ ~100 lines of working code are being ignored
- ⚠️ This gives a false impression that the library is working

### Current State
```cmake
# File: src/libkadence/CMakeLists.txt (CURRENT - WRONG)
add_library(kadence_core STATIC
    kadence_core.cpp          # ⚠️ This is just an empty stub!
)
# Missing: session_analyzer.cpp
# Missing: time_utils.cpp
```

### Fix (30 seconds)
```cmake
# File: src/libkadence/CMakeLists.txt (CORRECTED)
add_library(kadence_core STATIC
    kadence_core.cpp
    session_analyzer.cpp      # ADD THIS LINE
    time_utils.cpp            # ADD THIS LINE
)
```

---

## Step-by-Step Fix

### 1. Open the file
```bash
nano src/libkadence/CMakeLists.txt
# or
vim src/libkadence/CMakeLists.txt
# or use any text editor
```

### 2. Change lines 1-3 from:
```cmake
add_library(kadence_core STATIC
    kadence_core.cpp
)
```

### 3. To:
```cmake
add_library(kadence_core STATIC
    kadence_core.cpp
    session_analyzer.cpp
    time_utils.cpp
)
```

### 4. Save and rebuild
```bash
cd build
cmake --build .
```

---

## Verification

### Check that it worked:
```bash
# The library should be larger now (contains actual code)
ls -lh build/src/libkadence/libkadence_core.a

# Should show all three object files being compiled
cd build && cmake --build . --verbose
# Look for:
#   Building CXX object .../kadence_core.cpp.o
#   Building CXX object .../session_analyzer.cpp.o  ← NEW
#   Building CXX object .../time_utils.cpp.o        ← NEW
```

### Expected changes:
- **Before fix:** Library is ~2-5 KB (almost empty)
- **After fix:** Library is ~20-40 KB (contains actual code)

---

## Why This Matters

### What becomes available after the fix:

#### 1. SessionAnalyzer class
```cpp
#include "kadence/session_analyzer.hpp"

kadence::SessionAnalyzer analyzer;
kadence::Session session;

// Add frame timing samples
kadence::FrameSample sample{
    .timestamp_ns = kadence::now_monotonic_ns(),
    .frame_time_ms = 16.67f,
    .dropped = false
};

analyzer.add_sample(session, sample);

// Get computed statistics
std::cout << "Average FPS: " << session.stats.avg_fps << "\n";
std::cout << "1% Low FPS: " << session.stats.p1_low_fps << "\n";
std::cout << "0.1% Low FPS: " << session.stats.p01_low_fps << "\n";
std::cout << "Dropped frames (last sec): " << session.stats.dropped_last_sec << "\n";
```

#### 2. Time utilities
```cpp
#include "kadence/time_utils.hpp"

// Get current time in nanoseconds
auto now = kadence::now_monotonic_ns();

// Convert between time units
double milliseconds = kadence::ns_to_ms(1000000000);  // → 1000.0
uint64_t nanoseconds = kadence::ms_to_ns(16.67);      // → 16670000
```

---

## Next Steps After Fix

Once the library is building correctly:

### 1. Test the library
```cpp
// Create a test program: test_analyzer.cpp
#include <iostream>
#include "kadence/session_analyzer.hpp"
#include "kadence/time_utils.hpp"

int main() {
    kadence::SessionAnalyzer analyzer;
    kadence::Session session;
    
    // Simulate 60 FPS (16.67ms per frame)
    for (int i = 0; i < 100; i++) {
        kadence::FrameSample sample{
            .timestamp_ns = kadence::now_monotonic_ns(),
            .frame_time_ms = 16.67f,
            .dropped = false
        };
        analyzer.add_sample(session, sample);
    }
    
    std::cout << "Avg FPS: " << session.stats.avg_fps << "\n";
    return 0;
}
```

```bash
# Compile and run
g++ -std=c++20 -I include test_analyzer.cpp -L build/src/libkadence -lkadence_core -o test
./test
# Should output: Avg FPS: 59.988...
```

### 2. Use in the daemon
```cpp
// In src/daemon/kadenced.cpp, add:
#include "kadence/session_analyzer.hpp"
#include "kadence/time_utils.hpp"

// Create analyzer instance
kadence::SessionAnalyzer analyzer;
kadence::Session session;

// Feed frame timing data to analyzer
// (once you implement frame capture)
```

---

## Additional Recommendations

### Consider removing kadence_core.cpp
The `kadence_core.cpp` file currently only contains an empty function:
```cpp
void placeholder_core_link() {}
```

**Options:**
1. **Remove it** if not needed (recommended)
2. **Use it** for library initialization code
3. **Keep it** if needed for linking purposes

If removing:
```cmake
add_library(kadence_core STATIC
    session_analyzer.cpp
    time_utils.cpp
)
```

---

## Summary

| Aspect | Status |
|--------|--------|
| **Difficulty** | ⭐ Trivial (add 2 lines) |
| **Time Required** | 30 seconds |
| **Impact** | 🔴 CRITICAL |
| **Code Quality** | ✅ Already excellent |
| **Testing** | ⚠️ Should add unit tests |

**This is the single most important fix for the repository.** Without it, the repository contains working code that is completely unused.

---

See **ANALYSIS.md** for full details on what comes next after this fix.
