cmake_minimum_required(VERSION 3.14)
project(json_ext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_EXT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

# FIXME not sure whether this really is the minimum version I tested only with 1.81
find_package(Boost 1.65.0)
include_directories(${Boost_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP true
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB_RECURSE TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
add_executable(tests "${TEST_SRCS}")
target_include_directories(tests PUBLIC "${JSON_EXT_INCLUDE_DIRS}")
target_link_libraries(tests PUBLIC gtest_main gmock_main)

include(GoogleTest)
gtest_discover_tests(tests)
