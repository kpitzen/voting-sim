#include "voting_sim/election_factory.hpp"
#include "voting_sim/irv_election.hpp"
#include "voting_sim/borda_election.hpp"
#include "voting_sim/stv_election.hpp"
#include "voting_sim/schulze_election.hpp"
#include "voting_sim/coombs_election.hpp"
#include <algorithm>
#include <cctype>

std::unique_ptr<Election> ElectionFactory::create(ElectionMethod method) {
    switch (method) {
        case ElectionMethod::IRV:
            return std::make_unique<IRVElection>();
        case ElectionMethod::BORDA:
            return std::make_unique<BordaElection>();
        case ElectionMethod::STV:
            return std::make_unique<STVElection>();
        case ElectionMethod::SCHULZE:
            return std::make_unique<SchulzeElection>();
        case ElectionMethod::COOMBS:
            return std::make_unique<CoombsElection>();
        default:
            return std::make_unique<IRVElection>(); // Default fallback
    }
}

ElectionMethod ElectionFactory::fromString(const std::string& methodName) {
    std::string lower;
    std::transform(methodName.begin(), methodName.end(), std::back_inserter(lower),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "irv" || lower == "instant-runoff" || lower == "rcv") {
        return ElectionMethod::IRV;
    } else if (lower == "borda" || lower == "borda-count") {
        return ElectionMethod::BORDA;
    } else if (lower == "stv" || lower == "single-transferable-vote") {
        return ElectionMethod::STV;
    } else if (lower == "schulze" || lower == "schulze-method") {
        return ElectionMethod::SCHULZE;
    } else if (lower == "coombs" || lower == "coombs-method") {
        return ElectionMethod::COOMBS;
    }
    
    return ElectionMethod::IRV; // Default
}

std::string ElectionFactory::toString(ElectionMethod method) {
    switch (method) {
        case ElectionMethod::IRV: return "IRV";
        case ElectionMethod::BORDA: return "Borda";
        case ElectionMethod::STV: return "STV";
        case ElectionMethod::SCHULZE: return "Schulze";
        case ElectionMethod::COOMBS: return "Coombs";
        default: return "Unknown";
    }
}

