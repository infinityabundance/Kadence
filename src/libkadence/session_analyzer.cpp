#include "kadence/session_analyzer.hpp"

#include <algorithm>
#include <numeric>

#include "kadence/config.hpp"

namespace kadence {

void SessionAnalyzer::add_sample(Session& session, const FrameSample& sample)
{
    // Append new sample
    session.recent_samples.push_back(sample);

    // Trim to sliding window capacity
    trim_recent_samples(session);

    // Initialize start timestamp on first sample
    if (session.stats.start_timestamp_ns == 0) {
        session.stats.start_timestamp_ns = sample.timestamp_ns;
    }

    // Always update end timestamp
    session.stats.end_timestamp_ns = sample.timestamp_ns;

    // Recompute stats based on recent samples
    recompute_stats(session);
}

void SessionAnalyzer::trim_recent_samples(Session& session)
{
    if (session.recent_samples.size() <= RECENT_SAMPLE_CAPACITY) {
        return;
    }

    const auto excess = session.recent_samples.size() - RECENT_SAMPLE_CAPACITY;
    session.recent_samples.erase(session.recent_samples.begin(),
                                 session.recent_samples.begin() + static_cast<std::ptrdiff_t>(excess));
}

void SessionAnalyzer::recompute_stats(Session& session)
{
    auto& samples = session.recent_samples;
    auto& stats   = session.stats;

    if (samples.empty()) {
        stats.avg_fps       = 0.0;
        stats.p1_low_fps    = 0.0;
        stats.p01_low_fps   = 0.0;
        stats.dropped_last_sec = 0;
        return;
    }

    // ---- Average FPS ----
    double sum_ms = std::accumulate(
        samples.begin(), samples.end(), 0.0,
        [](double acc, const FrameSample& s) {
            return acc + static_cast<double>(s.frame_time_ms);
        });

    const std::size_t count = samples.size();
    const double avg_ms = (count > 0) ? (sum_ms / static_cast<double>(count)) : 0.0;

    if (avg_ms > 0.0) {
        stats.avg_fps = 1000.0 / avg_ms;
    } else {
        stats.avg_fps = 0.0;
    }

    // ---- 1% and 0.1% low FPS ----
    std::vector<float> frame_times_ms;
    frame_times_ms.reserve(samples.size());
    for (const auto& s : samples) {
        frame_times_ms.push_back(s.frame_time_ms);
    }

    // Sort descending: largest frame times (worst) first
    std::sort(frame_times_ms.begin(), frame_times_ms.end(), std::greater<float>());

    const std::size_t n = frame_times_ms.size();

    auto fps_from_ms = [](float ms) -> double {
        return (ms > 0.0f) ? (1000.0 / static_cast<double>(ms)) : 0.0;
    };

    // 1% low (worst 1% of frames)
    std::size_t idx_1 = 0;
    if (n > 1) {
        idx_1 = std::min<std::size_t>(n - 1, static_cast<std::size_t>(n * 0.01));
    }
    stats.p1_low_fps = fps_from_ms(frame_times_ms[idx_1]);

    // 0.1% low (worst 0.1% of frames)
    std::size_t idx_01 = 0;
    if (n > 1) {
        idx_01 = std::min<std::size_t>(n - 1, static_cast<std::size_t>(n * 0.001));
    }
    stats.p01_low_fps = fps_from_ms(frame_times_ms[idx_01]);

    // ---- Dropped frames in last second ----
    const std::uint64_t end_ns = stats.end_timestamp_ns;
    const std::uint64_t window_start =
        (end_ns > ONE_SECOND_NS) ? (end_ns - ONE_SECOND_NS) : 0;

    std::uint32_t dropped_last_sec = 0;
    for (const auto& s : samples) {
        if (s.timestamp_ns >= window_start &&
            s.frame_time_ms >= DROPPED_FRAME_THRESHOLD_MS)
        {
            ++dropped_last_sec;
        }
    }

    stats.dropped_last_sec = dropped_last_sec;
}

} // namespace kadence
