#include <gtest/gtest.h>
#include "voting_sim/election_factory.hpp"
#include "voting_sim/scenarios/canonical_examples.hpp"
#include "voting_sim/scenarios/irv_examples.hpp"
#include <algorithm>
#include <set>

using namespace voting_sim;
using namespace voting_sim::scenarios;

// Helper function to check if winner sets match (order-independent)
bool winnerSetsMatch(const std::vector<int>& actual, const std::vector<int>& expected) {
    std::set<int> actualSet(actual.begin(), actual.end());
    std::set<int> expectedSet(expected.begin(), expected.end());
    return actualSet == expectedSet;
}

// Helper function to run election and check results
void runElectionAndCheck(const Scenario& scenario, ElectionMethod method, const std::vector<int>& expectedWinners) {
    auto election = ElectionFactory::create(method);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    
    EXPECT_TRUE(winnerSetsMatch(winners, expectedWinners))
        << "Method: " << ElectionFactory::toString(method)
        << ", Scenario: " << scenario.name
        << ", Expected: [" << [&expectedWinners]() {
            std::string s;
            for (size_t i = 0; i < expectedWinners.size(); i++) {
                if (i > 0) s += ", ";
                s += std::to_string(expectedWinners[i]);
            }
            return s;
        }() << "]"
        << ", Got: [" << [&winners]() {
            std::string s;
            for (size_t i = 0; i < winners.size(); i++) {
                if (i > 0) s += ", ";
                s += std::to_string(winners[i]);
            }
            return s;
        }() << "]";
}

// ============================================================================
// IRV Tests
// ============================================================================

TEST(IRVElectionTest, SimpleMajority) {
    auto scenario = irv_simple_majority();
    runElectionAndCheck(scenario, ElectionMethod::IRV, {0});
}

TEST(IRVElectionTest, NeedsElimination) {
    auto scenario = irv_elimination_needed();
    runElectionAndCheck(scenario, ElectionMethod::IRV, scenario.expectedWinners);
}

TEST(IRVElectionTest, SplitVote) {
    auto scenario = irv_split_vote();
    // This scenario results in a tie, so no winner
    auto election = ElectionFactory::create(ElectionMethod::IRV);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    EXPECT_TRUE(winners.empty()) << "Split vote scenario should result in no winner (tie)";
}

TEST(IRVElectionTest, ExistingSimpleMajority) {
    auto scenario = simple_majority_irv();
    runElectionAndCheck(scenario, ElectionMethod::IRV, {0});
}

TEST(IRVElectionTest, ExistingEliminationCase) {
    auto scenario = needs_elimination_irv();
    runElectionAndCheck(scenario, ElectionMethod::IRV, scenario.expectedWinners);
}

TEST(IRVElectionTest, RoundHistoryRecorded) {
    auto scenario = irv_elimination_needed();
    auto election = ElectionFactory::create(ElectionMethod::IRV);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    const auto& rounds = election->getRoundHistory();
    
    EXPECT_FALSE(rounds.empty()) << "IRV should record round history";
    EXPECT_EQ(winners.size(), 1);
    EXPECT_EQ(winners[0], 2); // Carol wins
}

// ============================================================================
// Borda Count Tests
// ============================================================================

TEST(BordaElectionTest, ClassicExample) {
    auto scenario = borda_classic();
    runElectionAndCheck(scenario, ElectionMethod::BORDA, {1});
}

TEST(BordaElectionTest, ClearWinner) {
    auto scenario = borda_clear_winner();
    runElectionAndCheck(scenario, ElectionMethod::BORDA, {0});
}

TEST(BordaElectionTest, MultiWinner) {
    auto scenario = borda_multi_winner();
    auto election = ElectionFactory::create(ElectionMethod::BORDA);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    
    // Should return 2 winners
    EXPECT_EQ(winners.size(), 2);
    // Should include candidate 0 (highest score)
    EXPECT_TRUE(std::find(winners.begin(), winners.end(), 0) != winners.end());
}

TEST(BordaElectionTest, RoundHistoryRecorded) {
    auto scenario = borda_classic();
    auto election = ElectionFactory::create(ElectionMethod::BORDA);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    const auto& rounds = election->getRoundHistory();
    
    EXPECT_FALSE(rounds.empty()) << "Borda should record round history";
    EXPECT_EQ(winners.size(), 1);
}

// ============================================================================
// STV Tests
// ============================================================================

TEST(STVElectionTest, TwoWinners) {
    auto scenario = stv_two_winners();
    runElectionAndCheck(scenario, ElectionMethod::STV, {0, 1});
}

TEST(STVElectionTest, SurplusTransfer) {
    auto scenario = stv_surplus_transfer();
    auto election = ElectionFactory::create(ElectionMethod::STV);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    
    // Should return 2 winners
    EXPECT_EQ(winners.size(), 2);
    // Should include candidate 0 (meets quota)
    EXPECT_TRUE(std::find(winners.begin(), winners.end(), 0) != winners.end());
}

TEST(STVElectionTest, RoundHistoryRecorded) {
    auto scenario = stv_two_winners();
    auto election = ElectionFactory::create(ElectionMethod::STV);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    const auto& rounds = election->getRoundHistory();
    
    EXPECT_FALSE(rounds.empty()) << "STV should record round history";
    EXPECT_EQ(winners.size(), 2);
}

// ============================================================================
// Schulze Method Tests
// ============================================================================

TEST(SchulzeElectionTest, CondorcetWinner) {
    auto scenario = schulze_condorcet_winner();
    runElectionAndCheck(scenario, ElectionMethod::SCHULZE, {0});
}

