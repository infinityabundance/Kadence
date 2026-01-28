#pragma once

#include <cstdint>

namespace kadence {

std::uint64_t now_monotonic_ns();

double ns_to_ms(std::uint64_t ns);
std::uint64_t ms_to_ns(double ms);

} // namespace kadence
