# Setup Doxygen if available

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
