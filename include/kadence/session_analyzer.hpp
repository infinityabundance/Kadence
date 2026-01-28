#pragma once

#include <vector>

#include "kadence/types.hpp"

namespace kadence {

class SessionAnalyzer {
public:
    SessionAnalyzer() = default;

    void add_sample(Session& session, const FrameSample& sample);

private:
    void trim_recent_samples(Session& session);
    void recompute_stats(Session& session);
};

} // namespace kadence
