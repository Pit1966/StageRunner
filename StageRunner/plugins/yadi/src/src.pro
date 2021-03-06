include (../../../configvars.pri)

TEMPLATE = lib
LANGUAGE = C++
TARGET   = ../../yadidmxusbout

CONFIG      += plugin
CONFIG		+= debug
QT          += gui core

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets

DEFINES += IS_QT5
}

INCLUDEPATH += .
INCLUDEPATH += ../../interfaces

FORMS += \
	yadiconfigdialog.ui

HEADERS += \
	../../interfaces/qlcioplugin.h \
	yadidmxusbout.h \
	serialwrapper.h \
	yadidevice.h \
	yadidevicemanager.h \
	yadiconfigdialog.h \
	yadireceiver.h \
    dmxmonitor.h \
    mvgavgcollector.h

SOURCES += ../../interfaces/qlcioplugin.cpp \
    mvgavgcollector.cpp \
    yadidmxusbout.cpp \
    serialwrapper.cpp \
    yadidevice.cpp \
    yadidevicemanager.cpp \
    yadiconfigdialog.cpp \
    yadireceiver.cpp \
    dmxmonitor.cpp

OTHER_FILES += yadi.json


unix:!macx {
	QT      += dbus
	DEFINES += DBUS_ENABLED
	LIBS	+= -ludev

#    CONFIG += qtserialsupport			# compile QtSerialPort funtions
#	CONFIG += qtserial					# use QtSerial for interface

# Rules to make yadi devices readable & writable by normal users
#    udev.path  = /etc/udev/rules.d
#    udev.files = z65-enttec-dmxusb.rules
#    INSTALLS  += udev

}

macx: {
    CONFIG += qtserialsupport			# compile QtSerialPort funtions
#    CONFIG += qtserial					# use QtSerial for interface

}

CONFIG(qtserialsupport) {
    message(Building with QtSerialport support.)
    QT += serialport
    DEFINES += HAS_QTSERIAL

    CONFIG(qtserial) {
    message(  Use QtSerialport for yadi.)
        DEFINES += USE_QTSERIAL

        SOURCES += qserialportthread.cpp
        HEADERS += qserialportthread.h

        SOURCES -= yadireceiver.cpp
        HEADERS -= yadireceiver.h
    }
}

PRO_FILE      = src.pro

# This must be after "TARGET = " and before target installation so that
# install_name_tool can be run before target installation
# macx:include(../../../macx/nametool.pri)

# Plugin installation
target.path = $$INSTALLROOT/$$PLUGINDIR
INSTALLS   += target

OTHER_FILES += \
	yadi.json

DISTFILES += \
    serial_notes.txt
