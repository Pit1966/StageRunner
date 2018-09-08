include(../configvars.pri)

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += midi
SUBDIRS += artnet
SUBDIRS += dmxusb

unix:!macx {
SUBDIRS += yadi

# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}



SUBDIRS += yadi
