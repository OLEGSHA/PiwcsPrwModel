add_executable(examples
    EXCLUDE_FROM_ALL
    main.cpp
)

target_link_libraries(examples PRIVATE piwcsprwmodel)
