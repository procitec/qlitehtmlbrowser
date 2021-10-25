include(CMakePackageConfigHelpers)

set( INCLUDE_INSTALL_DIR "include")
set( LIBRARY_INSTALL_DIR "lib")
set( LIBRARY_VERSION ${QLiteHtmlBrowser_VERSION})
set( QT_VERSION ${Qt5Core_VERSION} )

configure_package_config_file(QLiteHtmlBrowserConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/QLiteHtmlBrowserConfig.cmake
  INSTALL_DESTINATION lib/QLiteHtmlBrowser/cmake
  PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR LIBRARY_VERSION )

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/QLiteHtmlBrowserConfigVersion.cmake
  VERSION ${LIBRARY_VERSION}
  COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QLiteHtmlBrowserConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/QLiteHtmlBrowserConfigVersion.cmake
        DESTINATION ${LIBRARY_INSTALL_DIR}/cmake/QLiteHtmlBrowser COMPONENT QLiteHtmlBrowser)


install(EXPORT "QLiteHtmlBrowserTargets"
    FILE "QLiteHtmlBrowserTargets.cmake"
    NAMESPACE QLiteHtmlBrowser::
    DESTINATION ${LIBRARY_INSTALL_DIR}/cmake/QLiteHtmlBrowser
    COMPONENT QLiteHtmlBrowser
)
