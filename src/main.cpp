#include "voting_sim/irv_election.hpp"
#include "voting_sim/generator.hpp"
#include <iostream>

int main() {
    IRVElection election;

    const int numCandidates = 5;
    const int numVoters = 100;
    const int seed = 42;

    election.setNumWinners(1);

    for (int i = 0; i < numCandidates; ++i)
    {
        election.addCandidate({i, "Candidate " + std::to_string(i)});
    }

    auto ballots = voting_sim::generateRandomBallots(numVoters, numCandidates, seed);
    for (const auto& b : ballots)
    {
        election.addBallot(b);
    }

    auto winners = election.runElection();
    std::cout << "Winner(s): ";
    for (int id_ : winners)
    {
        std::cout << id_ << " ";
    }
    std::cout << "\n";
}
