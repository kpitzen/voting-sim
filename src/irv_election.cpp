#include "voting_sim/irv_election.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>

std::vector<int> IRVElection::runElection() {
    std::unordered_set<int> eliminated;
    std::vector<voting_sim::Ballot> submittedBallots = getBallots();
    std::vector<voting_sim::Candidate> submittedCandidates = getCandidates();
    int numVoters = submittedBallots.size();
    roundHistory.clear();
    int round = 0;

    while (true) {
        round++;
        std::unordered_map<int, int> voteCounts;

        for (const voting_sim::Ballot& ballot : submittedBallots) {
            for (int choice : ballot.rankedCandidates) {
                if (!eliminated.contains(choice)) {
                    voteCounts[choice]++;
                    break;
                }
            }
        }

        // Record round result
        voting_sim::RoundResult result;
        result.roundNumber = round;
        for (const auto& [id, count] : voteCounts) {
            result.voteCounts[id] = count;
        }
        roundHistory.push_back(result);

        // After counting votes
        if (voteCounts.empty()) {
            return {}; // No remaining preferences → no winner
        }

        // Check for majority
        for (const auto& [candidate, count] : voteCounts) {
            if (count > numVoters / 2) {
                return { candidate };
            };
        }

        // Eliminate candidate(s) with fewest votes
        int minVotes = std::numeric_limits<int>::max();
        for (const auto& [candidate, count] : voteCounts) {
            minVotes = std::min(minVotes, count);
        }
        std::vector<int> toEliminate;
        for (const auto& [candidate, count] : voteCounts) {
            if (count == minVotes) {
                toEliminate.push_back(candidate);
            }
        }
        
        for (int candidate : toEliminate) {
            eliminated.insert(candidate);
            roundHistory.back().eliminated.push_back(candidate);
        }

        if (eliminated.size() == submittedCandidates.size())
        {
            return {}; // Tie / no winner
        }
    }
};
