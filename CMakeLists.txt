cmake_minimum_required(VERSION 3.0)

project(piwcsprwmodel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    set(DOXYGEN_EXTENSION_MAPPING no_extension=C++)
    doxygen_add_docs(doxygen include/PiwcsPrwModel)
endif()

add_subdirectory(src)
add_subdirectory(printer)
