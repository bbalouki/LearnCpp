cmake_minimum_required(VERSION 3.20)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)

project(
    LearnCpp
    VERSION "1.0"
)
set(LEARNCPP_CXX_STANDARD 20 CACHE STRING "C++ standard for the project")
set(CMAKE_CXX_STANDARD ${LEARNCPP_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(
    LearnCpp
    main.cpp
)

# if(LEARNCPP_PROJECT_ENV STREQUAL "DEV")
#     include(cmake/Tidy.cmake)
#     include(cmake/Format.cmake)
#     include(cmake/Coverage.cmake)
# endif()

# include(cmake/Sanitize.cmake)
# add_subdirectory(src/datamodel)
# add_subdirectory(src/greeter)
# add_subdirectory(src/date)
# add_subdirectory(src/app)

# if (LEARNCPP_BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

# if (LEARNCPP_BUILD_BENCHMARKS)
#     add_subdirectory(benchmarks)
# endif()

# if (LEARNCPP_BUILD_EXAMPLES)
#     add_subdirectory(examples)
# endif()


