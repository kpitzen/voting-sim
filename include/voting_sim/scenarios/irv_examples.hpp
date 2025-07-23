#pragma once

#include "../scenario.hpp"

namespace voting_sim::scenarios {

inline Scenario simple_majority_irv() {
    return {
        "Simple IRV Majority",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            {{0, 1, 2}},
            {{0, 2, 1}},
            {{1, 0, 2}},
            {{1, 0, 2}},
            {{0, 1, 2}}
        },
        {0}, // Alice wins
        1
    };
}

inline Scenario needs_elimination_irv() {
    return {
        "Elimination IRV Case",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            {{2, 1, 0}},
            {{2, 0, 1}},
            {{1, 0, 2}},
            {{1, 2, 0}},
            {{0, 2, 1}}
        },
        {1}, // Bob wins after Carol eliminated
        1
    };
}

} // namespace voting_sim::scenarios
