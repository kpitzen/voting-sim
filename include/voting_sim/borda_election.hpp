#pragma once

#include "election.hpp"
#include "results.hpp"

class BordaElection : public Election {
    std::vector<voting_sim::RoundResult> roundHistory;
public:
    std::vector<int> runElection() override;

    [[nodiscard]] const std::vector<voting_sim::RoundResult>& getRoundHistory() const override {
        return roundHistory;
    }
};

