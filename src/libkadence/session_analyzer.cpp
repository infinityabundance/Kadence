#include "kadence/session_analyzer.hpp"

#include <algorithm>
#include <numeric>

#include "kadence/config.hpp"

namespace kadence {

void SessionAnalyzer::add_sample(Session& session, const FrameSample& sample) {
    session.recent_samples.push_back(sample);
    trim_recent_samples(session);

    if (session.stats.start_timestamp_ns == 0) {
        session.stats.start_timestamp_ns = sample.timestamp_ns;
    }
    session.stats.end_timestamp_ns = sample.timestamp_ns;

    recompute_stats(session);
}

void SessionAnalyzer::trim_recent_samples(Session& session) {
    if (session.recent_samples.size() <= RECENT_SAMPLE_CAPACITY) {
        return;
    }

    const auto excess = session.recent_samples.size() - RECENT_SAMPLE_CAPACITY;
    session.recent_samples.erase(session.recent_samples.begin(),
                                 session.recent_samples.begin() + static_cast<std::ptrdiff_t>(excess));
}

void SessionAnalyzer::recompute_stats(Session& session) {
    if (session.recent_samples.empty()) {
        session.stats.avg_fps = 0.0;
        session.stats.p1_low_fps = 0.0;
        session.stats.p01_low_fps = 0.0;
        session.stats.dropped_last_sec = 0;
        return;
    }

    std::vector<float> frame_times;
    frame_times.reserve(session.recent_samples.size());
    for (const auto& sample : session.recent_samples) {
        frame_times.push_back(sample.frame_time_ms);
    }

    const double sum_frame_time = std::accumulate(frame_times.begin(), frame_times.end(), 0.0);
    const double avg_frame_time = sum_frame_time / static_cast<double>(frame_times.size());
    session.stats.avg_fps = avg_frame_time > 0.0 ? 1000.0 / avg_frame_time : 0.0;

    std::sort(frame_times.begin(), frame_times.end());
    const auto count = frame_times.size();
    const std::size_t idx_p1 = std::min(count - 1, static_cast<std::size_t>(count * 0.99));
    const std::size_t idx_p01 = std::min(count - 1, static_cast<std::size_t>(count * 0.999));

    const double p1_frame_time = frame_times[idx_p1];
    const double p01_frame_time = frame_times[idx_p01];

    session.stats.p1_low_fps = p1_frame_time > 0.0 ? 1000.0 / p1_frame_time : 0.0;
    session.stats.p01_low_fps = p01_frame_time > 0.0 ? 1000.0 / p01_frame_time : 0.0;

    const std::uint64_t window_start = session.stats.end_timestamp_ns >= ONE_SECOND_NS
        ? session.stats.end_timestamp_ns - ONE_SECOND_NS
        : 0;

    std::uint32_t dropped_last_sec = 0;
    for (const auto& sample : session.recent_samples) {
        if (sample.timestamp_ns >= window_start && sample.frame_time_ms >= DROPPED_FRAME_THRESHOLD_MS) {
            ++dropped_last_sec;
        }
    }

    session.stats.dropped_last_sec = dropped_last_sec;
}

} // namespace kadence
