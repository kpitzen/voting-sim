#include <gtest/gtest.h>
#include "voting_sim/generator.hpp"
#include "voting_sim/election_factory.hpp"
#include "voting_sim/scenario.hpp"
#include <vector>
#include <set>

using namespace voting_sim;

// Test fixture for ballot generation configurations
class BallotElectionCompatibilityTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Define all election methods to test
        methods = {
            ElectionMethod::IRV,
            ElectionMethod::BORDA,
            ElectionMethod::STV,
            ElectionMethod::SCHULZE,
            ElectionMethod::COOMBS
        };
    }

    std::vector<ElectionMethod> methods;
    
    // Helper to validate ballot is processable by election method
    bool canProcessBallots(const std::vector<Ballot>& ballots, 
                          const std::vector<Candidate>& candidates,
                          ElectionMethod method, 
                          int numWinners) {
        try {
            auto election = ElectionFactory::create(method);
            election->setNumWinners(numWinners);
            
            for (const auto& candidate : candidates) {
                election->addCandidate(candidate);
            }
            
            for (const auto& ballot : ballots) {
                election->addBallot(ballot);
            }
            
            auto winners = election->runElection();
            
            // Basic validation: winners should be valid candidate IDs or empty
            std::set<int> candidateIds;
            for (const auto& candidate : candidates) {
                candidateIds.insert(candidate.id);
            }
            
            for (int winnerId : winners) {
                if (candidateIds.count(winnerId) == 0) {
                    return false; // Invalid winner ID
                }
            }
            
            // Winners count should not exceed numWinners (unless all candidates win)
            if (!winners.empty() && 
                static_cast<int>(winners.size()) > numWinners && 
                static_cast<int>(candidates.size()) > numWinners) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false; // Exception thrown
        }
    }
};

// ============================================================================
// Test: Random Ballots (Full Rankings) vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, RandomFullRankings_AllMethods) {
    const int numCandidates = 5;
    const int numVoters = 20;
    const int numWinners = 1;
    const unsigned seed = 42;
    
    // Generate candidates
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    // Generate random ballots with full rankings (default)
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed);
    
    // Verify all ballots are full rankings
    for (const auto& ballot : ballots) {
        EXPECT_EQ(ballot.rankedCandidates.size(), numCandidates);
    }
    
    // Test each election method
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process random full-ranking ballots";
    }
}

// ============================================================================
// Test: Random Ballots (Partial Rankings) vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, RandomPartialRankings_AllMethods) {
    const int numCandidates = 7;
    const int numVoters = 30;
    const int numWinners = 1;
    const unsigned seed = 123;
    const int minRank = 2;
    const int maxRank = 5;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed, minRank, maxRank);
    
    // Verify ballots are within range
    for (const auto& ballot : ballots) {
        EXPECT_GE(ballot.rankedCandidates.size(), minRank);
        EXPECT_LE(ballot.rankedCandidates.size(), maxRank);
    }
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process random partial-ranking ballots";
    }
}

// ============================================================================
// Test: Clustered Ballots (Full Rankings) vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, ClusteredFullRankings_AllMethods) {
    const int numCandidates = 6;
    const int numVoters = 25;
    const int numWinners = 1;
    const unsigned seed = 456;
    
    ClusteredPreferences prefs{
        .numClusters = 3,
        .anchors = {},
        .noise = 0.1
    };
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateClusteredBallots(numVoters, numCandidates, prefs, seed);
    
    // Verify all ballots are full rankings
    for (const auto& ballot : ballots) {
        EXPECT_EQ(ballot.rankedCandidates.size(), numCandidates);
    }
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process clustered full-ranking ballots";
    }
}

// ============================================================================
// Test: Clustered Ballots (Partial Rankings) vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, ClusteredPartialRankings_AllMethods) {
    const int numCandidates = 8;
    const int numVoters = 35;
    const int numWinners = 1;
    const unsigned seed = 789;
    const int minRank = 3;
    const int maxRank = 6;
    
    ClusteredPreferences prefs{
        .numClusters = 2,
        .anchors = {},
        .noise = 0.2
    };
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateClusteredBallots(numVoters, numCandidates, prefs, seed, minRank, maxRank);
    
    // Verify ballots are within range
    for (const auto& ballot : ballots) {
        EXPECT_GE(ballot.rankedCandidates.size(), minRank);
        EXPECT_LE(ballot.rankedCandidates.size(), maxRank);
    }
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process clustered partial-ranking ballots";
    }
}

// ============================================================================
// Test: Single-Rank Ballots (Plurality-like) vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, SingleRankBallots_AllMethods) {
    const int numCandidates = 5;
    const int numVoters = 15;
    const int numWinners = 1;
    const unsigned seed = 321;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    // Generate ballots with only first choice
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed, 1, 1);
    
    // Verify all ballots have exactly 1 ranking
    for (const auto& ballot : ballots) {
        EXPECT_EQ(ballot.rankedCandidates.size(), 1);
    }
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process single-rank ballots";
    }
}

// ============================================================================
// Test: Multi-Winner Scenarios vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, MultiWinnerScenarios_AllMethods) {
    const int numCandidates = 6;
    const int numVoters = 30;
    const int numWinners = 2;
    const unsigned seed = 555;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed);
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process multi-winner scenario";
    }
}

// ============================================================================
// Test: Edge Cases - Minimal Candidates
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, MinimalCandidates_AllMethods) {
    const int numCandidates = 2;  // Minimum for meaningful election
    const int numVoters = 10;
    const int numWinners = 1;
    const unsigned seed = 111;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed);
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed with minimal candidates";
    }
}

