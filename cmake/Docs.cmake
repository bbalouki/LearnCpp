cmake_minimum_required(VERSION 3.23)

find_package(Doxygen)
if (Doxygen_FOUND)
    message(
        STATUS 
        "Found Doxygen Version " "${DOXYGEN_VERSION} " 
        "at ${DOXYGEN_EXECUTABLE}"
    )
    configure_file(
        "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        @ONLY
    )
    doxygen_add_docs(
        docs 
        CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        COMMENT "Generating docs with Doxygen ..."
    )
    message(STATUS "Add 'docs' target")
endif()


