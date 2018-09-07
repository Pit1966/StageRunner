include(../configvars.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET   = stagerunner

# CONFIG += static

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

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
INCLUDEPATH  += ../thirdparty/fftreal2

macx {
INCLUDEPATH += ../SDL2/SDL2
INCLUDEPATH += ../SDL2

INCLUDEPATH += /Library/Frameworks/SDL2.framewor/Headers
INCLUDEPATH += /Library/Frameworks/SDL2_mixer.framework/Headers
LIBS += -framework SDL2
LIBS += -framework SDL2_mixer
LIBS += -F/Library/Frameworks

DEFINES += USE_SDL
}

unix:!macx {
INCLUDEPATH += /usr/include/SDL
DEFINES += USE_SDL
LIBS += -lSDL2
LIBS += -lSDL2_mixer
}


QT += core gui
QT += sql
QT += multimedia
# QT		+= network

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += multimediawidgets

	DEFINES += IS_QT5
} else {
	message(Compiling with Qt4 is not supported!. Please use Qt5)
}

win32 {
QMAKE_LFLAGS += /INCREMENTAL:NO
}


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
	../tool/psmovingaverage.cpp \
	../tool/frqspectrum.cpp \
	../gui/customwidget/psspectrometer.cpp \
	../appcontrol/fxlistvarset.cpp \
	../tool/fftrealfixlenwrapper.cpp \
	../gui/customwidget/psvumeter.cpp \
	../system/videocontrol.cpp \
	../system/fxclipitem.cpp \
	../gui/customwidget/psvideowidget.cpp \
	../system/videoplayer.cpp \
	../system/dmxchanproperty.cpp \
	../system/dmxuniverseproperty.cpp \
	../fx/fxscriptitem.cpp \
    ../gui/fxscriptwidget.cpp \
	../fx/fxscripttools.cpp \
	../fx/fxcueitem.cpp \
    ../plugins/yadi/src/dmxmonitor.cpp \
	../gui/customwidget/psinfodialog.cpp \
	../system/variantmapserializer.cpp \
    ../gui/customwidget/psdockwidget.cpp

unix {
SOURCES += 	../system/unixsignalcatcher.cpp
}

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
	../tool/psmovingaverage.h \
	../tool/frqspectrum.h \
	../gui/customwidget/psspectrometer.h \
	../appcontrol/fxlistvarset.h \
	../thirdparty/fftreal2/ffft/Array.h \
	../thirdparty/fftreal2/ffft/Array.hpp \
	../thirdparty/fftreal2/ffft/def.h \
	../thirdparty/fftreal2/ffft/DynArray.h \
	../thirdparty/fftreal2/ffft/DynArray.hpp \
	../thirdparty/fftreal2/ffft/FFTReal.h \
	../thirdparty/fftreal2/ffft/FFTReal.hpp \
	../thirdparty/fftreal2/ffft/FFTRealFixLen.h \
	../thirdparty/fftreal2/ffft/FFTRealFixLen.hpp \
	../thirdparty/fftreal2/ffft/FFTRealFixLenParam.h \
	../thirdparty/fftreal2/ffft/FFTRealPassDirect.h \
	../thirdparty/fftreal2/ffft/FFTRealPassDirect.hpp \
	../thirdparty/fftreal2/ffft/FFTRealPassInverse.h \
	../thirdparty/fftreal2/ffft/FFTRealPassInverse.hpp \
	../thirdparty/fftreal2/ffft/FFTRealSelect.h \
	../thirdparty/fftreal2/ffft/FFTRealSelect.hpp \
	../thirdparty/fftreal2/ffft/FFTRealUseTrigo.h \
	../thirdparty/fftreal2/ffft/FFTRealUseTrigo.hpp \
	../thirdparty/fftreal2/ffft/OscSinCos.h \
	../thirdparty/fftreal2/ffft/OscSinCos.hpp \
	../tool/fftrealfixlenwrapper.h \
	../gui/customwidget/psvumeter.h \
	../system/dmxtypes.h \
	../system/videocontrol.h \
	../system/fxclipitem.h \
	../gui/customwidget/psvideowidget.h \
	../system/videoplayer.h \
	../system/dmxchanproperty.h \
	../system/dmxuniverseproperty.h \
	../fx/fxscriptitem.h \
    ../gui/fxscriptwidget.h \
	../fx/fxscripttools.h \
	../fx/fxcueitem.h \
    ../plugins/yadi/src/dmxmonitor.h \
	../gui/customwidget/psinfodialog.h \
	../system/variantmapserializer.h \
    ../gui/customwidget/psdockwidget.h

unix {
HEADERS +=	../system/unixsignalcatcher.h
}

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
	../gui/sequencestatuswidget.ui \
	../gui/customwidget/psinfodialog.ui \
	../gui/fxscriptwidget.ui

RESOURCES += \
	../gfx_ressource.qrc



# Installation
target.path = $$INSTALLROOT/$$BINDIR
INSTALLS   += target


#############################################################################
# version.h
#############################################################################
	GIT_VERSION = $$system($$quote(git describe --always))
	GIT_TIMESTAMP = $$system($$quote(git log -n 1 --format=format:"%at"))

	QMAKE_SUBSTITUTES += $$PWD/version.h.in
#	QMAKE_SUBSTITUTES += version.h.in


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

DISTFILES += \
    ../todo.txt \
    ../macos_notes.txt \
    ../platforms/macos/nametool.pri \
    ../platforms/macos/audioplugins-nametool.pri \
    ../platforms/macos/libsndfile-nametool.pri \
    ../platforms/macos/libusb-nametool.pri \
    ../platforms/macos/mediaservice-nametool.pri \
    ../platforms/macos/platformplugins-nametool.pri
