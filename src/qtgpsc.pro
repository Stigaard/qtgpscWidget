TEMPLATE     = app
CONFIG      += debug
RESOURCES   += qtgpsc.qrc
FORMS        = MainWindow.ui GpsdRawDataViewer.ui
LIBS        += -lgps -lm
HEADERS     += gpsd.h qtgpsc.h satview.h sattable.h gpsdrawdataviewer.h
SOURCES     += main.cpp gpsd.cpp qtgpsc.cpp satview.cpp sattable.cpp gpsdrawdataviewer.cpp
