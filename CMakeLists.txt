cmake_minimum_required(VERSION 3.28)
project(xchess LANGUAGES C)

set(SOURCES
  src/main.c
  src/graphics.c
  src/logger.c)

add_subdirectory(dep)
add_executable(xchess ${SOURCES})
target_compile_features(xchess PRIVATE c_std_23)
target_link_libraries(xchess PRIVATE glfw glad)

target_compile_options(xchess PRIVATE -g -ggdb)
