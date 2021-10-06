add_library( QLiteHtmlBrowser STATIC "")

qt5_add_resources( qrcs
    QLiteHtmlBrowser.qrc
)

qt5_wrap_cpp ( mocs
    container_qt.h
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
    )

target_sources(QLiteHtmlBrowser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h

    PRIVATE
        QLiteHtmlBrowser.cpp
        container_qt.cpp
        container_qt.h
        ${mocs}
        ${qrcs}
)

target_include_directories( QLiteHtmlBrowser PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)

target_link_libraries(QLiteHtmlBrowser PRIVATE litehtml Qt5::Widgets)

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
