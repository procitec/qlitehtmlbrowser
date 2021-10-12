add_library( QLiteHtmlBrowser SHARED "")

qt5_add_resources( qrcs
    QLiteHtmlBrowser.qrc
)

qt5_wrap_cpp ( mocs
    container_qt.h
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
    QLiteHtmlBrowserImpl.h
    )

target_sources(QLiteHtmlBrowser
    PUBLIC
       ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h

    PRIVATE
        QLiteHtmlBrowser.cpp
        QLiteHtmlBrowserImpl.cpp
        QLiteHtmlBrowserImpl.h
        container_qt.cpp
        container_qt.h
        ${mocs}
        ${qrcs}
)

target_include_directories( QLiteHtmlBrowser PRIVATE ${PROJECT_SOURCE_DIR}/include )
target_link_libraries(QLiteHtmlBrowser PUBLIC PRIVATE litehtml Qt5::Widgets)
set_target_properties(QLiteHtmlBrowser PROPERTIES VERSION ${QLiteHtmlBrowser_VERSION})

if( DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_XML YES )
    set( DOXYGEN_OUTPUT_DIRECTORY "${QLiteHtmlBrowser_SOURCE_DIR}/doc/_build" )
    doxygen_add_docs( doxygen_QLiteHtmlBrowser
        ALL
        ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
        USE_STAMP_FILE

    )
endif()

install( TARGETS QLiteHtmlBrowser DESTINATION lib COMPONENT QLiteHtmlBrowser )

