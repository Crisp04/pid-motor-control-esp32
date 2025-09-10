cmake_minimum_required(VERSION 3.14)
project(pid_host_tests)

set(CMAKE_CXX_STANDARD 17)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(pid_lib STATIC ../src/pid.cpp)
target_include_directories(pid_lib PUBLIC ../src)

add_executable(pid_test pid_test.cpp)
target_link_libraries(pid_test PRIVATE pid_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(pid_test)
