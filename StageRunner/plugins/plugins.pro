include(../configvars.pri)

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += midi
SUBDIRS += artnet

unix:!macx {
SUBDIRS += yadi
SUBDIRS += dmxusb

# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}

#SUBDIRS += dmxusb
#SUBDIRS += midi


