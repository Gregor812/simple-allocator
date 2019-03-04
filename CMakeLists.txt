cmake_minimum_required(VERSION 3.5)

project(simpleAllocator
    LANGUAGES ASM C CXX
)

add_library(
    Allocator
    SHARED
    ${CMAKE_SOURCE_DIR}/allocator.cpp
)

#set_property(TARGET App PROPERTY CXX_STANDARD 14)
