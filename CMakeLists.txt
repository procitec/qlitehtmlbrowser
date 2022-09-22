if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    cmake_minimum_required( VERSION 3.18 )

    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
    endif()
    cmake_policy( SET CMP0048 NEW )

    set( CMAKE_CXX_STANDARD 17 )

    set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS
    set( CMAKE_INSTALL_PREFIX "" )

    option( WITH_ASAN "Build with sanitizer" OFF)
    option( WTIH_DOCS "Build documentation of the library" OFF)

    if( NOT MSVC AND WITH_ASAN)
        set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")
    endif()

    set( CMAKE_POSITION_INDEPENDENT_CODE TRUE )
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)

    set( AUTOMOC OFF)
    set( AUTOUIC OFF)
    set( AUTORCC OFF)
endif()


project( QLiteHtmlBrowser VERSION 1.1.0)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    find_package(Qt5 COMPONENTS Widgets Gui Core Test REQUIRED)
    if( WITH_DOCS )
        find_package(Doxygen REQUIRED)
    endif()

    include( CTest )
    enable_testing()
endif()

add_subdirectory( foreign )
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( dist )

if (BUILD_TESTING)
    add_subdirectory( test )
endif()


if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    if( EXISTS "${CMAKE_SOURCE_DIR}/CPackSettings.cmake")
        include (CPackSettings.cmake)
        add_custom_target(dist-source COMMAND cpack --config ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake  )
    endif()
endif()

