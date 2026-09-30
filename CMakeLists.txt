cmake_minimum_required(VERSION 3.20)
project(HW1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SmartPointers INTERFACE)

target_sources(SmartPointers
  INTERFACE
    FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      UnqPtr.hpp
      ShrdPtr.hpp
)

if(NOT MSVC)
    target_compile_options(SmartPointers INTERFACE -Wall -Wextra -Wpedantic)
endif()

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(testUnqPtr tests/testUnqPtr.cpp)
    target_link_libraries(testUnqPtr PRIVATE SmartPointers GTest::gtest_main)

    add_executable(testShrdPtr tests/testShrdPtr.cpp)
    target_link_libraries(testShrdPtr PRIVATE SmartPointers GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(testUnqPtr)
    gtest_discover_tests(testShrdPtr)
endif()

add_executable(bench bench.cpp)
target_link_libraries(bench PRIVATE SmartPointers)

if(NOT MSVC)
    target_compile_options(bench PRIVATE -O2)
endif()