// ============================================================================
// Test: Edge Cases - Many Candidates
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, ManyCandidates_AllMethods) {
    const int numCandidates = 15;
    const int numVoters = 50;
    const int numWinners = 1;
    const unsigned seed = 222;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateRandomBallots(numVoters, numCandidates, seed, 5, 10);
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed with many candidates";
    }
}

// ============================================================================
// Test: Clustered with Custom Anchors vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, ClusteredCustomAnchors_AllMethods) {
    const int numCandidates = 5;
    const int numVoters = 20;
    const int numWinners = 1;
    const unsigned seed = 333;
    
    // Create custom anchor preferences
    std::vector<std::vector<int>> anchors = {
        {0, 1, 2, 3, 4},  // Cluster 1: prefers 0 > 1 > 2 > 3 > 4
        {4, 3, 2, 1, 0}   // Cluster 2: prefers 4 > 3 > 2 > 1 > 0
    };
    
    ClusteredPreferences prefs{
        .numClusters = 2,
        .anchors = anchors,
        .noise = 0.15
    };
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    auto ballots = generateClusteredBallots(numVoters, numCandidates, prefs, seed);
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(ballots, candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process clustered ballots with custom anchors";
    }
}

// ============================================================================
// Test: Scenario Generation vs All Election Methods
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, GeneratedScenarios_AllMethods) {
    const int numCandidates = 5;
    const int numVoters = 25;
    const int numWinners = 1;
    const unsigned seed = 444;
    
    ClusteredPreferences prefs{2, {}, 0.1};
    Scenario scenario = generateScenario(
        "Compatibility Test",
        numCandidates,
        numVoters,
        numWinners,
        prefs,
        2,   // minRank
        5,   // maxRank
        seed
    );
    
    for (ElectionMethod method : methods) {
        EXPECT_TRUE(canProcessBallots(scenario.ballots, scenario.candidates, method, numWinners))
            << "Method " << ElectionFactory::toString(method) 
            << " failed to process generated scenario";
    }
}

// ============================================================================
// Test: Comprehensive Cross-Product Test
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, ComprehensiveCrossProduct) {
    // Test multiple configurations
    struct TestConfig {
        int numCandidates;
        int numVoters;
        int numWinners;
        std::optional<int> minRank;
        std::optional<int> maxRank;
        bool useClustered;
        ClusteredPreferences prefs;
        std::string description;
    };
    
    std::vector<TestConfig> configs = {
        {5, 20, 1, {}, {}, false, {}, "Random full rankings"},
        {6, 25, 1, 2, 4, false, {}, "Random partial rankings"},
        {5, 20, 1, {}, {}, true, ClusteredPreferences{2, {}, 0.1}, "Clustered full"},
        {7, 30, 1, 3, 5, true, ClusteredPreferences{3, {}, 0.2}, "Clustered partial"},
        {4, 15, 2, {}, {}, false, {}, "Multi-winner random"},
        {5, 20, 2, {}, {}, true, ClusteredPreferences{2, {}, 0.1}, "Multi-winner clustered"},
    };
    
    for (const auto& config : configs) {
        std::vector<Candidate> candidates;
        for (int i = 0; i < config.numCandidates; ++i) {
            candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
        }
        
        std::vector<Ballot> ballots;
        if (config.useClustered) {
            ballots = generateClusteredBallots(
                config.numVoters,
                config.numCandidates,
                config.prefs,
                42,
                config.minRank,
                config.maxRank
            );
        } else {
            ballots = generateRandomBallots(
                config.numVoters,
                config.numCandidates,
                42,
                config.minRank,
                config.maxRank
            );
        }
        
        for (ElectionMethod method : methods) {
            EXPECT_TRUE(canProcessBallots(ballots, candidates, method, config.numWinners))
                << "Config: " << config.description
                << ", Method: " << ElectionFactory::toString(method)
                << " - Failed compatibility test";
        }
    }
}

// ============================================================================
// Test: Verify Results Are Deterministic (Same Seed = Same Results)
// ============================================================================

TEST_F(BallotElectionCompatibilityTest, DeterministicResults) {
    const int numCandidates = 5;
    const int numVoters = 20;
    const int numWinners = 1;
    const unsigned seed = 999;
    
    std::vector<Candidate> candidates;
    for (int i = 0; i < numCandidates; ++i) {
        candidates.emplace_back(Candidate{i, "Candidate " + std::to_string(i)});
    }
    
    // Generate ballots twice with same seed
    auto ballots1 = generateRandomBallots(numVoters, numCandidates, seed);
    auto ballots2 = generateRandomBallots(numVoters, numCandidates, seed);
    
    // Ballots should be identical
    ASSERT_EQ(ballots1.size(), ballots2.size());
    for (size_t i = 0; i < ballots1.size(); ++i) {
        EXPECT_EQ(ballots1[i].rankedCandidates, ballots2[i].rankedCandidates);
    }
    
    // Results should be identical for each method
    for (ElectionMethod method : methods) {
        auto election1 = ElectionFactory::create(method);
        auto election2 = ElectionFactory::create(method);
        
        for (auto* election : {election1.get(), election2.get()}) {
            election->setNumWinners(numWinners);
            for (const auto& candidate : candidates) {
                election->addCandidate(candidate);
            }
        }
        
        for (const auto& ballot : ballots1) {
            election1->addBallot(ballot);
        }
        for (const auto& ballot : ballots2) {
            election2->addBallot(ballot);
        }
        
        auto winners1 = election1->runElection();
        auto winners2 = election2->runElection();
        
        EXPECT_EQ(winners1, winners2)
            << "Method " << ElectionFactory::toString(method) 
            << " produced different results with same ballots";
    }
}

