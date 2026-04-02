add_executable(unit_tests test_parser.cpp test_tokenizer.cpp test_engine.cpp test_entire_project.cpp)

target_compile_options(unit_tests PUBLIC -fsanitize=address)
target_link_libraries(unit_tests PUBLIC -fsanitize=address)

# Link to GTest AND your app_logic library
target_link_libraries(
        unit_tests
        PRIVATE
        GTest::gtest_main
        app_logic
)

# This makes the test show up in 'ctest'
add_test(NAME MyLogicTests COMMAND unit_tests)