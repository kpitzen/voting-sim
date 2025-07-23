#include "voting_sim/irv_election.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>

std::vector<int> IRVElection::runElection() {
    std::unordered_set<int> eliminated;
    std::vector<Ballot> submittedBallots = getBallots();
    std::vector<Candidate> submittedCandidates = getCandidates();
    int numVoters = submittedBallots.size();

    while (true) {
        std::unordered_map<int, int> voteCounts;

        for (const Ballot& ballot : submittedBallots) {
            for (int choice : ballot.rankedCandidates) {
                if (eliminated.count(choice) == 0) {
                    voteCounts[choice]++;
                    break;
                }
            }
        }


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
        for (const auto& [candidate, count] : voteCounts) {
            if (count == minVotes) {
                eliminated.insert(candidate);
            }
        }

        if (eliminated.size() == submittedCandidates.size())
        {
            return {}; // Tie / no winner
        }
    }
};
