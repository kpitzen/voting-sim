#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace voting_sim {

struct Candidate {
    int id;
    std::string name;
};

inline void to_json(nlohmann::json& j, const Candidate& c) {
    j = {{"id", c.id}, {"name", c.name}};
}

inline void from_json(const nlohmann::json& j, Candidate& c) {
    j.at("id").get_to(c.id);
    j.at("name").get_to(c.name);
}
}
