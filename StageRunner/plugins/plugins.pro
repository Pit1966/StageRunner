include(../configvars.pri)

TEMPLATE = subdirs

SUBDIRS += midi
SUBDIRS += artnet
SUBDIRS += dmxusb
SUBDIRS += yadi
# SUBDIRS += hid

unix:!macx {
# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}



