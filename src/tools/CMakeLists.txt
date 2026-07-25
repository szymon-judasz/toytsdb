cmake_minimum_required(VERSION 3.10)
project(test_utils)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

find_library(LIBURING_LIBRARY uring REQUIRED)


# Add executable from test_sender.cpp
add_executable(test_sender test_sender.cpp)
