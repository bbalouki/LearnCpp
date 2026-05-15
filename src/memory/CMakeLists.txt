set(MEMORY_SOURCES
    characters.cpp
    floats.cpp
    images.cpp
    integers.cpp
    sounds.cpp
    video_sound.cpp
    videos.cpp
)

foreach(SOURCE ${MEMORY_SOURCES})
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE})
    target_compile_features(${EXE_NAME} PRIVATE cxx_std_23)
endforeach()
