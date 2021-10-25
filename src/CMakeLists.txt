add_library( QLiteHtmlBrowser SHARED "")

add_definitions(-DQT_BUILD_WIDGETS_LIB)

qt5_add_resources( qrcs
    QLiteHtmlBrowser.qrc
)

qt5_wrap_cpp ( mocs
    container_qt.h
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
    QLiteHtmlBrowserImpl.h
    )

set( PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
)

target_sources(QLiteHtmlBrowser
    PRIVATE
        QLiteHtmlBrowser.cpp
        QLiteHtmlBrowserImpl.cpp
        QLiteHtmlBrowserImpl.h
        container_qt.cpp
        container_qt.h
        browserdefinitions.h
        ${mocs}
        ${qrcs}
)

target_include_directories( QLiteHtmlBrowser
    PUBLIC #${PROJECT_SOURCE_DIR}/include
    # where top-level project will look for the library's public headers
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    # where external projects will look for the library's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(QLiteHtmlBrowser PRIVATE litehtml Qt::Widgets Qt::Gui Qt::Core)
set_target_properties(QLiteHtmlBrowser PROPERTIES VERSION ${QLiteHtmlBrowser_VERSION})
set_target_properties(QLiteHtmlBrowser PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_property(TARGET QLiteHtmlBrowser PROPERTY CXX_STANDARD 17)

if( WITH_DOCS AND DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_XML YES )
    set( DOXYGEN_OUTPUT_DIRECTORY "${QLiteHtmlBrowser_SOURCE_DIR}/doc/_build" )
    doxygen_add_docs( doxygen_QLiteHtmlBrowser
        ALL
        ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
        USE_STAMP_FILE

    )
endif()

install( TARGETS QLiteHtmlBrowser EXPORT QLiteHtmlBrowserTargets DESTINATION lib COMPONENT QLiteHtmlBrowser
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qlitehtmlbrowser
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
    )


