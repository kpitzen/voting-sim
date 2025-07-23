cmake_minimum_required(VERSION 3.16)
project(voting_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include/ so headers like "voting_sim/ballot.hpp" resolve
include_directories(${CMAKE_SOURCE_DIR}/include)

include(FetchContent)

# Download and make gtest available
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent GoogleTest from overriding your compiler/linker flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# JSON Serde
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0  # latest stable as of mid-2024
)

FetchContent_MakeAvailable(nlohmann_json)

# CLI11
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.5.0  # or latest stable
)

FetchContent_MakeAvailable(CLI11)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
