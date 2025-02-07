project( TestBrowser )

find_package( Qt6 COMPONENTS Widgets Gui Core Help REQUIRED)


add_executable( testbrowser )

qt_wrap_cpp ( testbrowser_mocs
  testbrowser.h
  QHelpBrowser.h
)

target_sources( testbrowser
      PRIVATE
      main.cpp
      testbrowser.cpp
      testbrowser.h
      QHelpBrowser.cpp
      QHelpBrowser.h
      ${testbrowser_mocs}
    )

target_include_directories(testbrowser PRIVATE "${QLiteHtmlBrowser_SOURCE_DIR}/include")
target_link_libraries( testbrowser PRIVATE QLiteHtmlBrowser Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Help )
