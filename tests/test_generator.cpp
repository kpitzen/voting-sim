#include <gtest/gtest.h>
#include "voting_sim/generator.hpp"

using namespace voting_sim;

TEST(ClusteredGeneratorTest, ProducesCorrectNumberOfBallots) {
    const int numVoters = 1000;
    const int numCandidates = 5;
    const int seed = 42;

    ClusteredPreferences prefs{
        .numClusters = 3,
        .anchors = {},     // Let generator create random clusters
        .noise = 0.2
    };

    auto ballots = generateClusteredBallots(numVoters, numCandidates, prefs, seed);

    ASSERT_EQ(ballots.size(), numVoters);

    for (const auto& ballot : ballots) {
        EXPECT_EQ(ballot.rankedCandidates.size(), numCandidates);
        std::vector<bool> seen(numCandidates, false);
        for (int c : ballot.rankedCandidates) {
            EXPECT_GE(c, 0);
            EXPECT_LT(c, numCandidates);
            EXPECT_FALSE(seen[c]) << "Duplicate candidate in ballot";
            seen[c] = true;
        }
    }
}

TEST(ClusteredGeneratorTest, ProducesBiasInFirstChoice) {
    const int numVoters = 1000;
    const int numCandidates = 5;
    const int seed = 123;

    ClusteredPreferences prefs{
        .numClusters = 3,
        .anchors = {},
        .noise = 0.0  // no noise = pure cluster behavior
    };

    auto ballots = generateClusteredBallots(numVoters, numCandidates, prefs, seed);

    std::map<int, int> firstChoiceCounts;
    for (const auto& b : ballots) {
        ASSERT_FALSE(b.rankedCandidates.empty());
        firstChoiceCounts[b.rankedCandidates[0]]++;
    }

    // Expect at least 2 candidates to have high first-choice counts
    ASSERT_GE(firstChoiceCounts.size(), 2);

    // Check that no single candidate dominates all first choices
    int maxVotes = 0;
    for (auto& [_, count] : firstChoiceCounts) {
        maxVotes = std::max(maxVotes, count);
    }

    // Max shouldn't have more than 80% of the vote in a 3-cluster model
    EXPECT_LT(maxVotes, numVoters * 0.8);
}

TEST(PartialBallotTest, HonorsMinAndMaxRankedRange) {
    const int numVoters = 200;
    const int numCandidates = 6;
    const int minRank = 2;
    const int maxRank = 4;
    const int seed = 321;

    auto ballots = generateRandomBallots(numVoters, numCandidates, seed, minRank, maxRank);

    for (const auto& b : ballots) {
        EXPECT_GE(b.rankedCandidates.size(), minRank);
        EXPECT_LE(b.rankedCandidates.size(), maxRank);
    }
}
