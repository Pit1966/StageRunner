#############################################################################
# Application name & version
#############################################################################

APPNAME    = StageRunner
APPVERSION = 0.1.0

#############################################################################
# Compiler & linker configuration
#############################################################################

# Treat all compiler warnings as errors
QMAKE_CXXFLAGS += -Werror
CONFIG         += warn_on

# Build everything in the order specified in .pro files
# CONFIG         += ordered
CONFIG         -= release # Enable this when making a release
CONFIG         += debug   # Disable this when making a release

# Disable these if you don't want to see SVN revision in the About Box
unix:REVISION = $$system(svn info | grep "Revision" | sed 's/Revision://')
unix:APPVERSION = $$APPVERSION-r$$REVISION


#macx:CONFIG   += x86 ppc  # Build universal binaries (Leopard only)
macx:CONFIG    -= app_bundle # Let StageRunner construct the .app bundle

# Produce build targets to the source directory
win32:DESTDIR  = ./

# Don't whine about some imports
win32:QMAKE_LFLAGS += -enable-auto-import


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
unix:!macx:PLUGINDIR = $$LIBSDIR/qt4/plugins/stagerunner
macx:PLUGINDIR       = Plugins

# Translations
win32:TRANSLATIONDIR      =
unix:!macx:TRANSLATIONDIR = $$DATADIR/translations
macx:TRANSLATIONDIR       = $$DATADIR/Translations

