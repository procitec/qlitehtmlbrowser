#add_library(QLiteHtmlBrowserIfc INTERFACE)

#target_include_directories(QLiteHtmlBrowserIfc
#  INTERFACE
#    ${CMAKE_CURRENT_SOURCE_DIR}
#  )

#target_sources(QLiteHtmlBrowserIfc
#  INTERFACE
#    ${CMAKE_CURRENT_SOURCE_DIR}/qlitehtmlbrowser/QLiteHtmlBrowser.h
#)

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/qlitehtmlbrowser/QLiteHtmlBrowser.h
               ${CMAKE_CURRENT_SOURCE_DIR}/qlitehtmlbrowser/QLiteHtmlBrowser
         DESTINATION "include/qlitehtmlbrowser" COMPONENT QLiteHtmlBrowser )

