string (TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)
string( TOLOWER ${CMAKE_HOST_SYSTEM_NAME} HOST_SYSTEM_NAME_LC)

set(CPACK_SOURCE_PACKAGE_NAME "${PROJECT_NAME_LC}-${PROJECT_VERSION}-source")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME_LC}-${PROJECT_VERSION}-source")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME_LC}-${PROJECT_VERSION}-${HOST_SYSTEM_NAME_LC}")
set(CPACK_SOURCE_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME_LC} ${PROJECT_VERSION} source code")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME_LC}")

set( CPACK_SOURCE_IGNORE_FILES "\\\\.git/;\\\\.github/;\\\\.gitmodules;doc/;CMakeLists.txt.user;CPack.*;build.*/" )

if( UNIX )
    set( CPACK_SOURCE_GENERATOR TGZ)
    set( CPACK_GENERATOR TGZ)
else()
    set( CPACK_SOURCE_GENERATOR ZIP )
    set( CPACK_GENERATOR ZIP )
endif()

include( CPack )
