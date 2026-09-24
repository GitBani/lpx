cmake_minimum_required(VERSION 3.16)
project(lpx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/4267679b6887f349f17b01ccd70c9e3483689b25.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.cpp")

add_library(${PROJECT_NAME}_lib INTERFACE)
target_include_directories(${PROJECT_NAME}_lib INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(${PROJECT_NAME}_lib INTERFACE
  $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion
                    -O0 -fsanitize=address,undefined -fno-omit-frame-pointer>
)
target_link_options(${PROJECT_NAME}_lib INTERFACE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(${PROJECT_NAME}_tests ${TESTS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_lib GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
