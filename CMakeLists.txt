project( QLiteHtmlBrowser )

cmake_minimum_required( VERSION 3.2 )

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")

    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
    endif()
    
    find_package( Qt5 COMPONENTS Widgets Gui PrintSupport Test Network Xml Core Svg REQUIRED)
endif()

add_subdirectory(foreign)
