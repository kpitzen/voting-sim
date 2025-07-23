#include "voting_sim/generator.hpp"
#include "voting_sim/ballot.hpp"
#include <random>
#include <algorithm>

namespace voting_sim {

    int pickRankingLength(int minRank, int maxRank, std::mt19937& rng) {
        if (minRank == maxRank) {
            return minRank;
        }
        std::uniform_int_distribution<int> dist(minRank, maxRank);
        return dist(rng);
    }

    std::vector<Ballot> generateRandomBallots(
        int numVoters,
        int numCandidates,
        std::optional<unsigned> seed,
        std::optional<int> minRankedCandidates,
        std::optional<int> maxRankedCandidates
    ) {
        std::mt19937 rng(seed.has_value() ? seed.value() : std::random_device{}());
        int minRank = -1;
        int maxRank = -1;
        int length = -1;
        if (!minRankedCandidates && !maxRankedCandidates) {
            // default: full ballots
            length = minRank = maxRank = numCandidates;
        } else {
            minRank = minRankedCandidates.value_or(1);
            maxRank = maxRankedCandidates.value_or(numCandidates);
            maxRank = std::min(maxRank, numCandidates);
            minRank = std::clamp(minRank, 1, maxRank);
            length = pickRankingLength(minRank, maxRank, rng);
        }
        std::vector<Ballot> ballots;
        std::vector<int> base(numCandidates);

        for (int i = 0; i < numCandidates; ++i)
        {
            base.push_back(i);
        }

        for (int v = 0; v < numVoters; ++v) {
            std::vector<int> ranking = base;
            std::shuffle(ranking.begin(), ranking.end(), rng);
            ranking.resize(length);
            ballots.emplace_back(Ballot{ranking});
        }

        return ballots;
    }

    std::vector<Ballot> generateClusteredBallots(
        int numVoters,
        int numCandidates,
        const ClusteredPreferences& prefs,
        std::optional<unsigned> seed,
        std::optional<int> minRankedCandidates,
        std::optional<int> maxRankedCandidates
    ) {
        std::mt19937 rng(seed.has_value() ? seed.value() : std::random_device{}());
        std::uniform_int_distribution<int> clusterDist(0, prefs.numClusters - 1);
        std::uniform_real_distribution<double> noiseDist(0.0, 1.0);
        int minRank = -1;
        int maxRank = -1;
        int length = -1;
        if (!minRankedCandidates && !maxRankedCandidates) {
            // default: full ballots
            length = minRank = maxRank = numCandidates;
        } else {
            minRank = minRankedCandidates.value_or(1);
            maxRank = maxRankedCandidates.value_or(numCandidates);
            maxRank = std::min(maxRank, numCandidates);
            minRank = std::clamp(minRank, 1, maxRank);
            length = pickRankingLength(minRank, maxRank, rng);
        }

        // Create anchors if not provided
        std::vector<std::vector<int>> anchors = prefs.anchors;
        if (anchors.empty()) {
            for (int i = 0; i < prefs.numClusters; ++i) {
                std::vector<int> base(numCandidates);
                std::iota(base.begin(), base.end(), 0);
                std::shuffle(base.begin(), base.end(), rng);
                anchors.push_back(base);
            }
        }

        std::vector<Ballot> ballots;

        for (int v = 0; v < numVoters; ++v) {
            int clusterId = clusterDist(rng);
            std::vector<int> ranking = anchors[clusterId];

            // Add noise: randomly swap adjacent elements
            for (int i = 0; i + 1 < ranking.size(); ++i) {
                if (noiseDist(rng) < prefs.noise) {
                    std::swap(ranking[i], ranking[i + 1]);
                }
            }

            ranking.resize(length);
            ballots.emplace_back(Ballot{ranking});
        }

        return ballots;
    }

    Scenario generateScenario(
        std::string name,
        int numCandidates,
        int numVoters,
        int numWinners,
        const ClusteredPreferences& prefs,
        std::optional<int> minRanked,
        std::optional<int> maxRanked,
        std::optional<unsigned> seed
    ) {
        Scenario s;
        s.name = std::move(name);
        s.numWinners = numWinners;

        // Generate candidates
        for (int i = 0; i < numCandidates; ++i) {
            s.candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
        }

        // Generate ballots
        s.ballots = generateClusteredBallots(
            numVoters,
            numCandidates,
            prefs,
            seed,
            minRanked,
            maxRanked
        );

        // Leave expectedWinners empty for now — to be filled after evaluation
        return s;
    }

}
