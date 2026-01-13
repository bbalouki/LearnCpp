cmake_minimum_required(VERSION 3.20)

add_executable(
    LearnCpp
    example.cpp
)

apply_clang_tidy(LearnCpp)
