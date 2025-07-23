

# === Core Library ===
add_library(voting_sim_lib
    ballot.cpp
    candidate.cpp
    election.cpp
    irv_election.cpp
    generator.cpp
    scenario.cpp
    # any other logic-only .cpp files
)

target_include_directories(voting_sim_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# === CLI Binary ===
add_executable(voting_sim main.cpp)

target_link_libraries(voting_sim
    PRIVATE voting_sim_lib
)

target_link_libraries(voting_sim PRIVATE CLI11::CLI11)
target_link_libraries(voting_sim PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(voting_sim_lib PUBLIC nlohmann_json::nlohmann_json)

# Optional: put CLI binary in bin/
set_target_properties(voting_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
