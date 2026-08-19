cmake_minimum_required(VERSION 3.16)
project(lpx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Dev>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Dev>:-fsanitize=address,undefined>
)