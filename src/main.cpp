#include "voting_sim/generator.hpp"
#include "voting_sim/irv_election.hpp"
#include "CLI/CLI.hpp"

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

int main(int argc, char** argv) {
    CLI::App app{"Ranked-choice voting simulator"};

    // === "run" subcommand ===
    auto run = app.add_subcommand("run", "Run a scenario and print results");

    std::string scenarioFile;
    bool useGenerated = false;
    run->add_option("-f,--file", scenarioFile, "Path to scenario JSON");
    run->add_flag("-g,--generate", useGenerated, "Use a random generated scenario");

    // === "generate" subcommand ===
    auto generate = app.add_subcommand("generate", "Generate and save a random scenario");

    std::string outputPath = "scenario.json";
    int numCandidates = 5, numVoters = 100, numWinners = 1;
    generate->add_option("-o,--output", outputPath, "Output scenario file");
    generate->add_option("--candidates", numCandidates, "Number of candidates");
    generate->add_option("--voters", numVoters, "Number of voters");
    generate->add_option("--winners", numWinners, "Number of winners");

    app.require_subcommand();
    CLI11_PARSE(app, argc, argv);

    // === handle generate ===
    if (generate->parsed()) {
        auto scenario = voting_sim::generateScenario("Generated Scenario", numCandidates, numVoters, numWinners);
        std::ofstream out(outputPath);
        nlohmann::json j = scenario;
        out << j.dump(2);
        std::cout << "Scenario saved to " << outputPath << "\n";
        return 0;
    }

    // === handle run ===
    if (run->parsed()) {
        voting_sim::Scenario scenario;
        if (useGenerated) {
            scenario = voting_sim::generateScenario("Generated", 5, 100, 1);
        } else if (!scenarioFile.empty()) {
            std::ifstream in(scenarioFile);
            scenario = nlohmann::json::parse(in).get<voting_sim::Scenario>();
        } else {
            std::cerr << "Please provide --file or --generate for 'run'\n";
            return 1;
        }

        IRVElection election;
        election.setNumWinners(scenario.numWinners);
        for (const auto& c : scenario.candidates) election.addCandidate(c);
        for (const auto& b : scenario.ballots) election.addBallot(b);

        auto winners = election.runElection();
        const auto& rounds = election.getRoundHistory();

        std::cout << "Winner(s): ";
        for (int w : winners) std::cout << w << " ";
        std::cout << "\n";

        for (const auto& round : rounds) {
            std::cout << "Round " << round.roundNumber << ":\n";
            for (auto& [id, count] : round.voteCounts) {
                std::cout << "  Candidate " << id << ": " << count << " votes\n";
            }
            if (!round.eliminated.empty()) {
                std::cout << "  Eliminated: ";
                for (int id : round.eliminated) std::cout << id << " ";
                std::cout << "\n";
            }
        }
    }

    return 0;
}
