project( QLiteHtmlBrowserTest )

find_package(Qt6 COMPONENTS Core Gui Widgets Test)
if(Qt6_FOUND)
  set(QT_VERSION_MAJOR 6)
else()
  find_package(Qt5 5.15 COMPONENTS Core Gui Widgets Test REQUIRED)
  set(QT_VERSION_MAJOR 5)
endif()

add_compile_definitions(-DUNIT_TEST)
add_compile_definitions(-DQLITEHTMLBROWSER_LIBRARY)

set( test_names
    test_html_content
    test_css_content
    test_history
    test_find
    test_selection
)

set( runpath "$<TARGET_FILE_DIR:QLiteHtmlBrowser>;$<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Widgets>")
if(WIN32)
  string(REPLACE ";" "\\\;" runpath "${runpath}")
endif()

list(APPEND environment "$<IF:$<PLATFORM_ID:Windows>,PATH,LD_LIBRARY_PATH>=$<SHELL_PATH:${runpath}>")

qt_add_resources(_RESOURCES fonts.qrc)

foreach( name ${test_names})

    add_executable( ${name} )

    qt_wrap_cpp ( ${name}_mocs
        ${name}.h
        test_base.h
        ${QLiteHtmlBrowser_SOURCE_DIR}/src/container_qt.h
        ${QLiteHtmlBrowser_SOURCE_DIR}/src/QLiteHtmlBrowserImpl.h
        ${QLiteHtmlBrowser_SOURCE_DIR}/src/TextManager.h
        ${QLiteHtmlBrowser_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
    )

    target_sources(${name}
      PRIVATE
      ${name}.cpp
      ${name}.h
      test_base.h
      ${${name}_mocs}
      ${QLiteHtmlBrowser_SOURCE_DIR}/include/qlitehtmlbrowser/QLiteHtmlBrowser.h
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/QLiteHtmlBrowser.cpp
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/QLiteHtmlBrowserImpl.cpp
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/QLiteHtmlBrowserImpl.h
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/container_qt.h
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/container_qt.cpp
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/TextManager.cpp
      ${QLiteHtmlBrowser_SOURCE_DIR}/src/TextManager.h
      ${_RESOURCES}
    )

    target_include_directories( ${name} PRIVATE ${QLiteHtmlBrowser_SOURCE_DIR}/include ${QLiteHtmlBrowser_SOURCE_DIR}/src)

    target_link_libraries(${name} PUBLIC litehtml Qt::Widgets Qt::Gui Qt::Test )

    target_compile_definitions(${name} PRIVATE TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

    add_test( NAME ${name} COMMAND ${name} --searchpath=${CMAKE_CURRENT_SOURCE_DIR})

    set_tests_properties(${name} PROPERTIES
        ENVIRONMENT "${environment}"
    )


endforeach()
