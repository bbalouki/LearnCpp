cmake_minimum_required(VERSION 3.20)

# No source file
add_library(Date INTERFACE)

# The iclude directory is an INTERFACE property 
# Date itself doesn't need to compile anything
# However, anyone who uses it needs this path.
target_include_directories(
    Date INTERFACE 
    ${CMAKE_SOURCE_DIR}/include/date
)

# We are using std::format, which was added in c++20
# Anyone linking against us will need to support that
target_compile_features(Date INTERFACE cxx_std_20)
