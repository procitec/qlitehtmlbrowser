project( QLiteHtmlBrowserTest )

find_package( Qt5 COMPONENTS Test REQUIRED )

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
      test_base.h
      ${${name}_mocs}
    )

    target_include_directories( ${name} PRIVATE ${QLiteHtmlBrowser_SOURCE_DIR}/include )

    target_link_libraries(${name} PRIVATE QLiteHtmlBrowser Qt5::Widgets Qt5::Gui Qt5::Test )

    add_test( NAME ${name} COMMAND ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endforeach()
