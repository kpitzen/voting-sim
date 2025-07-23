#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "voting_sim/candidate.hpp"
#include "voting_sim/ballot.hpp"

namespace voting_sim {

struct Scenario {
    std::string name;
    std::vector<Candidate> candidates;
    std::vector<Ballot> ballots;
    std::vector<int> expectedWinners;  // candidate IDs
    int numWinners = 1;
};

inline void to_json(nlohmann::json& j, const Scenario& s) {
    j = {
        {"name", s.name},
        {"numWinners", s.numWinners},
        {"candidates", s.candidates},
        {"ballots", s.ballots},
        {"expectedWinners", s.expectedWinners}
    };
}

inline void from_json(const nlohmann::json& j, Scenario& s) {
    j.at("name").get_to(s.name);
    j.at("numWinners").get_to(s.numWinners);
    j.at("candidates").get_to(s.candidates);
    j.at("ballots").get_to(s.ballots);
    j.at("expectedWinners").get_to(s.expectedWinners);
}
}
