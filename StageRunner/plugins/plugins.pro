include(../configvars.pri)

TEMPLATE = subdirs

#SUBDIRS += midi
#SUBDIRS += artnet
#SUBDIRS += yadi
#SUBDIRS += hid
#SUBDIRS += dmxusb

unix:!macx {
# old - substituted by dmxusb
#SUBDIRS += enttecdmxusb
}

