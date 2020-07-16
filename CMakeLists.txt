cmake_minimum_required(VERSION 3.16)
project(c_ipher)

include_directories(.)
include_directories(./inc/)
include_directories(./src/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TARGET c_ipher)

set(INC_DIR inc)
set(SRC_DIR src)

set(SRC_EXT .c)
set(INC_EXT .h)

list(APPEND FILES)

set(INC ${FILES})
list(TRANSFORM INC PREPEND ${INC_DIR}/)
list(TRANSFORM INC APPEND  ${INC_EXT})

set(SRC ${FILES})
list(TRANSFORM SRC PREPEND ${SRC_DIR}/)
list(TRANSFORM SRC APPEND  ${SRC_EXT})

add_executable(${TARGET} ${SRC_DIR}/${TARGET}${SRC_EXT} ${INC} ${SRC})
target_include_directories(${TARGET} PRIVATE ${INC_DIR})