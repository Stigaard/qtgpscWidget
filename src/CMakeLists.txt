project(qtgpsc)
 
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(Qt4 REQUIRED)
find_package(Gpsd REQUIRED)
# include(QT4Defaults)
 
add_definitions (${QT_DEFINITIONS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${QT4_INCLUDES}
   ${LIBGPS_INCLUDE_DIR}
   )

set(qtgpsc_SRCS gpsd.cpp  gpsdrawdataviewer.cpp  main.cpp  qtgpsc.cpp  sattable.cpp  satview.cpp)
set(qtgpsc_MOC gpsd.h  gpsdrawdataviewer.h  qtgpsc.h  sattable.h  satview.h)
set(qtgpsc_UI GpsdRawDataViewer.ui  MainWindow.ui)
set(qtgpsc_RCS qtgpsc.qrc)

include( ${QT_USE_FILE} )
  
# this command will generate rules that will run rcc on all files from qtgpsc_RCS
# in result qtgpsc_RC_SRCS variable will contain paths to files produced by rcc
qt4_add_resources( qtgpsc_RC_SRCS ${qtgpsc_RCS} )
  
# this will run uic on .ui files:
qt4_wrap_ui( qtgpsc_UI_HDRS ${qtgpsc_UI} )
  
# and finally this will run moc:
qt4_wrap_cpp( qtgpsc_MOC_SRCS ${qtgpsc_MOC} )
  
# here we instruct CMake to build the executable from all of the source files
add_executable( qtgpsc ${qtgpsc_SRCS} ${qtgpsc_MOC_SRCS} ${qtgpsc_RC_SRCS} ${qtgpsc_UI_HDRS} )
  
# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
target_link_libraries( qtgpsc ${QT_LIBRARIES} ${LIBGPS_LIBRARIES})

set (qtgpsc_DESKTOP_FILE qtgpsc.desktop)
configure_file (${qtgpsc_DESKTOP_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${qtgpsc_DESKTOP_FILE})

install(TARGETS qtgpsc DESTINATION bin)
 
install(FILES images/qtgpsc.png
        DESTINATION share/icons/hicolor/48x48/apps)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${qtgpsc_DESKTOP_FILE}
        DESTINATION share/applications)

# # Install icons
# add_subdirectory (img)
 
