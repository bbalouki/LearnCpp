cmake_minimum_required(VERSION 3.23)

find_program(CLANG_TIDY_EXE "clang-tidy")

if(CLANG_TIDY_EXE)
    message(STATUS "Found Clang-tidy : ${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
    )
else()
    message(WARNING "Clang-tidy Not Found!")
endif()
