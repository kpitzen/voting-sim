#include "voting_sim/irv_election.hpp"
#include "voting_sim/generator.hpp"
#include <iostream>

using namespace voting_sim;

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

    voting_sim::ClusteredPreferences prefs{3, {}, 0.2};

    voting_sim::Scenario s = generateScenario(
        "Three-Cluster Sim",
        /*numCandidates=*/6,
        /*numVoters=*/1000,
        /*numWinners=*/1,
        prefs,
        /*minRank=*/3,
        /*maxRank=*/5,
        /*seed=*/123
    );

    std::cout << "Scenario: " << s.name << "\n";
    std::cout << "Candidates:\n";
    for (const auto& c : s.candidates) {
        std::cout << "  " << c.id << ": " << c.name << "\n";
    }
    std::cout << "Ballots generated: " << s.ballots.size() << "\n";
}
