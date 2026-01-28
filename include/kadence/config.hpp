#pragma once

#include <cstddef>
#include <cstdint>

namespace kadence {

constexpr std::size_t RECENT_SAMPLE_CAPACITY = 2048;
constexpr std::uint64_t ONE_SECOND_NS = 1'000'000'000ULL;
constexpr float DROPPED_FRAME_THRESHOLD_MS = 50.0f;

} // namespace kadence
