cmake_minimum_required(VERSION 3.16)
project(WSesh-hb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(deps/harfbuzz EXCLUDE_FROM_ALL)

add_executable(hb
  src/hb/main.cpp
)
target_link_libraries(hb PRIVATE harfbuzz)
