cmake_minimum_required(VERSION 3.23)

set(SANITIZER_SUPPORTED OFF)

if (MINGW)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SANITIZER_SUPPORTED OFF)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(SANITIZER_SUPPORTED ON)
  endif()
elseif (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set(SANITIZER_SUPPORTED ON)
endif()

if (SANITIZER_SUPPORTED)
  if (MSVC)
    add_compile_options(
      "$<$<CONFIG:Sanitize>:/fsanitize=address>"
      "$<$<CONFIG:Sanitize>:/Zi>"
    )
    add_link_options(
      "$<$<CONFIG:Sanitize>:/fsanitize=address>"
    )
  else()
    add_compile_options(
      "$<$<CONFIG:Sanitize>:-fsanitize=address>"
      "$<$<CONFIG:Sanitize>:-fno-omit-frame-pointer>"
      "$<$<CONFIG:Sanitize>:-g>"
    )
    add_link_options(
      "$<$<CONFIG:Sanitize>:-fsanitize=address>"
    )
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Sanitize")
    message(STATUS "Enabling AddressSanitizer")
  endif()
else()
  if (CMAKE_BUILD_TYPE STREQUAL "Sanitize")
    message(WARNING "Sanitizers are NOT supported with ${CMAKE_CXX_COMPILER_ID} on MinGW. Skipping.")
  endif()
endif()
