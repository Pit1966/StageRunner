include(../configvars.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET   = stagerunner


INCLUDEPATH  += ../
INCLUDEPATH  += ../fx
INCLUDEPATH  += ../gui
INCLUDEPATH  += ../main
INCLUDEPATH  += ../system
INCLUDEPATH  += ../thirdparty
INCLUDEPATH  += ../tool

QT += core gui
QT += sql
QT += multimedia

# QT		+= network


greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets

DEFINES += IS_QT5

}

lessThan(QT_MAJOR_VERSION, 5): QT += phonon

# INCLUDEPATH += D:\work\PeterCoding\git_StageRunner\StageRunner

SOURCES += \
	main.cpp \
	../fx/fxitem.cpp \
	../fx/fxlist.cpp \
	../fx/fxaudioitem.cpp \
	../system/audiocontrol.cpp \
	../system/audiochannel.cpp \
	../system/log.cpp \
	../system/audioiodevice.cpp \
	../system/audioformat.cpp \
	../appcontrol/appcentral.cpp \
	../appcontrol/project.cpp \
	../appcontrol/usersettings.cpp \
	../tool/toolclasses.cpp \
	../tool/prefvarset.cpp \
	../tool/prefvar.cpp \
	../tool/dbquery.cpp \
	../tool/database.cpp \
	../thirdparty/widget/qsynthdialpeppinostyle.cpp \
	../thirdparty/widget/qsynthknob.cpp \
	../thirdparty/widget/qsynthmeter.cpp \
	../thirdparty/widget/qsynthdialclassicstyle.cpp \
	../gui/fxlistwidget.cpp \
	../gui/stagerunnermainwin.cpp \
	../gui/sequencecontrolwidget.cpp \
	../gui/audioslotwidget.cpp \
	../gui/audiocontrolwidget.cpp \
	../gui/ptablewidget.cpp

HEADERS  += \
	../config.h \
	../fx/fxitem.h \
	../fx/fxlist.h \
	../fx/fxaudioitem.h \
	../system/audiocontrol.h \
	../system/audiochannel.h \
	../system/log.h \
	../system/commandsystem.h \
	../system/audioiodevice.h \
	../system/audioformat.h \
	../appcontrol/appcentral.h \
	../appcontrol/project.h \
	../appcontrol/usersettings.h \
	../tool/toolclasses.h \
	../tool/prefvarset.h \
	../tool/prefvar.h \
	../tool/dbquery.h \
	../tool/database.h \
	../thirdparty/widget/qsynthdialpeppinostyle.h \
	../thirdparty/widget/qsynthknob.h \
	../thirdparty/widget/qsynthmeter.h \
	../thirdparty/widget/qsynthdialclassicstyle.h \
	../gui/fxlistwidget.h \
	../gui/stagerunnermainwin.h \
	../gui/sequencecontrolwidget.h \
	../gui/audioslotwidget.h \
	../gui/audiocontrolwidget.h \
	../gui/ptablewidget.h

FORMS    += \
	../gui/fxlistwidget.ui \
	../gui/stagerunnermainwin.ui \
	../gui/sequencecontrolwidget.ui \
	../gui/audioslotwidget.ui \
	../gui/audiocontrolwidget.ui

RESOURCES += \
	../gfx_ressource.qrc


# Installation
target.path = $$INSTALLROOT/$$BINDIR
INSTALLS   += target
