include(../configvars.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET   = stagerunner


INCLUDEPATH  += .
INCLUDEPATH  += ../
INCLUDEPATH  += ../appcontrol
INCLUDEPATH  += ../fx
INCLUDEPATH  += ../gui
INCLUDEPATH  += ../main
INCLUDEPATH  += ../system
INCLUDEPATH  += ../thirdparty
INCLUDEPATH  += ../tool
INCLUDEPATH  += ../plugins/interfaces

QT += core gui
QT += sql
QT += multimedia

# QT		+= network


greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets

DEFINES += IS_QT5

}

lessThan(QT_MAJOR_VERSION, 5): QT += phonon


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
	../gui/ptablewidget.cpp \
	../appcontrol/ioplugincentral.cpp \
	../gui/customwidget/mixerchannel.cpp \
	../gui/setupwidget.cpp \
	../gui/customwidget/mixergroup.cpp \
	../gui/fxitempropertywidget.cpp \
	../system/dmxchannel.cpp \
	../fx/fxsceneitem.cpp \
	../tool/varset.cpp \
	../gui/scenedeskwidget.cpp \
	../gui/customwidget/pslineedit.cpp \
	../system/lightcontrol.cpp \
	../appcontrol/lightloop.cpp \
	../appcontrol/lightloopthreadinterface.cpp \
	../gui/scenestatuswidget.cpp \
	../gui/lightcontrolwidget.cpp \
	../tool/qtstatictools.cpp \
	../gui/fxlistwidgetitem.cpp \
	../appcontrol/pluginmapping.cpp \
	../gui/customwidget/pslabel.cpp \
	../gui/customwidget/extmimedata.cpp \
	../gui/style/lightdeskstyle.cpp \
	../system/messagehub.cpp \
	../system/messagedialog.cpp \
	../fx/fxplaylistitem.cpp \
	../gui/fxplaylistwidget.cpp \
	../gui/customwidget/pslistwidget.cpp \
	../fx/fxitemtool.cpp \
	../fx/execcenter.cpp \
	../fx/executer.cpp \
	../fx/fxitemobj.cpp \
	../fx/fxseqitem.cpp \
	../system/fxcontrol.cpp \
	../appcontrol/execloopthreadinterface.cpp \
	../appcontrol/execloop.cpp \
	../gui/sequencestatuswidget.cpp \
    ../system/audioplayer.cpp \
    ../appcontrol/scapplication.cpp \
    ../system/unixsignalcatcher.cpp

HEADERS  += \
	../config.h \
#	../configref.h \
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
	../gui/ptablewidget.h \
	../appcontrol/ioplugincentral.h \
	../gui/customwidget/mixerchannel.h \
	../gui/setupwidget.h \
	../gui/customwidget/mixergroup.h \
	../gui/fxitempropertywidget.h \
	../system/dmxchannel.h \
	../fx/fxsceneitem.h \
	../tool/varset.h \
	../tool/varsetlist.h \
	../gui/scenedeskwidget.h \
	../gui/customwidget/pslineedit.h \
	../system/lightcontrol.h \
	../appcontrol/lightloopthreadinterface.h \
	../appcontrol/lightloop.h \
	../gui/scenestatuswidget.h \
	../gui/lightcontrolwidget.h \
	../tool/qtstatictools.h \
	../gui/fxlistwidgetitem.h \
	../appcontrol/pluginmapping.h \
	../gui/customwidget/pslabel.h \
	../gui/customwidget/extmimedata.h \
	../gui/style/lightdeskstyle.h \
	../system/messagehub.h \
	../system/messagedialog.h \
	../fx/fxplaylistitem.h \
	../gui/fxplaylistwidget.h \
	../gui/customwidget/pslistwidget.h \
	../fx/fxitemtool.h \
	../fx/execcenter.h \
	../fx/executer.h \
	../fx/fxitemobj.h \
	../fx/fxseqitem.h \
	../system/fxcontrol.h \
	../appcontrol/execloopthreadinterface.h \
	../appcontrol/execloop.h \
	../gui/sequencestatuswidget.h \
    ../system/audioplayer.h \
    ../appcontrol/scapplication.h \
    ../system/unixsignalcatcher.h

FORMS    += \
	../gui/fxlistwidget.ui \
	../gui/stagerunnermainwin.ui \
	../gui/sequencecontrolwidget.ui \
	../gui/audioslotwidget.ui \
	../gui/audiocontrolwidget.ui \
	../gui/setupwidget.ui \
	../gui/fxitempropertywidget.ui \
	../gui/scenedeskwidget.ui \
	../gui/scenestatuswidget.ui \
	../gui/lightcontrolwidget.ui \
	../gui/fxlistwidgetitem.ui \
	../system/messagedialog.ui \
	../gui/fxplaylistwidget.ui \
	../gui/sequencestatuswidget.ui

RESOURCES += \
	../gfx_ressource.qrc


# Installation
target.path = $$INSTALLROOT/$$BINDIR
INSTALLS   += target


#############################################################################
# version.h
#############################################################################
	GIT_VERSION = $$system($$quote(git describe))
	GIT_TIMESTAMP = $$system($$quote(git log -n 1 --format=format:"%at"))

	QMAKE_SUBSTITUTES += $$PWD/version.h.in



#############################################################################
# configrev.h generation
#############################################################################

CONFIGFILE = configrev.h
conf.target = $$CONFIGFILE
QMAKE_EXTRA_TARGETS += conf
PRE_TARGETDEPS += $$CONFIGFILE
QMAKE_CLEAN += $$CONFIGFILE
QMAKE_DISTCLEAN += $$CONFIGFILE

macx {
	conf.commands += echo \"$$LITERAL_HASH ifndef CONFIGREV_H\" > $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define CONFIGREV_H\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APPNAME \\\"$$APPNAME\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APPVERSION \\\"$$APPVERSION\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define DOCSDIR \\\"$$DOCSDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define PLUGINDIR \\\"$$PLUGINDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define TRANSLATIONDIR \\\"$$TRANSLATIONDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH endif\" >> $$CONFIGFILE
}
unix:!macx {
	conf.commands += echo \"$$LITERAL_HASH ifndef CONFIGREV_H\" > $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define CONFIGREV_H\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APPNAME \\\"$$APPNAME\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APPVERSION \\\"$$APPVERSION\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define DOCSDIR \\\"$$INSTALLROOT/$$DOCSDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define PLUGINDIR \\\"$$INSTALLROOT/$$PLUGINDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define TRANSLATIONDIR \\\"$$INSTALLROOT/$$TRANSLATIONDIR\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH endif\" >> $$CONFIGFILE
}
win32 {
	conf.commands += @echo $$LITERAL_HASH ifndef CONFIG_H > $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define CONFIG_H >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define APPNAME \"$$APPNAME\" >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define APPVERSION \"$$APPVERSION\" >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define DOCSDIR \"$$DOCSDIR\" >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define PLUGINDIR \"$$PLUGINDIR\" >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH define TRANSLATIONDIR \"$$TRANSLATIONDIR\" >> $$CONFIGFILE &&
	conf.commands += @echo $$LITERAL_HASH endif >> $$CONFIGFILE
}

OTHER_FILES += \
    version.h.in
