cmake_minimum_required(VERSION 3.14)
project(UniquePtr)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(unique_ptr_app src/main.cpp)

enable_testing() # activates cmake's ctest
add_subdirectory(tests)