#pragma once

#include "ballot.hpp"
#include <vector>
#include <optional>

namespace voting_sim {

std::vector<Ballot> generateRandomBallots(int numVoters, int numCandidates, std::optional<unsigned> seed = {});

} // namespace voting_sim
