cmake_minimum_required(VERSION 3.20)

project(piwcsprwmodel VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(piwcsprwmodel)

# Setup clang-tidy if available
find_program(clang_tidy_EXECUTABLE NAMES clang-tidy-13 clang-tidy)
if(clang_tidy_EXECUTABLE)
    list(APPEND clang_tidy_command "${clang_tidy_EXECUTABLE}"
        "--warnings-as-errors=*"
        "--use-color")

    set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_command}")
endif()

# Setup doxygen if available
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
    set(DOXYGEN_INCLUDE_GRAPH NO)
    set(DOXYGEN_INCLUDED_BY_GRAPH NO)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_IMAGE_PATH docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    doxygen_add_docs(doxygen include README.md)
endif()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
