#include "voting_sim/coombs_election.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>

std::vector<int> CoombsElection::runElection() {
    const auto& submittedCandidates = getCandidates();
    const auto& submittedBallots = getBallots();
    int numVoters = submittedBallots.size();
    int numWinners = getNumWinners();
    
    if (submittedCandidates.empty() || submittedBallots.empty()) {
        return {};
    }
    
    std::unordered_set<int> eliminated;
    int round = 0;
    
    while (true) {
        round++;
        std::unordered_map<int, int> firstChoiceCounts;
        std::unordered_map<int, int> lastChoiceCounts;
        
        // Count first choices and last choices
        for (const auto& ballot : submittedBallots) {
            // Count first choice
            for (int choice : ballot.rankedCandidates) {
                if (eliminated.count(choice) == 0) {
                    firstChoiceCounts[choice]++;
                    break;
                }
            }
            
            // Count last choice (most disliked among remaining)
            std::vector<int> remainingInBallot;
            for (int choice : ballot.rankedCandidates) {
                if (eliminated.count(choice) == 0) {
                    remainingInBallot.push_back(choice);
                }
            }
            if (!remainingInBallot.empty()) {
                int lastChoice = remainingInBallot.back();
                lastChoiceCounts[lastChoice]++;
            }
        }
        
        if (firstChoiceCounts.empty()) {
            return {}; // No remaining preferences
        }
        
        // Record round result
        voting_sim::RoundResult result;
        result.roundNumber = round;
        for (const auto& [id, count] : firstChoiceCounts) {
            result.voteCounts[id] = count;
        }
        roundHistory.push_back(result);
        
        // Check for majority winner
        for (const auto& [candidate, count] : firstChoiceCounts) {
            if (count > numVoters / 2) {
                return { candidate };
            }
        }
        
        // Check if we have enough winners
        int remainingCandidates = static_cast<int>(submittedCandidates.size()) - static_cast<int>(eliminated.size());
        if (remainingCandidates <= numWinners) {
            std::vector<int> winners;
            for (const auto& candidate : submittedCandidates) {
                if (eliminated.count(candidate.id) == 0) {
                    winners.push_back(candidate.id);
                }
            }
            return winners;
        }
        
        // Eliminate candidate with most last-place votes
        int maxLastPlaceVotes = 0;
        for (const auto& [candidate, count] : lastChoiceCounts) {
            maxLastPlaceVotes = std::max(maxLastPlaceVotes, count);
        }
        
        std::vector<int> toEliminate;
        for (const auto& [candidate, count] : lastChoiceCounts) {
            if (count == maxLastPlaceVotes) {
                toEliminate.push_back(candidate);
            }
        }
        
        // If tie, eliminate the one with fewest first-choice votes
        if (toEliminate.size() > 1) {
            std::sort(toEliminate.begin(), toEliminate.end(),
                     [&firstChoiceCounts](int a, int b) {
                         return firstChoiceCounts[a] < firstChoiceCounts[b];
                     });
        }
        
        if (!toEliminate.empty()) {
            eliminated.insert(toEliminate[0]);
            roundHistory.back().eliminated.push_back(toEliminate[0]);
        } else {
            break; // No progress
        }
    }
    
    return {};
}

