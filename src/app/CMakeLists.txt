cmake_minimum_required(VERSION 3.20)
include(${CMAKE_SOURCE_DIR}/cmake/TargetHelpers.cmake)

option(
    SAY_GOODBYE
    "Should the program say goodbye before exiting"
    OFF
)
set(GREETER_SOURCES
    main.cpp
)
if (SAY_GOODBYE)
    message(STATUS "Adding goodbye.cpp to GREETER_SOURCES")
    list(APPEND GREETER_SOURCES goodbye.cpp)
    list(LENGTH GREETER_SOURCES GREETER_SOURCES_COUNT)
    message(STATUS "GREETER_SOURCES now has ${GREETER_SOURCES} files")
endif()


# include(FetchContent)
# # Step 1: Try to find an existing spdlog installation.
# # The QUIET keyword prevents it from outputting a warning
# find_package(spdlog 1.1 QUIET) 

# # Step 2: If it wasn't found, fetch it from source.
# if(spdlog_FOUND) 
#   message(STATUS "Found spdlog version ${spdlog_VERSION}") 
# else() 
#   message(STATUS "spdlog not found. Fetching from source") 
#   FetchContent_Declare(spdlog 
#     GIT_REPOSITORY https://github.com/gabime/spdlog.git 
#     GIT_TAG v1.15.3 
#   ) 
#   FetchContent_MakeAvailable(spdlog) 
# endif() 

# add_executable(GreeterApp ${GREETER_SOURCES})
# setup_target(GreeterApp)
# target_include_directories(GreeterApp PUBLIC 
#     ${PROJECT_SOURCE_DIR}/include
# )
find_package(spdlog CONFIG REQUIRED)
# # find_package(tabulate CONFIG REQUIRED)
# # find_package(Boost CONFIG REQUIRED COMPONENTS program_options)
# target_link_libraries(GreeterApp PRIVATE DataModel)

# target_link_libraries(
#     GreeterApp 
#     PRIVATE 
#     Date
#     Greeter::Lib
#     spdlog::spdlog
#     # tabulate::tabulate
#     # Boost::program_options
# )

# if (SAY_GOODBYE)
#     message(STATUS "Defining SAY_GOOBYE")
#     target_compile_definitions(GreeterApp PUBLIC SAY_GOODBYE)
# endif()

# install(TARGETS GreeterApp DESTINATION "${CMAKE_SOURCE_DIR}/bin")
# set_target_properties(GreeterApp PROPERTIES
#     INSTALL_RPATH "$ORIGIN"
# )

# 1. The library containing the testable logic
add_library(GreeterAppLogic run.cpp)

target_link_libraries(
    GreeterAppLogic PRIVATE 
    GreeterLib
    spdlog::spdlog
)

target_sources(GreeterAppLogic PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "${PROJECT_SOURCE_DIR}/src/app"
  FILES "${PROJECT_SOURCE_DIR}/src/app/run.h"
)

# 2. The thin executable wrapper
add_executable(GreeterApp main.cpp)
target_link_libraries(GreeterApp PRIVATE GreeterAppLogic)
