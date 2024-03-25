target_sources(piwcsprwmodel PRIVATE
    model.cpp
    nodes.cpp
    section.cpp
    metadata.cpp
    util.cpp
    algorithms.cpp
    printing.cpp
    io_read.cpp
    io_write.cpp
)

target_include_directories(piwcsprwmodel PUBLIC ../include)
target_include_directories(piwcsprwmodel PRIVATE ../lib)
