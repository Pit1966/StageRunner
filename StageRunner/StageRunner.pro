include(configvars.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

SUBDIRS += main
SUBDIRS += plugins


# Translations
translations.files += stagerunner_de_DE.qm
translations.path   = $$INSTALLROOT/$$TRANSLATIONDIR
INSTALLS           += translations

