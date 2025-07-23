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

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
