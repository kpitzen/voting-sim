# Voting Simulation Tool

A C++ tool for simulating and comparing various ranked-choice voting methods.

## Features

- **Multiple Voting Methods**: Supports 5 different ranked-choice voting methods:
  - **IRV** (Instant Runoff Voting) - Eliminates lowest vote-getters until a majority winner emerges
  - **Borda Count** - Assigns points based on ranking position
  - **STV** (Single Transferable Vote) - Multi-winner extension of IRV with vote transfers
  - **Schulze Method** - Condorcet method using strongest path algorithm
  - **Coombs' Method** - Eliminates candidates with most last-place votes

- **Scenario Generation**: Generate random voting scenarios with configurable parameters
- **Method Comparison**: Compare how different voting methods select winners from the same scenario
- **Round-by-Round Analysis**: View detailed round-by-round results for methods that use elimination

## Building

```bash
make
```

## Usage

### Generate a Scenario

```bash
./build/bin/voting_sim generate --candidates 5 --voters 100 --winners 1 -o scenario.json
```

### Run a Single Method

```bash
# Using a scenario file
./build/bin/voting_sim run --file scenario.json --method irv

# Using a generated scenario
./build/bin/voting_sim run --generate --method borda

# Available methods: irv, borda, stv, schulze, coombs
```

### Compare All Methods

```bash
# Compare all methods on a scenario file
./build/bin/voting_sim compare --file scenario.json

# Compare all methods on a generated scenario
./build/bin/voting_sim compare --generate
```

## Example Output

```
Comparing voting methods on scenario: Generated
Candidates: 5, Voters: 100, Winners: 1

IRV: 3
Borda: 3
STV: 3
Schulze: 3
Coombs: 3
```

## Next Steps

Potential enhancements:
- Additional voting methods (Ranked Pairs, Copeland, Bucklin, etc.)
- Visualization of results
- Statistical analysis of method properties
- Support for equal rankings and incomplete ballots
- Performance optimizations for large-scale simulations

