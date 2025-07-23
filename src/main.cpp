#include "voting_sim/irv_election.hpp"
#include "voting_sim/scenarios/irv_examples.hpp"
#include <iostream>

int main() {
    auto scenario = voting_sim::scenarios::simple_majority_irv();
    IRVElection election;
    election.setNumWinners(scenario.numWinners);

    for (auto& c : scenario.candidates)
    {
        election.addCandidate(c);
    }
    for (auto& b : scenario.ballots)
    {
        election.addBallot(b);
    }

    auto winners = election.runElection();
    if (winners == scenario.expectedWinners) {
        std::cout << scenario.name << ": PASS\n";
    } else {
        std::cout << scenario.name << ": FAIL (expected ";
        for (int e : scenario.expectedWinners) std::cout << e << " ";
        std::cout << ", got ";
        for (int w : winners) std::cout << w << " ";
        std::cout << ")\n";
    }
}
