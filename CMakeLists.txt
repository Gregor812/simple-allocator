cmake_minimum_required(VERSION 3.5)

project(simpleAllocator
    LANGUAGES ASM C CXX
)

set(${CMAKE_BUILD_DIR} ${CMAKE_SOURCE_DIR}/build)

#add_library(
    #Allocator
    #SHARED
    #${CMAKE_SOURCE_DIR}/allocator.cpp
#)

#set_property(TARGET Allocator PROPERTY CXX_STANDARD 14)

add_executable(
	App
    ${CMAKE_SOURCE_DIR}/allocator.cpp
	${CMAKE_SOURCE_DIR}/main.cpp
)

set_property(TARGET App PROPERTY CXX_STANDARD 14)

#target_link_libraries(
	#App
	#Allocator
#)
