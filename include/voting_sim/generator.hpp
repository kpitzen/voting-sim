#pragma once

#include "ballot.hpp"
#include <vector>
#include <optional>

namespace voting_sim {

    struct ClusteredPreferences {
        int numClusters;
        std::vector<std::vector<int>> anchors; // optional, can be empty
        double noise = 0.0;
    };

    std::vector<Ballot> generateRandomBallots(
        int numVoters,
        int numCandidates,
        std::optional<unsigned> seed = {},
        std::optional<int> minRankedCandidates = {},
        std::optional<int> maxRankedCandidates = {}
    );


    std::vector<Ballot> generateClusteredBallots(
        int numVoters,
        int numCandidates,
        const ClusteredPreferences& prefs,
        std::optional<unsigned> seed = {},
        std::optional<int> minRankedCandidates = {},
        std::optional<int> maxRankedCandidates = {}
    );

} // namespace voting_sim
