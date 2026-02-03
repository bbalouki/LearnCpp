find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/_deps")
    include(FetchContent)
    FetchContent_Declare(
        GTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG "v${GTEST_VERSION}"
    )
    FetchContent_MakeAvailable(GTest)
endif()

add_executable(
    GreeterTests 
    app/test_app.cpp
    greeter/test_greeter.cpp
)

# Link against the logic library, NOT the executable
target_link_libraries(GreeterTests PRIVATE
    GreeterAppLogic
    GreeterLib
    GTest::gtest 
    GTest::gmock
    GTest::gtest_main
    GTest::gmock_main
)
target_include_directories(GreeterTests PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/app
)

include(GoogleTest)
gtest_discover_tests(
  GreeterTests
  PROPERTIES LABELS "application"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DISCOVERY_MODE PRE_TEST
)
