cmake_minimum_required(VERSION 3.0)

project(test)

set(CMAKE_C_STANDARD 11)

set(SRC_DIR  ${PROJECT_SOURCE_DIR})
set(SRC_FILES   ${SRC_DIR}/test/main.c
                ${SRC_DIR}/tracker/trk.c
                ${SRC_DIR}/ram/ram.c
                ${SRC_DIR}/list/list.c)

add_executable(test ${SRC_FILES})
