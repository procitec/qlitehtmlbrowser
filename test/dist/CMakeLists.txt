add_test( NAME library-dist-install
          COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/library-dist-test-install -P ${CMAKE_CURRENT_SOURCE_DIR}/LibraryDistTestInstall.cmake
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties( library-dist-install PROPERTIES FIXTURES_SETUP library-dist-test )


# Describe property
define_property(GLOBAL PROPERTY QT_INSTALL_DIR
    BRIEF_DOCS "Base QT install dir"
    FULL_DOCS "Basic root installation dir of QT"
)

# Describe property
define_property(GLOBAL PROPERTY QT_LIBRARY_DIR
    BRIEF_DOCS "Base QT library dir"
    FULL_DOCS "Basic library dir of QT"
)

get_target_property( QMAKE Qt::qmake IMPORTED_LOCATION )

get_filename_component( QT_BIN_DIR ${QMAKE} DIRECTORY)
get_filename_component( QT_INSTALL_DIR "${QT_BIN_DIR}/../" ABSOLUTE )
if( UNIX )
  get_filename_component( QT_LIBRARY_DIR "${QT_INSTALL_DIR}/lib" ABSOLUTE )
else()
  get_filename_component( QT_LIBRARY_DIR "${QT_INSTALL_DIR}/bin" ABSOLUTE )
endif()

set_property(GLOBAL PROPERTY QT_INSTALL_DIR "${QT_INSTALL_DIR}")
set_property(GLOBAL PROPERTY QT_LIBRARY_DIR "${QT_LIBRARY_DIR}")

add_test( NAME library-dist-cmake
          COMMAND ${CMAKE_COMMAND} -DQT_INSTALL_DIR=${QT_INSTALL_DIR} -DDISTBROWSER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/distbrowser -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/library-dist-test-install -P ${CMAKE_CURRENT_SOURCE_DIR}/LibraryDistTest.cmake
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_tests_properties( "library-dist-cmake" PROPERTIES FIXTURES_REQUIRED library-dist-test )
