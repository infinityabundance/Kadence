#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <sys/types.h> // for pid_t

namespace kadence {

struct FrameSample {
    std::uint64_t timestamp_ns = 0;  // monotonic timestamp in ns
    float         frame_time_ms = 0; // frame time in milliseconds
    bool          dropped = false;   // true if considered "dropped"
};

struct SessionStats {
    std::uint64_t start_timestamp_ns = 0;  // timestamp of first sample
    std::uint64_t end_timestamp_ns   = 0;  // timestamp of most recent sample

    double avg_fps     = 0.0;  // average FPS over recent window
    double p1_low_fps  = 0.0;  // 1% low FPS
    double p01_low_fps = 0.0;  // 0.1% low FPS

    std::uint32_t dropped_last_sec = 0; // number of dropped frames in the last second
};

struct Session {
    std::uint64_t id         = 0;
    pid_t         process_id = 0;
    std::string   process_name;

    // Sliding buffer of recent samples for stats calculations.
    std::vector<FrameSample> recent_samples;

    SessionStats stats;
};

} // namespace kadence
