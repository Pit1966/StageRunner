TEMPLATE = subdirs

macx:SUBDIRS       += macx
unix:!macx:SUBDIRS += alsa
win32:SUBDIRS      += win32

TRANSLATIONS += MIDI_de_DE.ts
TRANSLATIONS += MIDI_es_ES.ts
TRANSLATIONS += MIDI_fi_FI.ts
TRANSLATIONS += MIDI_fr_FR.ts
TRANSLATIONS += MIDI_it_IT.ts