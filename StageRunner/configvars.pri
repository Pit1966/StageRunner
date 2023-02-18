#############################################################################
# Application name & version
#############################################################################

APPNAME    = StageRunner
APPVERSION = V0.9.7.5
APP_MILESTONE = Mausefalle+5
APP_PRODUCER = 2023 Stonechip Entertainment
APP_ORG_STRING = Stonechip
APP_ORG_DOMAIN = stonechip.de

#############################################################################
# Compiler & linker configuration
#############################################################################


# QMAKE_LFLAGS += /INCREMENTAL:NO

# Treat all compiler warnings as errors
# QMAKE_CXXFLAGS += -Werror
CONFIG         += warn_on

# Build everything in the order specified in .pro files
# CONFIG         += ordered
CONFIG         -= release # Enable this when making a release
CONFIG         += debug   # Disable this when making a release

# Disable these if you don't want to see SVN revision in the About Box
# unix:REVISION = $$system(svn info | grep "Revision" | sed 's/Revision://')
# unix:APPVERSION = $$APPVERSION-r$$REVISION


#macx:CONFIG   += x86 ppc  # Build universal binaries (Leopard only)
#macx:CONFIG    -= app_bundle # Let StageRunner construct the .app bundle

# Produce build targets to the source directory
win32:DESTDIR  = ./

# Don't whine about some imports
# win32:QMAKE_LFLAGS += -enable-auto-import

#message(BUILDDIR: $$OUT_PWD)
#INCLUDEPATH += $$OUT_PWD

#############################################################################
# Installation paths
#############################################################################

# Install root
win32:INSTALLROOT       = $$(SystemDrive)/StageRunner
macx:INSTALLROOT        = ~/StageRunner.app/Contents
unix:!macx:INSTALLROOT += /usr

# Binaries
win32:BINDIR      =
unix:!macx:BINDIR = bin
macx:BINDIR       = MacOS

# Libraries
win32:LIBSDIR      =
unix:!macx:LIBSDIR = lib
macx:LIBSDIR       = Frameworks

# Data
win32:DATADIR      =
unix:!macx:DATADIR = share/stagerunner
macx:DATADIR       = Resources

# Documentation
win32:DOCSDIR      = Documents
unix:!macx:DOCSDIR = $$DATADIR/documents
macx:DOCSDIR       = $$DATADIR/Documents

# Plugins
win32:PLUGINDIR      = Plugins
unix:!macx:PLUGINDIR = share/stagerunner/plugins
macx:PLUGINDIR       = PlugIns/stagerunner

# Translations
win32:TRANSLATIONDIR      =
unix:!macx:TRANSLATIONDIR = $$DATADIR/translations
macx:TRANSLATIONDIR       = $$DATADIR/Translations

# Log files
win32:LOF_FILE_PATH			= c:/StageRunner.log
unix:!macx:LOG_FILE_PATH	= /tmp/StageRunner.log
macx:LOG_FILE_PATH			= /tmp/StageRunner.log


#############################################################################
# configrev.h generation
#############################################################################

#CONFIGFILE = $$_PRO_FILE_PWD_/configrev.h
CONFIGFILE = $$OUT_PWD/configrev.h
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
    conf.commands += echo \"$$LITERAL_HASH define APP_MILESTONE \\\"$$APP_MILESTONE\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_PRODUCER \\\"$$APP_PRODUCER\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_ORG_STRING \\\"$$APP_ORG_STRING\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APP_ORG_DOMAIN \\\"$$APP_ORG_DOMAIN\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_CONFIG_PATH \\\"$$APP_ORG_DOMAIN\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define DOCSDIR \\\"$$DOCSDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define PLUGINDIR \\\"$$PLUGINDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define TRANSLATIONDIR \\\"$$TRANSLATIONDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define LOG_FILE_PATH \\\"$$LOG_FILE_PATH\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define QSETFORMAT QSettings::IniFormat,QSettings::UserScope,APP_ORG_DOMAIN\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH endif\" >> $$CONFIGFILE
}
unix:!macx {
    conf.commands += echo \"$$LITERAL_HASH ifndef CONFIGREV_H\" > $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define CONFIGREV_H\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APPNAME \\\"$$APPNAME\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APPVERSION \\\"$$APPVERSION\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_MILESTONE \\\"$$APP_MILESTONE\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_PRODUCER \\\"$$APP_PRODUCER\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_ORG_STRING \\\"$$APP_ORG_STRING\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APP_ORG_DOMAIN \\\"$$APP_ORG_DOMAIN\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_CONFIG_PATH \\\"$$APP_ORG_STRING\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define DOCSDIR \\\"$$INSTALLROOT/$$DOCSDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define PLUGINDIR \\\"$$INSTALLROOT/$$PLUGINDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define TRANSLATIONDIR \\\"$$INSTALLROOT/$$TRANSLATIONDIR\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define LOG_FILE_PATH \\\"$$LOG_FILE_PATH\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define QSETFORMAT QSettings::NativeFormat,QSettings::UserScope,APP_ORG_STRING\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH endif\" >> $$CONFIGFILE
}
win32 {
    conf.commands += echo \"$$LITERAL_HASH ifndef CONFIGREV_H\" > $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define CONFIGREV_H\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APPNAME \\\"$$APPNAME\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APPVERSION \\\"$$APPVERSION\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_MILESTONE \\\"$$APP_MILESTONE\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_PRODUCER \\\"$$APP_PRODUCER\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_ORG_STRING \\\"$$APP_ORG_STRING\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define APP_ORG_DOMAIN \\\"$$APP_ORG_DOMAIN\\\"\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define APP_CONFIG_PATH \\\"$$APP_ORG_STRING\\\"\" >> $$CONFIGFILE &&
    conf.commands += @echo $$LITERAL_HASH define DOCSDIR \"$$DOCSDIR\" >> $$CONFIGFILE &&
    conf.commands += @echo $$LITERAL_HASH define PLUGINDIR \"$$PLUGINDIR\" >> $$CONFIGFILE &&
    conf.commands += @echo $$LITERAL_HASH define TRANSLATIONDIR \"$$TRANSLATIONDIR\" >> $$CONFIGFILE &&
    conf.commands += echo \"$$LITERAL_HASH define LOG_FILE_PATH \\\"$$LOG_FILE_PATH\\\"\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH define QSETFORMAT QSettings::IniFormat,QSettings::UserScope,APP_ORG_DOMAIN\" >> $$CONFIGFILE &&
	conf.commands += echo \"$$LITERAL_HASH endif\" >> $$CONFIGFILE
}




