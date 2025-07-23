# Create the test executable
add_executable(voting_sim_tests
    test_generator.cpp
    # Add other test files here
)

# Link to your main library and gtest
target_link_libraries(voting_sim_tests
    PRIVATE voting_sim_lib
    PRIVATE gtest_main
)

# Let GoogleTest discover tests
include(GoogleTest)
gtest_discover_tests(voting_sim_tests)

# DO NOT do add_test() again manually
# It is handled by gtest_discover_tests
