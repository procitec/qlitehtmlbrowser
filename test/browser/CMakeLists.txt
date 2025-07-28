project( TestBrowser )

find_package(Qt6 COMPONENTS Core Gui Widgets Help)
if(Qt6_FOUND)
  set(QT_VERSION_MAJOR 6)
else()
  set(QT_VERSION_MAJOR 5)
  find_package(Qt5 5.15 COMPONENTS Core Gui Widgets Help REQUIRED)
endif()

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
target_link_libraries( testbrowser PRIVATE QLiteHtmlBrowser Qt::Widgets Qt::Gui Qt::Core Qt::Help )
