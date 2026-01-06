#pragma once
#include "candidate.hpp"
#include "ballot.hpp"
#include "results.hpp"
#include <vector>

class Election {
private:
    std::vector<voting_sim::Candidate> candidates;
    std::vector<voting_sim::Ballot> ballots;
    int numWinners = 1;

public:
    virtual ~Election() = default;

    void setNumWinners(int numWinners_) { numWinners = numWinners_; }

    void addCandidate(const voting_sim::Candidate& candidate) {
        candidates.push_back(candidate);
    }

    void addBallot(const voting_sim::Ballot& ballot) {
        ballots.push_back(ballot);
    }

    std::vector<voting_sim::Candidate> getCandidates() {
        return candidates;
    }

    std::vector<voting_sim::Ballot> getBallots() {
        return ballots;
    }

    [[nodiscard]] const std::vector<voting_sim::Candidate>& getCandidates() const { return candidates; }
    [[nodiscard]] const std::vector<voting_sim::Ballot>& getBallots() const { return ballots; }
    [[nodiscard]] int getNumWinners() const { return numWinners; }

    // Return a vector of winning candidate IDs (size = K)
    virtual std::vector<int> runElection() = 0;

    // Return round-by-round history (empty if method doesn't use rounds)
    virtual const std::vector<voting_sim::RoundResult>& getRoundHistory() const {
        static const std::vector<voting_sim::RoundResult> empty;
        return empty;
    }

};
