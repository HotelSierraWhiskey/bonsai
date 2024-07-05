cmake_minimum_required(VERSION 3.20)
project(Bonsai)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH BONSAI_PATH)
message(STATUS "Include Path: ${BONSAI_PATH}")
set(BONSAI_SOURCES src/api.cpp src/bonsai.cpp src/edit.cpp src/internal.cpp src/nvmctrl.cpp src/primatives.cpp)

# TODO: Add tests support as well

# TODO: Is bonsai a static library?
add_library(bonsai STATIC ${BONSAI_SOURCES})
target_include_directories(bonsai PUBLIC ${BONSAI_PATH})
target_compile_options(bonsai PUBLIC "-fpermissive;-w")
