project( QLiteHtmlBrowser )

cmake_minimum_required( VERSION 3.2 )

set( AUTOMOC OFF)
set( AUTOUIC OFF)
set( AUTORCC OFF)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")

    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
    endif()
endif()

set( CMAKE_CXX_STANDARD 17 )

find_package( Qt5 COMPONENTS Widgets Gui Test Core Svg Help REQUIRED)
find_package(Doxygen REQUIRED)

include( CTest )
enable_testing()

add_subdirectory( foreign )
#add_subdirectory( include )
add_subdirectory( src )

add_subdirectory( test )
