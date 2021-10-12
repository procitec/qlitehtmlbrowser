
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")

    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
    endif()
    cmake_policy( SET CMP0048 NEW )

    cmake_minimum_required( VERSION 3.2 )

    set( CMAKE_CXX_STANDARD 17 )

    set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS
    set( CMAKE_INSTALL_PREFIX "" )

    add_custom_target(install-qlitehtmlbrowser
                    COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_COMPONENT=QLiteHtmlBrowser -P ${CMAKE_BINARY_DIR}/cmake_install.cmake

    )

    option( WITH_ASAN "Build with sanitizer" OFF)

    if( NOT MSVC AND WITH_ASAN)
        set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")
    endif()

    if( UNIX )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    endif()

endif()


project( QLiteHtmlBrowser VERSION 1.0.0)

set( AUTOMOC OFF)
set( AUTOUIC OFF)
set( AUTORCC OFF)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    find_package( Qt5 COMPONENTS Widgets Gui Core Test REQUIRED)
    find_package(Doxygen REQUIRED)

    include( CTest )
    enable_testing()
endif()

add_subdirectory( foreign )
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( dist )
add_subdirectory( test )

