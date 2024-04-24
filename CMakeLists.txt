cmake_minimum_required(VERSION 3.20)

include(cmake/ClangTidy.cmake)
include(cmake/Doxygen.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(piwcsprwmodel VERSION 1.0.1 LANGUAGES CXX)

add_library(piwcsprwmodel)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
