include(../../../../configvars.pri)

TEMPLATE = lib
LANGUAGE = C++
TARGET   = ../../midiplugin

CONFIG      += plugin
CONFIG      += link_pkgconfig
PKGCONFIG   += alsa

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

include(../common/common.pri)
