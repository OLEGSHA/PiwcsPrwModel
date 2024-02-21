add_library(piwcsprwmodel
    model.cpp
    nodes.cpp
    section.cpp
    util.cpp
    printing.cpp
    io.cpp
)

target_include_directories(piwcsprwmodel PUBLIC ../include)
