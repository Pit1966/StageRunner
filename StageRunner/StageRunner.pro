QT += core gui
QT += sql
QT += multimedia

# QT		+= network


greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets

DEFINES += IS_QT5

}

lessThan(QT_MAJOR_VERSION, 5): QT += phonon


TARGET = StageRunner
TEMPLATE = app

# INCLUDEPATH += D:\work\PeterCoding\git_StageRunner\StageRunner
INCLUDEPATH += .

SOURCES += main.cpp\
	fx/fxitem.cpp \
	gui/fxlistwidget.cpp \
	system/audiocontrol.cpp \
	system/audiochannel.cpp \
	main/appcentral.cpp \
	gui/stagerunnermainwin.cpp \
	fx/fxlist.cpp \
	fx/fxaudioitem.cpp \
	tool/toolclasses.cpp \
	tool/prefvarset.cpp \
	tool/prefvar.cpp \
	tool/dbquery.cpp \
	tool/database.cpp \
	system/log.cpp \
	main/project.cpp \
	main/usersettings.cpp \
	gui/sequencecontrolwidget.cpp \
	thirdparty/widget/qsynthdialpeppinostyle.cpp \
	thirdparty/widget/qsynthknob.cpp \
	thirdparty/widget/qsynthmeter.cpp \
	thirdparty/widget/qsynthdialclassicstyle.cpp \
	gui/audioslotwidget.cpp \
	gui/audiocontrolwidget.cpp \
	system/audioiodevice.cpp \
	system/audioformat.cpp \
	gui/ptablewidget.cpp

HEADERS  += \
	fx/fxitem.h \
	gui/fxlistwidget.h \
	system/audiocontrol.h \
	system/audiochannel.h \
	main/appcentral.h \
	gui/stagerunnermainwin.h \
	fx/fxlist.h \
	fx/fxaudioitem.h \
	tool/toolclasses.h \
	tool/prefvarset.h \
	tool/prefvar.h \
	tool/dbquery.h \
	tool/database.h \
	system/log.h \
	main/project.h \
	main/usersettings.h \
	config.h \
	gui/sequencecontrolwidget.h \
	thirdparty/widget/qsynthdialpeppinostyle.h \
	thirdparty/widget/qsynthknob.h \
	thirdparty/widget/qsynthmeter.h \
	thirdparty/widget/qsynthdialclassicstyle.h \
	gui/audioslotwidget.h \
	gui/audiocontrolwidget.h \
	system/commandsystem.h \
	system/audioiodevice.h \
	system/audioformat.h \
	gui/ptablewidget.h

FORMS    += \
	gui/fxlistwidget.ui \
	gui/stagerunnermainwin.ui \
	gui/sequencecontrolwidget.ui \
	gui/audioslotwidget.ui \
	gui/audiocontrolwidget.ui

RESOURCES += \
	gfx_ressource.qrc


