cmake_minimum_required(VERSION 3.23)

add_executable(GreeterAppTests test_app.cpp)

# Link against the logic library, NOT the executable
target_link_libraries(GreeterAppTests PRIVATE
  GreeterAppLogic
  GTest::gtest
  GTest::gmock
  GTest::gmock_main
)
target_include_directories(GreeterAppTests PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/app
)

include(GoogleTest)
gtest_discover_tests(
  GreeterAppTests
  DISCOVERY_MODE PRE_TEST
  PROPERTIES LABELS "application"
)
