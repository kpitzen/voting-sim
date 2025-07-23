cmake_minimum_required(VERSION 3.15)
project(voting_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include paths
include_directories(include)

include(FetchContent)

# Download GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(voting_sim_lib STATIC
    src/irv_election.cpp
    src/scenario.cpp
)

target_include_directories(voting_sim_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
add_executable(voting_sim src/main.cpp)
target_link_libraries(voting_sim PRIVATE voting_sim_lib)


# Optional: mark include/ as a system directory to suppress clang-tidy warnings
# target_include_directories(voting_sim SYSTEM PRIVATE include)

# For testing: define a separate target
enable_testing()
add_subdirectory(tests)
