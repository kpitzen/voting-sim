add_library(voting_sim_lib
    generator.cpp
    irv_election.cpp
    # add other .cpp files
)

target_include_directories(voting_sim_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(voting_sim main.cpp)
target_link_libraries(voting_sim PRIVATE voting_sim_lib)
