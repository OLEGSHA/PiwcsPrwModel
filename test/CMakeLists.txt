find_package(GTest)

if(GTest_FOUND)
    enable_testing()

    add_executable(tests
        data_model.cpp
    )

    target_link_libraries(tests piwcsprwmodel)

    target_link_libraries(tests GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
