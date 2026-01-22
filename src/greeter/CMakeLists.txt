# cmake_minimum_required(VERSION 3.20)

# set(LIB_SOURCES 
#     Greeter.cpp
#     ChronoDateProvider.cpp
# )
# if (WIN32)
#     message(STATUS "Using platform_win.cpp")
#     list(APPEND LIB_SOURCES platform_win.cpp)
# else()
#     message(STATUS, "Using platform_nix.cpp")
#     list(APPEND LIB_SOURCES platform_nix.cpp)
# endif()

# add_library(GreeterLib ${LIB_SOURCES})
# set_cxx_std(GreeterLib 20) # CXX standard
# target_link_libraries(GreeterLib warnings::strict)
# if (CMAKE_SIZEOF_VOID_P EQUAL 8)
#     target_compile_definitions(GreeterLib PRIVATE ARCH_64BIT)
#     message(STATUS "Targeting 64-bit architecture")
# else()
#     target_compile_definitions(GreeterLib Private ARCH_32BIT)
#     message(STATUS "Targeting 32-bit arhictecture")
# endif()

# add_library(Greeter::Lib ALIAS GreeterLib)

# target_include_directories(GreeterLib PUBLIC 
#     ${PROJECT_SOURCE_DIR}/include
# )
# target_include_directories(GreeterLib PRIVATE 
#     ${PROJECT_SOURCE_DIR}/src/greeter
# )
# install(TARGETS GreeterLib 
#     DESTINATION "${CMAKE_SOURCE_DIR}/bin"
# )
##################################################################################""
# Step 1: Declaring a project()
# project(GreeterLib VERSION 1.0.0)

include(GNUInstallDirs)

add_library(GreeterLib Greeter.cpp ChronoDateProvider.cpp)
add_library(GreeterLib::Lib ALIAS GreeterLib)
set_target_properties(
    GreeterLib PROPERTIES
    OUTPUT_NAME Greeter 
    EXPORT_NAME Lib
)

# Step 2: Updating Include Directories
target_include_directories(
    GreeterLib PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/greeter>

)
target_include_directories(
    GreeterLib PRIVATE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/greeter>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/greeter>

)

# Step 3: Specifying Output Destinations
install(
    TARGETS GreeterLib
    EXPORT GreeterLibTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Development
        COMPONENT Application
)

# Step 4: Specifying Header Destinations
file(
    GLOB GREETER_ROOT_HEADERS 
    "${PROJECT_SOURCE_DIR}/include/greeter/*.h"
)
install(
    FILES ${GREETER_ROOT_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/greeter
        COMPONENT Development
)

# Step 5: Exporting the Target Information
install(
    EXPORT GreeterLibTargets
    FILE GreeterLibTargets.cmake 
    NAMESPACE Greeter::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GreeterLib
        COMPONENT Development
)

# Step 6: Creating the Main Package Configuration File
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/GreeterLibConfig.cmake.in"
    "GreeterLibConfig.cmake"
    INSTALL_DESTINATION 
    "${CMAKE_INSTALL_LIBDIR}/cmake/GreeterLib"
)
# Step 7: Creating a Package Version File
write_basic_package_version_file(
    "GreeterLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Step 8: Installing the Generated Package Files
install(
    FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/GreeterLibConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GreeterLibConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GreeterLib"
    COMPONENT Development
)
