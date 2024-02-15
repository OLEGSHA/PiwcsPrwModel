add_executable(piwcsprwmodel-print
    EXCLUDE_FROM_ALL
    main.cpp
)

target_link_libraries(piwcsprwmodel-print PRIVATE piwcsprwmodel)
