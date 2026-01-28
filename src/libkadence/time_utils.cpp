#include "kadence/time_utils.hpp"

#include <chrono>

namespace kadence {

std::uint64_t now_monotonic_ns() {
    const auto now = std::chrono::steady_clock::now();
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    return static_cast<std::uint64_t>(ns.count());
}

double ns_to_ms(std::uint64_t ns) {
    return static_cast<double>(ns) / 1'000'000.0;
}

std::uint64_t ms_to_ns(double ms) {
    return static_cast<std::uint64_t>(ms * 1'000'000.0);
}

} // namespace kadence
