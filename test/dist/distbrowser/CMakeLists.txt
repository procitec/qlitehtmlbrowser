project( QLiteHtmlBrowserDistTest )

cmake_minimum_required( VERSION 3.15 )

set( AUTOMOC OFF)
set( AUTOUIC OFF)
set( AUTORCC OFF)

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS
set( CMAKE_INSTALL_PREFIX "" )

find_package( Qt5 COMPONENTS Widgets REQUIRED)
find_package( QLiteHtmlBrowser REQUIRED )

add_executable( distbrowser )

qt5_wrap_cpp ( distbrowser_mocs
  distbrowser.h
  QDistBrowser.h
)

target_sources( distbrowser
      PRIVATE
      main.cpp
      distbrowser.cpp
      distbrowser.h
      QDistBrowser.cpp
      QDistBrowser.h
      ${distbrowser_mocs}
    )

target_link_libraries( distbrowser PRIVATE QLiteHtmlBrowser::QLiteHtmlBrowser Qt::Widgets Qt::Gui Qt::Core )
