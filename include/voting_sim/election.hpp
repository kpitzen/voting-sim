#pragma once
#include "candidate.hpp"
#include "ballot.hpp"
#include <vector>

class Election {
private:
    std::vector<Candidate> candidates;
    std::vector<Ballot> ballots;
    int numWinners = 1;

public:
    virtual ~Election() = default;

    void setNumWinners(int numWinners_) { numWinners = numWinners_; }

    void addCandidate(const Candidate& candidate) {
        candidates.push_back(candidate);
    }

    void addBallot(const Ballot& ballot) {
        ballots.push_back(ballot);
    }

    std::vector<Candidate> getCandidates() {
        return candidates;
    }

    std::vector<Ballot> getBallots() {
        return ballots;
    }

    [[nodiscard]] const std::vector<Candidate>& getCandidates() const { return candidates; }
    [[nodiscard]] const std::vector<Ballot>& getBallots() const { return ballots; }
    [[nodiscard]] int getNumWinners() const { return numWinners; }

    // Return a vector of winning candidate IDs (size = K)
    virtual std::vector<int> runElection() = 0;

};
