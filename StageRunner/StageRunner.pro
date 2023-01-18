TEMPLATE = subdirs

SUBDIRS += main
SUBDIRS += plugins

# Translations
translations.files += stagerunner_de_DE.qm
translations.path   = $$INSTALLROOT/$$TRANSLATIONDIR
INSTALLS           += translations


