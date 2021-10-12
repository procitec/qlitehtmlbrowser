add_test( NAME library-dist-install
          COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/library-dist-test-install -P ${CMAKE_CURRENT_SOURCE_DIR}/LibraryDistTestInstall.cmake
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties( library-dist-install PROPERTIES FIXTURES_SETUP library-dist-test )


# Describe property
define_property(GLOBAL PROPERTY QT5_INSTALL_DIR
    BRIEF_DOCS "Base Qt5 install dir"
    FULL_DOCS "Basic root installation dir of qt5"
)

# Describe property
define_property(GLOBAL PROPERTY QT5_LIBRARY_DIR
    BRIEF_DOCS "Base Qt5 library dir"
    FULL_DOCS "Basic library dir of qt5"
)

get_target_property( QMAKE Qt5::qmake IMPORTED_LOCATION )

get_filename_component( QT5_BIN_DIR ${QMAKE} DIRECTORY)
get_filename_component( QT5_INSTALL_DIR "${QT5_BIN_DIR}/../" ABSOLUTE )
if( UNIX )
  get_filename_component( QT5_LIBRARY_DIR "${QT5_INSTALL_DIR}/lib" ABSOLUTE )
else()
  get_filename_component( QT5_LIBRARY_DIR "${QT5_INSTALL_DIR}/bin" ABSOLUTE )
endif()

set_property(GLOBAL PROPERTY QT5_INSTALL_DIR "${QT5_INSTALL_DIR}")
set_property(GLOBAL PROPERTY QT5_LIBRARY_DIR "${QT5_LIBRARY_DIR}")

add_test( NAME library-dist-cmake
          COMMAND ${CMAKE_COMMAND} -DQT5_INSTALL_DIR=${QT5_INSTALL_DIR} -DTESTBROWSER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/testbrowser -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/library-dist-test-install -P ${CMAKE_CURRENT_SOURCE_DIR}/LibraryDistTest.cmake
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_tests_properties( "library-dist-cmake" PROPERTIES FIXTURES_REQUIRED library-dist-test )
