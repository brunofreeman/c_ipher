cmake_minimum_required(VERSION 3.16)
project(c_ipher)

include_directories(.)
include_directories(./inc/)
include_directories(./src/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TARGET1 c_ipher)
set(TARGET1_MAIN cipher_run)

set(TARGET2 c_ipher_tests)
set(TARGET2_MAIN cipher_test)

set(INC_DIR inc)
set(SRC_DIR src)

set(SRC_EXT .c)
set(INC_EXT .h)

list(APPEND FILES ciphers cipher_util)

set(INC ${FILES})
list(TRANSFORM INC PREPEND ${INC_DIR}/)
list(TRANSFORM INC APPEND  ${INC_EXT})

set(SRC ${FILES})
list(TRANSFORM SRC PREPEND ${SRC_DIR}/)
list(TRANSFORM SRC APPEND  ${SRC_EXT})

add_executable(${TARGET1} ${SRC_DIR}/${TARGET1_MAIN}${SRC_EXT} ${INC} ${SRC})
target_include_directories(${TARGET1} PRIVATE ${INC_DIR})

add_executable(${TARGET2} ${SRC_DIR}/${TARGET2_MAIN}${SRC_EXT} ${INC} ${SRC})
target_include_directories(${TARGET2} PRIVATE ${INC_DIR})