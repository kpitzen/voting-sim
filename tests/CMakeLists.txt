enable_testing()

add_executable(voting_sim_tests
    test_generator.cpp
    # add other tests here
)

target_link_libraries(voting_sim_tests
    PRIVATE voting_sim_lib
    PRIVATE gtest_main
)

include(GoogleTest)
gtest_discover_tests(voting_sim_tests)
