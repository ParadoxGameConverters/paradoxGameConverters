cmake_minimum_required(VERSION 3.2)

# Set compiler flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wno-invalid-source-encoding")

project (CK2ToEU3)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install" CACHE PATH "CMake Install Prefix" FORCE)
add_subdirectory(Source)
add_subdirectory(Test)