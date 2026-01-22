cmake_minimum_required(VERSION 3.20)

find_package(spdlog CONFIG REQUIRED)
find_package(tabulate CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

# 1. The library containing the testable logic
add_library(GreeterAppLogic run.cpp)

target_sources(GreeterAppLogic PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "${PROJECT_SOURCE_DIR}/src/app"
  FILES "${PROJECT_SOURCE_DIR}/src/app/run.h"
)

target_link_libraries(
    GreeterAppLogic 
    PRIVATE 
    Date
    DataModel
    GreeterLib
    spdlog::spdlog
    tabulate::tabulate
    argparse::argparse
)


# 2. The thin executable wrapper
add_executable(GreeterApp main.cpp)
target_link_libraries(GreeterApp PRIVATE GreeterAppLogic)


include(GNUInstallDirs)
install(
  TARGETS GreeterApp
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Application
)

install(
  TARGETS GreeterApp
  RUNTIME_DEPENDENCIES
  PRE_INCLUDE_REGEXES ".*spdlog.*" ".*tabulate.*" ".*argparse.*"
  PRE_EXCLUDE_REGEXES ".*"
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Application
)
