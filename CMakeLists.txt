cmake_minimum_required(VERSION 3.28)

project( QLiteHtmlBrowser VERSION 2.3.1 )

include(GNUInstallDirs)

if(PROJECT_IS_TOP_LEVEL)
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
        message(FATAL_ERROR "In-source builds are not allowed.")
    endif()

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

    find_package(Qt6 COMPONENTS Core Gui Widgets)
    if(Qt6_FOUND)
      set(QT_VERSION_MAJOR 6)
    else()
      set(QT_VERSION_MAJOR 5)
      find_package(Qt5 5.15 COMPONENTS Core Gui Widgets REQUIRED)
    endif()

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


if(PROJECT_IS_TOP_LEVEL AND EXISTS "${CMAKE_SOURCE_DIR}/CPackSettings.cmake")
    include (CPackSettings.cmake)
    add_custom_target(dist-source COMMAND cpack --config ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake  )
endif()
