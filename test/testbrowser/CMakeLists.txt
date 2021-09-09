
add_executable( testbrowser )

qt5_wrap_cpp ( testbrowser_mocs
  testbrowser.h
)

target_sources( testbrowser
      PRIVATE
      main.cpp
      testbrowser.cpp
      testbrowser.h
      ${testbrowser_mocs}
    )

target_include_directories( testbrowser PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)
target_link_libraries( testbrowser PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Gui Qt5::Test )
