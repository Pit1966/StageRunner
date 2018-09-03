include(../configvars.pri)

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += midi
unix:!macx {
SUBDIRS += yadi
SUBDIRS += dmxusb

# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}

SUBDIRS += artnet
#SUBDIRS += dmxusb
#SUBDIRS += midi


