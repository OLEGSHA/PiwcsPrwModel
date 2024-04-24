# Setup clang-tidy if available

option(SKIP_CLANG_TIDY "Disable clang-tidy checks at compile time")

if(SKIP_CLANG_TIDY)
    message(STATUS "Skipping clang-tidy: disabled via option SKIP_CLANG_TIDY")
    return()
endif()

if(NOT clang_tidy_EXECUTABLE)
    find_program(clang_tidy_EXECUTABLE NAMES clang-tidy)
endif()

if(clang_tidy_EXECUTABLE)
    message(STATUS "Found clang-tidy: ${clang_tidy_EXECUTABLE}")

    list(APPEND clang_tidy_command "${clang_tidy_EXECUTABLE}"
        "--warnings-as-errors=*"
        "--use-color")

    set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_command}")
else()
    message(STATUS "clang-tidy not found; continuing")
endif()
