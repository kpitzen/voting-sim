#pragma once

#include "election.hpp"
#include <memory>
#include <string>

enum class ElectionMethod {
    IRV,        // Instant Runoff Voting
    BORDA,      // Borda Count
    STV,        // Single Transferable Vote
    SCHULZE,    // Schulze method (Condorcet)
    COOMBS      // Coombs' method
};

class ElectionFactory {
public:
    static std::unique_ptr<Election> create(ElectionMethod method);
    static ElectionMethod fromString(const std::string& methodName);
    static std::string toString(ElectionMethod method);
};

