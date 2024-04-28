include(../configvars.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET   = stagerunner


#exists($$(HOME)/DEVELOPER_NO_PRECOMPILED) {
#	message("Disabled PRECOMPILED_HEADER")
#} else {
#	PRECOMPILED_HEADER = ../precompiled_headers.h
#	CONFIG += precompile_header
#}

# PKG_CONFIG = /usr/local/bin/pkg-config   #mac

# CONFIG += static

CONFIG += c++11
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

INCLUDEPATH += /Library/Frameworks/SDL2.framework/Headers
INCLUDEPATH += /Library/Frameworks/SDL2_mixer.framework/Headers
LIBS += -framework SDL2
LIBS += -framework SDL2_mixer
LIBS += -F/Library/Frameworks

DEFINES += IS_MAC
DEFINES += USE_SDL
}

unix:!macx {
	exists(/usr/include/SDL2/SDL.h) {
		exists(/usr/include/SDL2/SDL_mixer.h) {
			message ("Found SDL2 ...")
			INCLUDEPATH += /usr/include/SDL2
			DEFINES += USE_SDL
			LIBS += -lSDL2
			LIBS += -lSDL2_mixer
		} else {
			message("Found SDL2 ... but not SDL_Mixer2")
		}
	}
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
	../appcontrol/appcentral.cpp \
	../appcontrol/colorsettings.cpp \
	../appcontrol/execloop.cpp \
	../appcontrol/execloopthreadinterface.cpp \
	../appcontrol/fxlistvarset.cpp \
	../appcontrol/ioplugincentral.cpp \
	../appcontrol/lightloop.cpp \
	../appcontrol/lightloopthreadinterface.cpp \
	../appcontrol/pluginmapping.cpp \
	../appcontrol/project.cpp \
	../appcontrol/scapplication.cpp \
	../appcontrol/usersettings.cpp \
	../fx/execcenter.cpp \
	../fx/executer.cpp \
	../fx/exttimelineitem.cpp \
	../fx/fxaudioitem.cpp \
	../fx/fxcueitem.cpp \
	../fx/fxitem.cpp \
	../fx/fxitemobj.cpp \
	../fx/fxitemtool.cpp \
	../fx/fxlist.cpp \
	../fx/fxplaylistitem.cpp \
	../fx/fxsceneitem.cpp \
	../fx/fxscriptitem.cpp \
	../fx/fxscripttools.cpp \
	../fx/fxseqitem.cpp \
	../fx/fxtimelineitem.cpp \
	../fx/fxtimelineobj.cpp \
	../fx/timelineexecuter.cpp \
	../gui/audiocontrolwidget.cpp \
	../gui/audioslotwidget.cpp \
	../gui/consolidatedialog.cpp \
	../gui/customwidget/extmimedata.cpp \
	../gui/customwidget/mixerchannel.cpp \
	../gui/customwidget/mixergroup.cpp \
	../gui/customwidget/psdockwidget.cpp \
	../gui/customwidget/psinfodialog.cpp \
	../gui/customwidget/pslabel.cpp \
	../gui/customwidget/pslineedit.cpp \
	../gui/customwidget/pslistwidget.cpp \
	../gui/customwidget/psoverlaylabel.cpp \
	../gui/customwidget/pspushbutton.cpp \
	../gui/customwidget/psspectrometer.cpp \
	../gui/customwidget/psspinbox.cpp \
	../gui/customwidget/psvideowidget.cpp \
	../gui/customwidget/psvumeter.cpp \
	../gui/deviceinfowidget.cpp \
	../gui/fxitempropertywidget.cpp \
	../gui/fxlistwidget.cpp \
	../gui/fxlistwidgetitem.cpp \
	../gui/fxplaylistwidget.cpp \
	../gui/fxscriptwidget.cpp \
	../gui/lightcontrolwidget.cpp \
	../gui/ptablewidget.cpp \
	../gui/scenedeskwidget.cpp \
	../gui/scenestatuswidget.cpp \
	../gui/sequencecontrolwidget.cpp \
	../gui/sequencestatuswidget.cpp \
	../gui/setupwidget.cpp \
	../gui/stagerunnermainwin.cpp \
	../gui/style/lightdeskstyle.cpp \
	../gui/universeeditorwidget.cpp \
	../gui/videostatuswidget.cpp \
	../mods/timeline/timelinebase.cpp \
	../mods/timeline/timelinecursor.cpp \
	../mods/timeline/timelineitem.cpp \
	../mods/timeline/timelineruler.cpp \
	../mods/timeline/timelinewidget.cpp \
	../plugins/yadi/src/dmxmonitor.cpp \
	../system/audiocontrol.cpp \
	../system/audioformat.cpp \
	../system/audioiodevice.cpp \
	../system/audiooutput/audiofileinfo.cpp \
	../system/audiooutput/iodeviceaudiobackend.cpp \
	../system/audiooutput/mediaplayeraudiobackend.cpp \
	../system/audioplayer.cpp \
	../system/audioslot.cpp \
	../system/convtools.cpp \
	../system/dmx/fixture.cpp \
	../system/dmxchannel.cpp \
	../system/dmxchanproperty.cpp \
	../system/dmxuniverseproperty.cpp \
	../system/extelapsedtimer.cpp \
	../system/fxclipitem.cpp \
	../system/fxcontrol.cpp \
	../system/fxtimer.cpp \
	../system/lightcontrol.cpp \
	../system/log.cpp \
	../system/messagedialog.cpp \
	../system/messagehub.cpp \
	../system/netserver.cpp \
	../system/runguard.cpp \
	../system/variantmapserializer.cpp \
	../system/videocontrol.cpp \
	../system/videoplayer.cpp \
	../thirdparty/widget/qsynthdialclassicstyle.cpp \
	../thirdparty/widget/qsynthdialpeppinostyle.cpp \
	../thirdparty/widget/qsynthknob.cpp \
	../thirdparty/widget/qsynthmeter.cpp \
	../tool/database.cpp \
	../tool/dbquery.cpp \
	../tool/fftrealfixlenwrapper.cpp \
	../tool/frqspectrum.cpp \
	../tool/prefvar.cpp \
	../tool/psmovingaverage.cpp \
	../tool/qtstatictools.cpp \
	../tool/toolclasses.cpp \
	../tool/varset.cpp \
	../widgets/editormarginwidget.cpp \
	../widgets/fxtimelineeditwidget.cpp \
	../widgets/scripteditwidget.cpp \
	main.cpp


unix {
	SOURCES += 	../system/unixsignalcatcher.cpp
}

HEADERS  += \
	../appcontrol/appcentral.h \
	../appcontrol/audiocontrol.h \
	../appcontrol/colorsettings.h \
	../appcontrol/execloop.h \
	../appcontrol/execloopthreadinterface.h \
	../appcontrol/fxlistvarset.h \
	../appcontrol/ioplugincentral.h \
	../appcontrol/lightloop.h \
	../appcontrol/lightloopthreadinterface.h \
	../appcontrol/pluginmapping.h \
	../appcontrol/project.h \
	../appcontrol/scapplication.h \
	../appcontrol/usersettings.h \
	../config.h \
	../fx/execcenter.h \
	../fx/executer.h \
	../fx/exttimelineitem.h \
	../fx/fx_defines.h \
	../fx/fxaudioitem.h \
	../fx/fxclipitem.h \
	../fx/fxcueitem.h \
	../fx/fxitem.h \
	../fx/fxitem_includes.h \
	../fx/fxitemobj.h \
	../fx/fxitemtool.h \
	../fx/fxlist.h \
	../fx/fxplaylistitem.h \
	../fx/fxsceneitem.h \
	../fx/fxscriptitem.h \
	../fx/fxscripttools.h \
	../fx/fxseqitem.h \
	../fx/fxtimelineitem.h \
	../fx/fxtimelineobj.h \
	../fx/timelineexecuter.h \
	../gui/audiocontrolwidget.h \
	../gui/audioslotwidget.h \
	../gui/consolidatedialog.h \
	../gui/customwidget/extmimedata.h \
	../gui/customwidget/mixerchannel.h \
	../gui/customwidget/mixergroup.h \
	../gui/customwidget/psdockwidget.h \
	../gui/customwidget/psinfodialog.h \
	../gui/customwidget/pslabel.h \
	../gui/customwidget/pslineedit.h \
	../gui/customwidget/pslistwidget.h \
	../gui/customwidget/psoverlaylabel.h \
	../gui/customwidget/pspushbutton.h \
	../gui/customwidget/psspectrometer.h \
	../gui/customwidget/psspinbox.h \
	../gui/customwidget/psvideowidget.h \
	../gui/customwidget/psvumeter.h \
	../gui/deviceinfowidget.h \
	../gui/fxitempropertywidget.h \
	../gui/fxlistwidget.h \
	../gui/fxlistwidgetitem.h \
	../gui/fxplaylistwidget.h \
	../gui/fxscriptwidget.h \
	../gui/lightcontrolwidget.h \
	../gui/ptablewidget.h \
	../gui/scenedeskwidget.h \
	../gui/scenestatuswidget.h \
	../gui/sequencecontrolwidget.h \
	../gui/sequencestatuswidget.h \
	../gui/setupwidget.h \
	../gui/stagerunnermainwin.h \
	../gui/style/lightdeskstyle.h \
	../gui/universeeditorwidget.h \
	../gui/videostatuswidget.h \
	../mods/timeline/timeline_defines.h \
	../mods/timeline/timelinebase.h \
	../mods/timeline/timelinecursor.h \
	../mods/timeline/timelineitem.h \
	../mods/timeline/timelineruler.h \
	../mods/timeline/timelinewidget.h \
	../plugins/yadi/src/dmxmonitor.h \
	../system/audioformat.h \
	../system/audioiodevice.h \
	../system/audiooutput/audiofileinfo.h \
	../system/audiooutput/iodeviceaudiobackend.h \
	../system/audiooutput/mediaplayeraudiobackend.h \
	../system/audioplayer.h \
	../system/audioslot.h \
	../system/commandsystem.h \
	../system/convtools.h \
	../system/dmx/fixture.h \
	../system/dmxchannel.h \
	../system/dmxchanproperty.h \
	../system/dmxtypes.h \
	../system/dmxuniverseproperty.h \
	../system/extelapsedtimer.h \
	../system/fxcontrol.h \
	../system/fxtimer.h \
	../system/importfxmaster/fxMaster.h \
	../system/lightcontrol.h \
	../system/log.h \
	../system/messagedialog.h \
	../system/messagehub.h \
	../system/netserver.h \
	../system/qt_versions.h \
	../system/runguard.h \
	../system/variantmapserializer.h \
	../system/videocontrol.h \
	../system/videoplayer.h \
	../thirdparty/fftreal2/ffft/Array.h \
	../thirdparty/fftreal2/ffft/Array.hpp \
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
	../thirdparty/fftreal2/ffft/def.h \
	../thirdparty/widget/qsynthdialclassicstyle.h \
	../thirdparty/widget/qsynthdialpeppinostyle.h \
	../thirdparty/widget/qsynthknob.h \
	../thirdparty/widget/qsynthmeter.h \
	../tool/database.h \
	../tool/dbquery.h \
	../tool/fftrealfixlenwrapper.h \
	../tool/frqspectrum.h \
	../tool/prefvar.h \
	../tool/psmovingaverage.h \
	../tool/qtstatictools.h \
	../tool/toolclasses.h \
	../tool/varset.h \
	../tool/varsetlist.h \
	../widgets/editormarginwidget.h \
	../widgets/fxtimelineeditwidget.h \
	../widgets/scripteditwidget.h
#    ../configrev.h \

unix {
	HEADERS +=	../system/unixsignalcatcher.h
}

FORMS    += \
	../gui/audiocontrolwidget.ui \
	../gui/audioslotwidget.ui \
	../gui/consolidatedialog.ui \
	../gui/customwidget/psinfodialog.ui \
	../gui/deviceinfowidget.ui \
	../gui/fxitempropertywidget.ui \
	../gui/fxlistwidget.ui \
	../gui/fxlistwidgetitem.ui \
	../gui/fxplaylistwidget.ui \
	../gui/fxscriptwidget.ui \
	../gui/lightcontrolwidget.ui \
	../gui/scenedeskwidget.ui \
	../gui/scenestatuswidget.ui \
	../gui/sequencecontrolwidget.ui \
	../gui/sequencestatuswidget.ui \
	../gui/setupwidget.ui \
	../gui/stagerunnermainwin.ui \
	../gui/universeeditorwidget.ui \
	../gui/videostatuswidget.ui \
	../system/messagedialog.ui \
	../widgets/fxtimelineeditwidget.ui

RESOURCES += \
	../gfx_ressource.qrc


contains(DEFINES,USE_SDL) {
	message("use SDL2")
	SOURCES +=  ../system/audiooutput/sdl2audiobackend.cpp
	HEADERS += ../system/audiooutput/sdl2audiobackend.h
}

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


OTHER_FILES += \
	version.h.in

DISTFILES += \
    ../devel.txt \
    ../docs/help.txt \
    ../todo.txt \
    ../macos_notes.txt \
    ../platforms/macos/nametool.pri \
    ../platforms/macos/audioplugins-nametool.pri \
    ../platforms/macos/libsndfile-nametool.pri \
    ../platforms/macos/libusb-nametool.pri \
    ../platforms/macos/mediaservice-nametool.pri \
    ../platforms/macos/platformplugins-nametool.pri
