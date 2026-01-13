cmake_minimum_required(VERSION 3.23)

add_executable(GreeterLibTests test_greeter.cpp)

target_link_libraries(
    GreeterLibTests PRIVATE 
    GreeterLib
    GTest::gtest 
    GTest::gmock
    GTest::gtest_main
    GTest::gmock_main
)
include(GoogleTest)
gtest_discover_tests(
    GreeterLibTests
    # Label all discovered tests as library tests
    PROPERTIES LABELS "library"
)
