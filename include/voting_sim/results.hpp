#pragma once
#include <map>
#include <vector>

namespace voting_sim {

struct RoundResult {
    int roundNumber;
    std::map<int, int> voteCounts;     // candidateID → vote count
    std::vector<int> eliminated;       // candidateIDs eliminated this round
};

} // namespace voting_sim
