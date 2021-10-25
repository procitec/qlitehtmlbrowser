project( TestBrowser )

find_package( Qt5 COMPONENTS Widgets Gui Core Help REQUIRED)


add_executable( testbrowser )

qt5_wrap_cpp ( testbrowser_mocs
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
target_link_libraries( testbrowser PRIVATE QLiteHtmlBrowser Qt::Widgets Qt::Gui Qt::Core Qt::Help )
