add_library( QLiteHtmlBrowser STATIC "")

qt5_wrap_cpp ( mocs
    container_qt.h
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
    )

target_sources(QLiteHtmlBrowser
  PRIVATE
    QLiteHtmlBrowser.cpp
    container_qt.cpp
    container_qt.h
    ${mocs}

  PUBLIC
    ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h

  )
  
target_include_directories( QLiteHtmlBrowser PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)

target_link_libraries(QLiteHtmlBrowser PRIVATE litehtml Qt5::Widgets)
