#pragma once
#include <vector>
#include <nlohmann/json.hpp>

namespace voting_sim {
struct Ballot {
    std::vector<int> rankedCandidates;
};
inline void to_json(nlohmann::json& j, const Ballot& b) {
    j = {{"rankedCandidates", b.rankedCandidates}};
}
inline void from_json(const nlohmann::json& j, Ballot& b) {
    j.at("rankedCandidates").get_to(b.rankedCandidates);
}
}
