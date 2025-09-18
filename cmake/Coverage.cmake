cmake_minimum_required(VERSION 3.23)

# Add coverage flags for GCC/Clang in the "Coverage" build type
set(COVERAGE_CONDITION
  "$<AND:$<CONFIG:Coverage>,$<CXX_COMPILER_ID:GNU,Clang>>"
)
set(COVERAGE_FLAGS "$<${COVERAGE_CONDITION}:--coverage>")
add_compile_options(${COVERAGE_FLAGS})
add_link_options(${COVERAGE_FLAGS})

find_program(GCOVR_EXE gcovr)
if(GCOVR_EXE)
  add_custom_target(coverage
    COMMAND ${GCOVR_EXE}
      -r "${CMAKE_SOURCE_DIR}"
      --filter "${CMAKE_SOURCE_DIR}/src/greeter/"
      --filter "${CMAKE_SOURCE_DIR}/src/app/"
      --html "coverage.html"
    COMMENT "Generating code coverage report..."
  )
  message(STATUS "Added 'coverage' target")
endif()
