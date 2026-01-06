#include "voting_sim/stv_election.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <numeric>

std::vector<int> STVElection::runElection() {
    const auto& submittedCandidates = getCandidates();
    const auto& submittedBallots = getBallots();
    int numVoters = submittedBallots.size();
    int numWinners = getNumWinners();
    
    if (submittedCandidates.empty() || submittedBallots.empty() || numWinners <= 0) {
        return {};
    }
    
    if (numWinners >= static_cast<int>(submittedCandidates.size())) {
        // All candidates win
        std::vector<int> allCandidates;
        for (const auto& candidate : submittedCandidates) {
            allCandidates.push_back(candidate.id);
        }
        return allCandidates;
    }
    
    // Calculate Droop quota: floor(votes / (seats + 1)) + 1
    int quota = (numVoters / (numWinners + 1)) + 1;
    
    std::unordered_set<int> elected;
    std::unordered_set<int> eliminated;
    std::vector<std::vector<int>> ballotPreferences; // Store ballot preferences with weights
    
    // Initialize ballot preferences with weight 1.0
    for (const auto& ballot : submittedBallots) {
        ballotPreferences.push_back(ballot.rankedCandidates);
    }
    std::vector<double> ballotWeights(ballotPreferences.size(), 1.0);
    
    int round = 0;
    
    while (static_cast<int>(elected.size()) < numWinners) {
        round++;
        std::unordered_map<int, double> voteCounts;
        
        // Get all remaining candidates
        std::unordered_set<int> remaining;
        for (const auto& candidate : submittedCandidates) {
            if (elected.count(candidate.id) == 0 && eliminated.count(candidate.id) == 0) {
                remaining.insert(candidate.id);
            }
        }
        
        if (remaining.empty()) {
            break; // No more candidates
        }
        
        // Count votes for remaining candidates
        for (size_t i = 0; i < ballotPreferences.size(); i++) {
            if (ballotWeights[i] <= 0.0) continue; // Exhausted ballot
            
            for (int candidateId : ballotPreferences[i]) {
                if (remaining.count(candidateId) > 0) {
                    voteCounts[candidateId] += ballotWeights[i];
                    break;
                }
            }
        }
        
        // Record round result
        voting_sim::RoundResult result;
        result.roundNumber = round;
        for (const auto& [id, count] : voteCounts) {
            result.voteCounts[id] = static_cast<int>(count);
        }
        for (int id : elected) {
            result.voteCounts[id] = -1; // Mark as elected
        }
        roundHistory.push_back(result);
        
        // Check for candidates who meet quota
        bool progress = false;
        for (const auto& [candidateId, count] : voteCounts) {
            if (count >= quota && elected.count(candidateId) == 0) {
                elected.insert(candidateId);
                progress = true;
                
                // Transfer surplus
                double surplus = count - quota;
                if (surplus > 0.0 && count > 0.0) {
                    double transferRatio = surplus / count;
                    
                    // Transfer surplus proportionally to next preferences
                    for (size_t i = 0; i < ballotPreferences.size(); i++) {
                        if (ballotWeights[i] <= 0.0) continue;
                        
                        // Check if this ballot contributed to this candidate
                        bool contributed = false;
                        for (int prefId : ballotPreferences[i]) {
                            if (prefId == candidateId && remaining.count(prefId) > 0) {
                                contributed = true;
                                break;
                            }
                            if (remaining.count(prefId) > 0) break;
                        }
                        
                        if (contributed) {
                            // Reduce weight and transfer to next preference
                            double transferAmount = ballotWeights[i] * transferRatio;
                            ballotWeights[i] -= transferAmount;
                            
                            // Find next preference and add transfer
                            bool foundNext = false;
                            for (size_t j = 0; j < ballotPreferences[i].size(); j++) {
                                if (ballotPreferences[i][j] == candidateId) {
                                    // Find next non-eliminated, non-elected candidate
                                    for (size_t k = j + 1; k < ballotPreferences[i].size(); k++) {
                                        int nextId = ballotPreferences[i][k];
                                        if (remaining.count(nextId) > 0 && elected.count(nextId) == 0) {
                                            // This will be counted in next iteration
                                            foundNext = true;
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            // Note: The transfer is effectively applied by reducing the weight
                            // and the next iteration will count the reduced weight
                        }
                    }
                }
            }
        }
        
        if (progress) continue;
        
        // If no one meets quota, eliminate lowest candidate
        if (static_cast<int>(elected.size()) + static_cast<int>(remaining.size()) <= numWinners) {
            // Remaining candidates all win
            for (int candidateId : remaining) {
                elected.insert(candidateId);
            }
            break;
        }
        
        // Find candidate with fewest votes
        int minCandidate = -1;
        double minVotes = std::numeric_limits<double>::max();
        for (const auto& [candidateId, count] : voteCounts) {
            if (count < minVotes) {
                minVotes = count;
                minCandidate = candidateId;
            }
        }
        
        if (minCandidate != -1) {
            eliminated.insert(minCandidate);
            result.eliminated.push_back(minCandidate);
            // Update round history with elimination
            if (!roundHistory.empty()) {
                roundHistory.back().eliminated.push_back(minCandidate);
            }
        } else {
            break; // No progress possible
        }
    }
    
    std::vector<int> winners(elected.begin(), elected.end());
    std::sort(winners.begin(), winners.end());
    return winners;
}

