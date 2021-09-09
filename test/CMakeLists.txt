set( test_names
    test_html_content
    test_css_content
)

foreach( name ${test_names})

    add_executable( ${name} )

    qt5_wrap_cpp ( ${name}_mocs
        ${name}.h
        test_base.h
    )

    target_sources(${name}
      PRIVATE
      ${name}.cpp
      ${name}.h
      ${${name}_mocs}
    )
    target_include_directories( ${name} PUBLIC ${PROJECT_SOURCE_DIR}/include/qlitehtmlbrowser)

    target_link_libraries(${name} PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Gui Qt5::Test )

    add_test( NAME ${name} COMMAND ${name} )

endforeach()

add_subdirectory( testbrowser )

