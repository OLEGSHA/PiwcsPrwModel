find_package(GTest)

if(GTest_FOUND)
    enable_testing()

    add_executable(tests
        model.cpp
        nodes.cpp
        section.cpp
        metadata.cpp
        io_read.cpp
        io_wr.cpp
    )

    target_link_libraries(tests piwcsprwmodel)

    target_link_libraries(tests GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
