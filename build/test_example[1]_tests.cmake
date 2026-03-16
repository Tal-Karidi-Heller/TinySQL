add_test([=[HelloTest.BasicAssertions]=]  /Users/tal/Desktop/TinySql/build/test_example [==[--gtest_filter=HelloTest.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HelloTest.BasicAssertions]=]  PROPERTIES DEF_SOURCE_LINE /Users/tal/Desktop/TinySql/tests/test_example.cpp:4 WORKING_DIRECTORY /Users/tal/Desktop/TinySql/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  test_example_TESTS HelloTest.BasicAssertions)
