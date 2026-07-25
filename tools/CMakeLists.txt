cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add executable from test_send.cpp
add_executable(test_send test_send.cpp)
