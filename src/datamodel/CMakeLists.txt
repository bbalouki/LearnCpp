cmake_minimum_required(VERSION 3.27)

find_package(protobuf CONFIG REQUIRED)
add_library(DataModel)

protobuf_generate(
  TARGET DataModel
  LANGUAGE cpp
  PROTOS proto/person.proto
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
  OUT_VAR PROTO_GENERATED_FILES 
)

set_source_files_properties(
  ${PROTO_GENERATED_FILES} 
  PROPERTIES SKIP_LINTING ON
)

target_include_directories(DataModel PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DataModel PUBLIC
  protobuf::libprotobuf
)
