add_executable( qlitehtmlbrowsertest )

qt5_wrap_cpp ( mocs
    QLiteHtmlBrowserTest.h
)

target_sources(qlitehtmlbrowsertest
  PRIVATE
  QLiteHtmlBrowserTest.cpp
  QLiteHtmlBrowserTest.h
  ${mocs}
)
target_include_directories( qlitehtmlbrowsertest PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)

target_link_libraries(qlitehtmlbrowsertest PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Test )

add_test( NAME qlitehtmlbrowsertest COMMAND qlitehtmlbrowsertest )
