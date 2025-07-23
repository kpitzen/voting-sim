#pragma once

#include "candidate.hpp"
#include "ballot.hpp"
#include <string>
#include <vector>

namespace voting_sim {

struct Scenario {
    std::string name;
    std::vector<Candidate> candidates;
    std::vector<Ballot> ballots;
    std::vector<int> expectedWinners;  // candidate IDs
    int numWinners = 1;
};

} // namespace voting_sim
