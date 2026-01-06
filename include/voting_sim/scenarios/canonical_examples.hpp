#pragma once

#include "../scenario.hpp"

namespace voting_sim::scenarios {

// ============================================================================
// IRV (Instant Runoff Voting) Scenarios
// ============================================================================

// Simple majority case - candidate wins in first round
inline Scenario irv_simple_majority() {
    return {
        "IRV Simple Majority",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            {{0, 1, 2}},  // 3 votes
            {{0, 2, 1}},  // 3 votes
            {{0, 1, 2}},  // 3 votes
            {{1, 0, 2}},  // 1 vote
            {{1, 0, 2}},  // 1 vote
            {{2, 0, 1}}   // 1 vote
        },
        {0}, // Alice wins with majority (3/6 = 50%+)
        1
    };
}

// Requires elimination - no first-round majority
inline Scenario irv_elimination_needed() {
    return {
        "IRV Elimination Case",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            {{2, 1, 0}},  // Carol: 1
            {{2, 0, 1}},  // Carol: 1
            {{1, 0, 2}},  // Bob: 1
            {{1, 2, 0}},  // Bob: 1
            {{0, 2, 1}}   // Alice: 1
        },
        {2}, // Carol wins after Alice eliminated (Alice=1 eliminated, her vote goes to Carol)
        1
    };
}

// Classic example: 3 candidates, split vote - results in tie
inline Scenario irv_split_vote() {
    return {
        "IRV Split Vote",
        {
            {0, "Left"},
            {1, "Center"},
            {2, "Right"}
        },
        {
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // Left: 3
            {{2, 1, 0}}, {{2, 1, 0}}, {{2, 1, 0}}, // Right: 3
            {{1, 0, 2}}, {{1, 2, 0}}                // Center: 2
        },
        {}, // No winner - tie between Left and Right after Center eliminated
        1
    };
}

// ============================================================================
// Borda Count Scenarios
// ============================================================================

// Classic Borda example where Borda winner differs from plurality winner
inline Scenario borda_classic() {
    return {
        "Borda Classic Example",
        {
            {0, "A"},
            {1, "B"},
            {2, "C"}
        },
        {
            // 3 candidates, so points: 1st=2, 2nd=1, 3rd=0
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // A: 3 ballots, each gives A=2, B=1 → A=6, B=3
            {{1, 2, 0}}, {{1, 2, 0}}, {{1, 2, 0}}, // B: 3 ballots, each gives B=2, C=1, A=0 → B=6, C=3
            {{2, 0, 1}}, {{2, 0, 1}}                // C: 2 ballots, each gives C=2, A=1, B=0 → C=4, A=2
            // Total: A=8, B=9, C=7 → B wins
        },
        {1}, // B wins with 9 points
        1
    };
}

// Borda with clear winner
inline Scenario borda_clear_winner() {
    return {
        "Borda Clear Winner",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // Alice: 4×2 = 8
            {{1, 0, 2}}, {{1, 0, 2}}                             // Bob: 2×2 = 4
            // Carol: 0 (not ranked)
        },
        {0}, // Alice wins with 8 points
        1
    };
}

// ============================================================================
// STV (Single Transferable Vote) Scenarios
// ============================================================================

// Simple 2-winner STV example
inline Scenario stv_two_winners() {
    return {
        "STV Two Winners",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"},
            {3, "David"}
        },
        {
            // Quota = floor(10/3) + 1 = 4
            {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, // Alice: 4 (meets quota)
            {{1, 0, 2, 3}}, {{1, 0, 2, 3}}, {{1, 0, 2, 3}}, {{1, 0, 2, 3}}, // Bob: 4 (meets quota)
            {{2, 3, 0, 1}}, {{3, 2, 0, 1}}                                    // Carol: 1, David: 1
        },
        {0, 1}, // Alice and Bob both meet quota
        2
    };
}

// STV with surplus transfer
inline Scenario stv_surplus_transfer() {
    return {
        "STV Surplus Transfer",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            // Quota = floor(9/2) + 1 = 5
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // Alice: 6 (surplus)
            {{1, 0, 2}}, {{1, 0, 2}}, {{1, 0, 2}}                                            // Bob: 3
        },
        {0, 1}, // Alice elected, surplus transfers to Bob
        2
    };
}

// ============================================================================
// Schulze (Condorcet) Scenarios
// ============================================================================

// Classic Condorcet winner example
inline Scenario schulze_condorcet_winner() {
    return {
        "Schulze Condorcet Winner",
        {
            {0, "A"},
            {1, "B"},
            {2, "C"}
        },
        {
            // A beats B: 5-4, A beats C: 5-4, B beats C: 6-3
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 2, 1}}, {{0, 2, 1}}, // A first: 5
            {{1, 0, 2}}, {{1, 0, 2}}, {{1, 2, 0}}, {{1, 2, 0}}                // B first: 4
        },
        {0}, // A is Condorcet winner (beats both B and C)
        1
    };
}

// Condorcet paradox - no Condorcet winner
inline Scenario schulze_paradox() {
    return {
        "Schulze Condorcet Paradox",
        {
            {0, "A"},
            {1, "B"},
            {2, "C"}
        },
        {
            // A beats B: 5-4, B beats C: 5-4, C beats A: 5-4 (cycle!)
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // A>B>C: 5
            {{1, 2, 0}}, {{1, 2, 0}}, {{1, 2, 0}}, {{1, 2, 0}}                // B>C>A: 4
        },
        {0}, // Schulze resolves cycle (A has strongest path)
        1
    };
}

// ============================================================================
// Coombs' Method Scenarios
// ============================================================================

// Coombs differs from IRV - eliminates most disliked
inline Scenario coombs_classic() {
    return {
        "Coombs Classic",
        {
            {0, "Alice"},
            {1, "Bob"},
            {2, "Carol"}
        },
        {
            // First choices: Alice=3, Bob=2, Carol=2
            // Last choices: Alice=2, Bob=0, Carol=5
            {{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}, // Alice first, Carol last
            {{1, 0, 2}}, {{1, 0, 2}},                // Bob first, Carol last
            {{2, 1, 0}}, {{2, 1, 0}}                // Carol first, Alice last
        },
        {1}, // Bob wins (Carol eliminated first for most last-place votes=5, then Bob beats Alice 4-3)
        1
    };
}

// Coombs where last-place elimination matters
inline Scenario coombs_last_place_elimination() {
    return {
        "Coombs Last Place Elimination",
        {
            {0, "A"},
            {1, "B"},
            {2, "C"}
        },
        {
            {{0, 1, 2}}, {{0, 1, 2}}, // A first: 2, C last: 2
            {{1, 0, 2}}, {{1, 0, 2}}, // B first: 2, C last: 2
            {{2, 0, 1}}, {{2, 0, 1}}, {{2, 0, 1}} // C first: 3, A last: 3
        },
        {0}, // A wins (C eliminated for most last-place votes=4, then A beats B 5-2)
        1
    };
}

// ============================================================================
// Multi-Winner Scenarios
// ============================================================================

// Simple multi-winner Borda
inline Scenario borda_multi_winner() {
    return {
        "Borda Multi-Winner",
        {
            {0, "A"},
            {1, "B"},
            {2, "C"},
            {3, "D"}
        },
        {
            {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, // A: 3×3=9
            {{1, 0, 2, 3}}, {{1, 0, 2, 3}},                  // B: 2×3=6
            {{2, 0, 1, 3}}, {{2, 0, 1, 3}}                   // C: 2×3=6
        },
        {0, 1}, // Top 2: A and B (or C if tie-break)
        2
    };
}

} // namespace voting_sim::scenarios

