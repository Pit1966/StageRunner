include(../../../configvars.pri)

TEMPLATE = lib
LANGUAGE = C++
TARGET   = ../../midiplugin

CONFIG      += plugin
CONFIG      += link_pkgconfig
PKGCONFIG   += alsa

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets

DEFINES += IS_QT5

}

include(../common/common.pri)

HEADERS += alsamidiinputthread.h \
		   alsamidiinputdevice.h \
		   alsamidioutputdevice.h \
		   alsamidienumerator.h \
		   alsamidiutil.h

SOURCES += alsamidiinputthread.cpp \
		   alsamidiinputdevice.cpp \
		   alsamidioutputdevice.cpp \
		   alsamidienumerator.cpp \
		   alsamidiutil.cpp
