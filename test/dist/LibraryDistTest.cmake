execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/library-dist-test-build-Debug )
execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/library-dist-test-build-Release )
execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/library-dist-test-src )
#execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/home )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${DISTBROWSER_DIR} ${CMAKE_INSTALL_PREFIX}/library-dist-test-src
                 RESULT_VARIABLE BUILD_STATUS
)

#execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/home )

if( NOT "${BUILD_STATUS}" STREQUAL "0" )
  message("module cmake setup returned: ${BUILD_STATUS}")
  message( FATAL_ERROR "error: module cmake setup failed!")
endif()

#set( RUN_ENV "${VENDOR_NAME_UC}_USER_DIR=${CMAKE_INSTALL_PREFIX}/home" )

foreach( buildtype "Release" "Debug")

  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/library-dist-test-build-${buildtype} )
  execute_process( COMMAND ${CMAKE_COMMAND} -E env Qt5_DIR=${QT_INSTALL_DIR} QLiteHtmlBrowser_DIR=${CMAKE_INSTALL_PREFIX} ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${buildtype}  ${CMAKE_INSTALL_PREFIX}/library-dist-test-src
                   RESULT_VARIABLE BUILD_STATUS
                   WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/library-dist-test-build-${buildtype}

  )

  if( NOT "${BUILD_STATUS}" STREQUAL "0" )
    message("module cmake configure ${buildtype} returned: ${BUILD_STATUS}")
    message( FATAL_ERROR "error: module cmake configure ${buildtype} failed!")
  endif()


  execute_process( COMMAND ${CMAKE_COMMAND} --build . --config ${buildtype} --target all
                   RESULT_VARIABLE CPACK_STATUS
                   WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/library-dist-test-build-${buildtype}
  )

  if( NOT "${CPACK_STATUS}" STREQUAL "0" )
    message("module build target ${buildtype} returned: ${CPACK_STATUS}")
    message( FATAL_ERROR "error: module build ${buildtype} failed!")
  endif()
endforeach()
