#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <sys/types.h>

namespace kadence {

struct FrameSample {
    std::uint64_t timestamp_ns;
    float frame_time_ms;
    bool dropped;
};

struct SessionStats {
    std::uint64_t start_timestamp_ns = 0;
    std::uint64_t end_timestamp_ns = 0;

    double avg_fps = 0.0;
    double p1_low_fps = 0.0;
    double p01_low_fps = 0.0;
    std::uint32_t dropped_last_sec = 0;
};

struct Session {
    std::uint64_t id = 0;
    pid_t process_id = 0;
    std::string process_name;

    std::vector<FrameSample> recent_samples;

    SessionStats stats;
};

} // namespace kadence
