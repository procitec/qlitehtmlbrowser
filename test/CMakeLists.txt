add_executable( test_html_content )

qt5_wrap_cpp ( mocs
    test_html_content.h
    test_base.h
)

target_sources(test_html_content
  PRIVATE
  test_html_content.cpp
  test_html_content.h
  ${mocs}
)
target_include_directories( test_html_content PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)

target_link_libraries(test_html_content PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Gui Qt5::Test )

add_test( NAME test_html_content COMMAND test_html_content )
