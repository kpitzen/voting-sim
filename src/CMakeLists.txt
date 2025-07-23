add_library(voting_sim_lib STATIC
    irv_election.cpp
)

target_include_directories(voting_sim_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
add_executable(voting_sim main.cpp)
target_link_libraries(voting_sim PRIVATE voting_sim_lib)
