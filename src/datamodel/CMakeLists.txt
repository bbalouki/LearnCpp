cmake_minimum_required(VERSION 3.21)


# find_package(protobuf CONFIG REQUIRED)
# set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proto/person.proto")
# set(GENERATED_H "${CMAKE_CURRENT_BINARY_DIR}/person.pb.h")
# set(GENERATED_CC "${CMAKE_CURRENT_BINARY_DIR}/person.pb.cc")

# add_custom_command(
#   OUTPUT ${GENERATED_H} ${GENERATED_CC}
#   DEPENDS ${PROTO_FILE}
#   COMMENT "Using protobuf to generate C++ files from person.proto"
#   COMMAND
#     ${Protobuf_PROTOC_EXECUTABLE}
#     --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
#     --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
#     ${PROTO_FILE}
# )
# add_library(DataModel ${GENERATED_CC})
# target_include_directories(DataModel PUBLIC
#   ${CMAKE_CURRENT_BINARY_DIR}
# )
# target_link_libraries(DataModel PUBLIC
#   protobuf::libprotobuf
# )

find_package(protobuf CONFIG REQUIRED)
add_library(DataModel)

# This replaces our add_custom_command() call
protobuf_generate(
  TARGET DataModel
  LANGUAGE cpp
  PROTOS proto/person.proto
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

target_include_directories(DataModel PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DataModel PUBLIC
  protobuf::libprotobuf
)
