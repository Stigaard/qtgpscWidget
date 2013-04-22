cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtGui QtCore)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(qtgpscWidget_SRCS gpswidget.cpp satview.cpp)

qt4_automoc(${qtgpscWidget_SRCS})

ADD_LIBRARY(qtgpscWidget STATIC ${qtgpscWidget_SRCS})
	   
TARGET_LINK_LIBRARIES(qtgpscWidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} gpsReader)