#include "voting_sim/generator.hpp"
#include <random>
#include <algorithm>

namespace voting_sim {

std::vector<Ballot> generateRandomBallots(int numVoters, int numCandidates, std::optional<unsigned> seed) {
    std::vector<Ballot> ballots;
    ballots.reserve(numVoters);
    std::vector<int> base;
    base.reserve(numVoters);

    std::mt19937 rng(seed.has_value() ? seed.value() : std::random_device{}());

    for (int i = 0; i < numCandidates; ++i)
    {
        base.push_back(i);
    }

    for (int v = 0; v < numVoters; ++v) {
        std::vector<int> ranking = base;
        std::shuffle(ranking.begin(), ranking.end(), rng);
        ballots.push_back(Ballot{ranking});
    }

    return ballots;
}

}
