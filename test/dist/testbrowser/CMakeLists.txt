project( QLiteHtmlBrowserDistTest )

cmake_minimum_required( VERSION 3.15 )

set( AUTOMOC OFF)
set( AUTOUIC OFF)
set( AUTORCC OFF)

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS
set( CMAKE_INSTALL_PREFIX "" )

find_package( Qt5 COMPONENTS Widgets Gui Core Help REQUIRED)
find_package( QLiteHtmlBrowser REQUIRED )

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

target_link_libraries( testbrowser PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Help )
