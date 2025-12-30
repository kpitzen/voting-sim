#include "voting_sim/borda_election.hpp"
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <set>

std::vector<int> BordaElection::runElection() {
    const auto& submittedCandidates = getCandidates();
    const auto& submittedBallots = getBallots();
    int numCandidates = submittedCandidates.size();
    int numWinners = getNumWinners();
    
    if (numCandidates == 0 || submittedBallots.empty()) {
        return {};
    }
    
    // Get all candidate IDs
    std::set<int> candidateIds;
    for (const auto& candidate : submittedCandidates) {
        candidateIds.insert(candidate.id);
    }
    
    // Calculate Borda scores
    // Points: 1st choice = (numCandidates - 1), 2nd = (numCandidates - 2), ..., last = 0
    std::unordered_map<int, int> bordaScores;
    for (const auto& candidate : submittedCandidates) {
        bordaScores[candidate.id] = 0;
    }
    
    for (const auto& ballot : submittedBallots) {
        int position = 0;
        for (int candidateId : ballot.rankedCandidates) {
            if (candidateIds.count(candidateId) > 0) {
                int points = numCandidates - 1 - position;
                bordaScores[candidateId] += points;
                position++;
            }
        }
        // Candidates not ranked get 0 points (already initialized)
    }
    
    // Record round result
    voting_sim::RoundResult result;
    result.roundNumber = 1;
    for (const auto& [id, score] : bordaScores) {
        result.voteCounts[id] = score;
    }
    roundHistory.push_back(result);
    
    // Select top K winners
    std::vector<std::pair<int, int>> scores;
    for (const auto& [id, score] : bordaScores) {
        scores.emplace_back(score, id);
    }
    
    // Sort by score (descending), then by ID (ascending) for tie-breaking
    std::sort(scores.begin(), scores.end(), 
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  if (a.first != b.first) return a.first > b.first;
                  return a.second < b.second;
              });
    
    std::vector<int> winners;
    for (int i = 0; i < std::min(numWinners, static_cast<int>(scores.size())); i++) {
        winners.push_back(scores[i].second);
    }
    
    return winners;
}

