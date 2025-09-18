cmake_minimum_required(VERSION 3.23)

find_program(CLANG_FORMAT_EXE clang-format)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/*.h" "${CMAKE_SOURCE_DIR}/*.hpp")

if (CLANG_FORMAT_EXE)
    add_custom_target(
        format
        COMMENT "Formatting files with clang-format ..."
        COMMAND ${CLANG_FORMAT_EXE} -i ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    message(STATUS "Add 'format' target")
else()
    message(WARNING "clang-format not found - format target unavailable")
endif()

# If we have a lot of files
# if (CLANG_FORMAT_EXE)
#     message(STATUS "Add 'format' target")
#     add_custom_target(
#         format
#         COMMENT "Formatting files with clang-format ..."
#     )
#     foreach(file ${SOURCES} ${HEADERS})
#         add_custom_command(
#             TARGET format PRE_BUILD
#             COMMAND ${CLANG_FORMAT_EXE} -i ${file}
#         )
#     endforeach()
# else()
#     message(WARNING "clang-format not found - format target unavailable")
# endif()
