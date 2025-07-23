#pragma once
#include <string>

struct Candidate {
    int id;
    std::string name;

    Candidate(int id_, std::string name_) : id(id_), name(std::move(name_)) {}
};
