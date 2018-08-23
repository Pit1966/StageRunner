include(../configvars.pri)

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += midi
unix:!macx {
SUBDIRS += yadi
SUBDIRS += dmxusb
}

SUBDIRS += artnet
#SUBDIRS += dmxusb

#SUBDIRS += enttecdmxusb
#SUBDIRS += midi
