include(../configvars.pri)

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += midi
SUBDIRS += artnet
SUBDIRS += dmxusb
SUBDIRS += yadi

unix:!macx {

# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}



