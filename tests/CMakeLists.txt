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


add_subdirectory(greeter)
add_subdirectory(app)