TEST(SchulzeElectionTest, CondorcetParadox) {
    auto scenario = schulze_paradox();
    auto election = ElectionFactory::create(ElectionMethod::SCHULZE);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    
    // Should return a winner (Schulze resolves cycles)
    EXPECT_EQ(winners.size(), 1);
    // In this specific cycle, A should win (has strongest path)
    EXPECT_EQ(winners[0], 0);
}

TEST(SchulzeElectionTest, RoundHistoryRecorded) {
    auto scenario = schulze_condorcet_winner();
    auto election = ElectionFactory::create(ElectionMethod::SCHULZE);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    const auto& rounds = election->getRoundHistory();
    
    EXPECT_FALSE(rounds.empty()) << "Schulze should record round history";
    EXPECT_EQ(winners.size(), 1);
}

// ============================================================================
// Coombs' Method Tests
// ============================================================================

TEST(CoombsElectionTest, Classic) {
    auto scenario = coombs_classic();
    runElectionAndCheck(scenario, ElectionMethod::COOMBS, scenario.expectedWinners);
}

TEST(CoombsElectionTest, LastPlaceElimination) {
    auto scenario = coombs_last_place_elimination();
    runElectionAndCheck(scenario, ElectionMethod::COOMBS, scenario.expectedWinners);
}

TEST(CoombsElectionTest, RoundHistoryRecorded) {
    auto scenario = coombs_classic();
    auto election = ElectionFactory::create(ElectionMethod::COOMBS);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    const auto& rounds = election->getRoundHistory();
    
    EXPECT_FALSE(rounds.empty()) << "Coombs should record round history";
    EXPECT_EQ(winners.size(), 1);
}

// ============================================================================
// Method Comparison Tests
// ============================================================================

TEST(MethodComparisonTest, IRVvsBorda) {
    // Use a scenario where IRV and Borda might differ
    auto scenario = borda_classic();
    
    auto irvElection = ElectionFactory::create(ElectionMethod::IRV);
    auto bordaElection = ElectionFactory::create(ElectionMethod::BORDA);
    
    for (auto* election : {irvElection.get(), bordaElection.get()}) {
        election->setNumWinners(scenario.numWinners);
        for (const auto& candidate : scenario.candidates) {
            election->addCandidate(candidate);
        }
        for (const auto& ballot : scenario.ballots) {
            election->addBallot(ballot);
        }
    }
    
    auto irvWinners = irvElection->runElection();
    auto bordaWinners = bordaElection->runElection();
    
    // Both should produce a winner
    EXPECT_EQ(irvWinners.size(), 1);
    EXPECT_EQ(bordaWinners.size(), 1);
    
    // In this specific scenario, they may differ
    // This test just ensures both methods complete successfully
}

TEST(MethodComparisonTest, AllMethodsComplete) {
    // Test that all methods can handle the same scenario
    auto scenario = irv_simple_majority();
    
    std::vector<ElectionMethod> methods = {
        ElectionMethod::IRV,
        ElectionMethod::BORDA,
        ElectionMethod::STV,
        ElectionMethod::SCHULZE,
        ElectionMethod::COOMBS
    };
    
    for (ElectionMethod method : methods) {
        auto election = ElectionFactory::create(method);
        election->setNumWinners(scenario.numWinners);
        
        for (const auto& candidate : scenario.candidates) {
            election->addCandidate(candidate);
        }
        
        for (const auto& ballot : scenario.ballots) {
            election->addBallot(ballot);
        }
        
        auto winners = election->runElection();
        
        // All methods should produce at least one winner for this scenario
        EXPECT_GE(winners.size(), 1) 
            << "Method " << ElectionFactory::toString(method) << " should produce a winner";
    }
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST(EdgeCaseTest, SingleCandidate) {
    Scenario scenario = {
        "Single Candidate",
        {{0, "Only"}},
        {{{0}}, {{0}}, {{0}}},
        {0},
        1
    };
    
    for (ElectionMethod method : {ElectionMethod::IRV, ElectionMethod::BORDA, 
                                   ElectionMethod::STV, ElectionMethod::SCHULZE, 
                                   ElectionMethod::COOMBS}) {
        auto election = ElectionFactory::create(method);
        election->setNumWinners(scenario.numWinners);
        
        for (const auto& candidate : scenario.candidates) {
            election->addCandidate(candidate);
        }
        
        for (const auto& ballot : scenario.ballots) {
            election->addBallot(ballot);
        }
        
        auto winners = election->runElection();
        EXPECT_EQ(winners.size(), 1);
        EXPECT_EQ(winners[0], 0);
    }
}

TEST(EdgeCaseTest, EmptyBallots) {
    Scenario scenario = {
        "Empty Ballots",
        {{0, "A"}, {1, "B"}},
        {},
        {},
        1
    };
    
    auto election = ElectionFactory::create(ElectionMethod::IRV);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    auto winners = election->runElection();
    EXPECT_TRUE(winners.empty());
}

TEST(EdgeCaseTest, AllCandidatesWin) {
    Scenario scenario = {
        "All Win",
        {{0, "A"}, {1, "B"}},
        {{{0, 1}}, {{1, 0}}},
        {0, 1},
        2
    };
    
    auto election = ElectionFactory::create(ElectionMethod::BORDA);
    election->setNumWinners(scenario.numWinners);
    
    for (const auto& candidate : scenario.candidates) {
        election->addCandidate(candidate);
    }
    
    for (const auto& ballot : scenario.ballots) {
        election->addBallot(ballot);
    }
    
    auto winners = election->runElection();
    EXPECT_EQ(winners.size(), 2);
}

