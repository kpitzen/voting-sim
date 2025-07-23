
# This assumes test_main.cpp is a standalone test binary.
add_executable(voting_sim_tests test_main.cpp)
target_link_libraries(voting_sim_tests PRIVATE voting_sim_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(voting_sim_tests)
# Link with main logic if separated into a library (optional)
# target_link_libraries(voting_sim_tests PRIVATE voting_core)

add_test(NAME AllTests COMMAND voting_sim_tests)
