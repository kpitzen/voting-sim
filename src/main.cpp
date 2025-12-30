#include "voting_sim/generator.hpp"
#include "voting_sim/election_factory.hpp"
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
    std::string methodStr = "irv";
    bool useGenerated = false;
    run->add_option("-f,--file", scenarioFile, "Path to scenario JSON");
    run->add_flag("-g,--generate", useGenerated, "Use a random generated scenario");
    run->add_option("-m,--method", methodStr, "Voting method (irv, borda, stv, schulze, coombs)")->default_val("irv");

    // === "generate" subcommand ===
    auto generate = app.add_subcommand("generate", "Generate and save a random scenario");

    std::string outputPath = "scenario.json";
    int numCandidates = 5, numVoters = 100, numWinners = 1;
    generate->add_option("-o,--output", outputPath, "Output scenario file");
    generate->add_option("--candidates", numCandidates, "Number of candidates");
    generate->add_option("--voters", numVoters, "Number of voters");
    generate->add_option("--winners", numWinners, "Number of winners");

    // === "compare" subcommand ===
    auto compare = app.add_subcommand("compare", "Compare multiple voting methods on the same scenario");
    std::string compareScenarioFile;
    bool compareUseGenerated = false;
    compare->add_option("-f,--file", compareScenarioFile, "Path to scenario JSON");
    compare->add_flag("-g,--generate", compareUseGenerated, "Use a random generated scenario");

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

    // === handle compare ===
    if (compare->parsed()) {
        voting_sim::Scenario scenario;
        if (compareUseGenerated) {
            scenario = voting_sim::generateScenario("Generated", 5, 100, 1);
        } else if (!compareScenarioFile.empty()) {
            std::ifstream in(compareScenarioFile);
            scenario = nlohmann::json::parse(in).get<voting_sim::Scenario>();
        } else {
            std::cerr << "Please provide --file or --generate for 'compare'\n";
            return 1;
        }

        std::vector<ElectionMethod> methods = {
            ElectionMethod::IRV,
            ElectionMethod::BORDA,
            ElectionMethod::STV,
            ElectionMethod::SCHULZE,
            ElectionMethod::COOMBS
        };

        std::cout << "Comparing voting methods on scenario: " << scenario.name << "\n";
        std::cout << "Candidates: " << scenario.candidates.size() 
                  << ", Voters: " << scenario.ballots.size() 
                  << ", Winners: " << scenario.numWinners << "\n\n";

        for (ElectionMethod method : methods) {
            auto election = ElectionFactory::create(method);
            election->setNumWinners(scenario.numWinners);
            for (const auto& c : scenario.candidates) election->addCandidate(c);
            for (const auto& b : scenario.ballots) election->addBallot(b);

            auto winners = election->runElection();
            std::cout << ElectionFactory::toString(method) << ": ";
            if (winners.empty()) {
                std::cout << "No winner\n";
            } else {
                for (size_t i = 0; i < winners.size(); i++) {
                    std::cout << winners[i];
                    if (i < winners.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
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

        ElectionMethod method = ElectionFactory::fromString(methodStr);
        auto election = ElectionFactory::create(method);
        election->setNumWinners(scenario.numWinners);
        for (const auto& c : scenario.candidates) election->addCandidate(c);
        for (const auto& b : scenario.ballots) election->addBallot(b);

        auto winners = election->runElection();
        
        std::cout << "Method: " << ElectionFactory::toString(method) << "\n";
        std::cout << "Winner(s): ";
        if (winners.empty()) {
            std::cout << "No winner\n";
        } else {
            for (size_t i = 0; i < winners.size(); i++) {
                std::cout << winners[i];
                if (i < winners.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }

        // Display round history (if available)
        const auto& rounds = election->getRoundHistory();
        if (!rounds.empty()) {
            std::cout << "\n";
            for (const auto& round : rounds) {
                std::cout << "Round " << round.roundNumber << ":\n";
                for (auto& [id, count] : round.voteCounts) {
                    if (count >= 0) {
                        std::cout << "  Candidate " << id << ": " << count << " votes\n";
                    } else {
                        std::cout << "  Candidate " << id << ": ELECTED\n";
                    }
                }
                if (!round.eliminated.empty()) {
                    std::cout << "  Eliminated: ";
                    for (int id : round.eliminated) std::cout << id << " ";
                    std::cout << "\n";
                }
            }
        }
    }

    return 0;
}
