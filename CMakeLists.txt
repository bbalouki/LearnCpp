cmake_minimum_required(VERSION 3.25)

file(READ "VERSION.txt" RAW_VERSION)
string(STRIP "${RAW_VERSION}" APP_VERSION)

project(LEARNCPP VERSION ${APP_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clangd

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/_deps")
include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Versions.cmake)

if(${PROJECT_NAME}_ADD_COVERAGE_ANALYSIS)
    add_coverage_analysis(${CMAKE_SOURCE_DIR})
endif()

if(${PROJECT_NAME}_APPLY_FORMATING)
    apply_formating(${PROJECT_NAME}_USE_PER_FILE_FORMATTING ${CMAKE_SOURCE_DIR})
endif()

if(${PROJECT_NAME}_APPLY_CLANG_TIDY_GLOBALY)
    apply_clang_tidy_globaly()
endif()

if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
    build_documenation()
endif()

if(${PROJECT_NAME}_ENABLE_ADDRESS_SANITIZER)
    anable_address_sanitizer()
endif()

add_subdirectory(src/datamodel)
add_subdirectory(src/greeter)
add_subdirectory(src/date)
add_subdirectory(src/app)

if (${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (${PROJECT_NAME}_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if (${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(BUILD_SHARED_LIBS ON)
set(CPACK_WIX_VERSION 4)
set(CPACK_PACKAGE_VENDOR "StudyPlan.dev")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_WIX_UPGRADE_GUID "9F193C51-0E82-4E49-96DE-07E2DAC59100")

if (MSVC)
    set(CPACK_GENERATOR "WIX") 
    set(CPACK_COMPONENTS_ALL "Application")
else()
    set(CPACK_GENERATOR "ZIP") 
    set(CPACK_COMPONENTS_ALL "Development")
endif()

include(CPack)
cpack_add_component(Development
  DISPLAY_NAME "The Greeter Library"
  DESCRIPTION "Compiled Library, Header, and CMake Files"
)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL:NO")
    #set(VCPKG_TARGET_TRIPLET "x64-windows-static")
endif()

cpack_add_component(Application
  DISPLAY_NAME "The Greeter Application"
  DESCRIPTION "An application that says hello to the user"
)
