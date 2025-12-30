#include "voting_sim/schulze_election.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>

std::vector<int> SchulzeElection::runElection() {
    const auto& submittedCandidates = getCandidates();
    const auto& submittedBallots = getBallots();
    int numWinners = getNumWinners();
    
    if (submittedCandidates.empty() || submittedBallots.empty()) {
        return {};
    }
    
    // Build pairwise preference matrix
    std::set<int> candidateIds;
    for (const auto& candidate : submittedCandidates) {
        candidateIds.insert(candidate.id);
    }
    
    std::unordered_map<int, std::unordered_map<int, int>> pairwise;
    
    // Initialize pairwise matrix
    for (int a : candidateIds) {
        for (int b : candidateIds) {
            if (a != b) {
                pairwise[a][b] = 0;
            }
        }
    }
    
    // Count pairwise preferences
    for (const auto& ballot : submittedBallots) {
        for (size_t i = 0; i < ballot.rankedCandidates.size(); i++) {
            int candidateA = ballot.rankedCandidates[i];
            if (candidateIds.count(candidateA) == 0) continue;
            
            for (size_t j = i + 1; j < ballot.rankedCandidates.size(); j++) {
                int candidateB = ballot.rankedCandidates[j];
                if (candidateIds.count(candidateB) == 0) continue;
                
                // A is preferred over B
                pairwise[candidateA][candidateB]++;
            }
        }
    }
    
    // Record pairwise results in round history
    voting_sim::RoundResult result;
    result.roundNumber = 1;
    // Store strongest pairwise victory for each candidate
    for (int candidate : candidateIds) {
        int maxVictory = 0;
        for (const auto& [opponent, votes] : pairwise[candidate]) {
            if (votes > pairwise[opponent][candidate]) {
                maxVictory = std::max(maxVictory, votes - pairwise[opponent][candidate]);
            }
        }
        result.voteCounts[candidate] = maxVictory;
    }
    roundHistory.push_back(result);
    
    // Compute strongest paths using Floyd-Warshall
    std::unordered_map<int, std::unordered_map<int, int>> strength;
    
    // Initialize strength matrix
    for (int a : candidateIds) {
        for (int b : candidateIds) {
            if (a == b) {
                strength[a][b] = 0;
            } else if (pairwise[a][b] > pairwise[b][a]) {
                strength[a][b] = pairwise[a][b];
            } else {
                strength[a][b] = 0;
            }
        }
    }
    
    // Floyd-Warshall algorithm
    for (int k : candidateIds) {
        for (int i : candidateIds) {
            if (i == k) continue;
            for (int j : candidateIds) {
                if (j == k || j == i) continue;
                strength[i][j] = std::max(strength[i][j], 
                                         std::min(strength[i][k], strength[k][j]));
            }
        }
    }
    
    // Find winners: candidates who beat all others in strongest path
    std::vector<int> winners;
    
    if (numWinners == 1) {
        // Single winner: find Condorcet winner
        for (int candidate : candidateIds) {
            bool beatsAll = true;
            for (int opponent : candidateIds) {
                if (candidate != opponent && strength[opponent][candidate] >= strength[candidate][opponent]) {
                    beatsAll = false;
                    break;
                }
            }
            if (beatsAll) {
                winners.push_back(candidate);
                break;
            }
        }
        
        // If no Condorcet winner, use Schulze ranking (candidate with most pairwise victories)
        if (winners.empty()) {
            std::vector<std::pair<int, int>> candidateScores;
            for (int candidate : candidateIds) {
                int victories = 0;
                for (int opponent : candidateIds) {
                    if (candidate != opponent && strength[candidate][opponent] > strength[opponent][candidate]) {
                        victories++;
                    }
                }
                candidateScores.emplace_back(victories, candidate);
            }
            std::sort(candidateScores.begin(), candidateScores.end(),
                     [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                         return a.first > b.first;
                     });
            if (!candidateScores.empty()) {
                winners.push_back(candidateScores[0].second);
            }
        }
    } else {
        // Multi-winner: select top K by Schulze score
        std::vector<std::pair<int, int>> candidateScores;
        for (int candidate : candidateIds) {
            int victories = 0;
            for (int opponent : candidateIds) {
                if (candidate != opponent && strength[candidate][opponent] > strength[opponent][candidate]) {
                    victories++;
                }
            }
            candidateScores.emplace_back(victories, candidate);
        }
        std::sort(candidateScores.begin(), candidateScores.end(),
                 [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                     if (a.first != b.first) return a.first > b.first;
                     return a.second < b.second;
                 });
        
        for (int i = 0; i < std::min(numWinners, static_cast<int>(candidateScores.size())); i++) {
            winners.push_back(candidateScores[i].second);
        }
    }
    
    return winners;
}

