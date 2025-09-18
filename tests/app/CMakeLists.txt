cmake_minimum_required(VERSION 3.23)

find_package(GTest CONFIG REQUIRED)
add_executable(GreeterAppTests test_app.cpp)

# Link against the logic library, NOT the executable
target_link_libraries(GreeterAppTests PRIVATE
  GreeterAppLogic
  GTest::gtest
  GTest::gmock
  GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(
  GreeterAppTests
  PROPERTIES LABELS "application"
)
