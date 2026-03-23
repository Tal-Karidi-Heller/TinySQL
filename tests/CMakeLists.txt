add_executable(unit_tests test_example.cpp test_parser.cpp)

# Link to GTest AND your app_logic library
target_link_libraries(
        unit_tests
        PRIVATE
        GTest::gtest_main
        app_logic
)

# This makes the test show up in 'ctest'
add_test(NAME MyLogicTests COMMAND unit_tests)