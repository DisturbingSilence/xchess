cmake_minimum_required(VERSION 3.28)
project(xchess LANGUAGES C)

set(SOURCES src/main.c)

add_executable(xchess ${SOURCES})
target_compile_features(xchess PRIVATE c_std_17)
