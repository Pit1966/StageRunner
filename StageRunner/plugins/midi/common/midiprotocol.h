/*
  Q Light Controller
  midiprotocol.h

  Copyright (c) Heikki Junnila

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  Version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details. The license is
  in the file "COPYING".

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef MIDIPROTOCOL_H
#define MIDIPROTOCOL_H

/****************************************************************************
 * MIDI conversion functions
 ****************************************************************************/

namespace QLCMIDIProtocol
{
    /**
     * Convert MIDI message to QLC input data
     *
     * @param cmd MIDI command byte
     * @param data1 MIDI first data byte
     * @param data2 MIDI second data byte (unused for some commands)
     * @param midiChannel MIDI channel to expect data on (255 for all)
     * @param channel The input channel that is mapped from the given MIDI data
     * @param value The value for the input channel
     * @return true if the values were parsed successfully, otherwise false
     */
    bool midiToInput(uchar cmd, uchar data1, uchar data2, uchar midiChannel,
                     quint32* channel, uchar* value);

    /**
     * Convert MIDI system common message to QLC input data
     *
     * @param cmd MIDI system common command byte
     * @param data1 MIDI first data byte (unused for some commands)
     * @param data2 MIDI second data byte (unused for some commands)
     * @param channel The input channel that is mapped from the given MIDI data
     * @param value The value for the input channel
     * @return true if the values were parsed successfully, otherwise false
     */
    bool midiSysCommonToInput(uchar cmd, uchar data1, uchar data2,
                              quint32* channel, uchar* value);

    /**
     * Convert QLC feedback data to MIDI message
     *
     * @param channel The input channel that receives feedback data
     * @param value The channel's feedback value
     * @param MIDI channel to send data on (0-15)
     * @param cmd MIDI command byte
     * @param data1 MIDI first data byte
     * @param data2 MIDI second data byte
     * @param data2Valid true if $data2 contains data, otherwise false
     * @return true if the values were parsed successfully, otherwise false
     */
    bool feedbackToMidi(quint32 channel, uchar value, uchar midiChannel,
                        uchar* cmd, uchar* data1,
                        uchar* data2, bool* data2Valid);
}

/****************************************************************************
 * MIDI helper macros
 ****************************************************************************/
/** Extract the MIDI channel part from a MIDI message (0x*0 - 0x*F) */
#define MIDI_CH(x) (x & 0x0F)

/** Extract the MIDI command part from a MIDI message (0x8* - 0xE*) */
#define MIDI_CMD(x) (x & 0xF0)

/** Check, whether a byte contains a MIDI command */
#define MIDI_IS_CMD(x) ((x & 0x80) ? true : false)

/** Check, whether the message is a system common message (0xF*) */
#define MIDI_IS_SYSCOMMON(x) (((x & 0xF0) == 0xF0) ? true : false)

/** Convert MIDI value to DMX value and make 127 == 255 (because 2*127=254) */
#define MIDI2DMX(x) uchar((x == CHAR_MAX) ? UCHAR_MAX : x << 1)

/** Convert DMX value to MIDI value */
#define DMX2MIDI(x) uchar(x >> 1)

/****************************************************************************
 * MIDI channels
 ****************************************************************************/
#define MAX_MIDI_DMX_CHANNELS   128
#define MAX_MIDI_CHANNELS       16

/****************************************************************************
 * MIDI commands with a MIDI channel (0-16)
 ****************************************************************************/
#define MIDI_NOTE_OFF           0x80
#define MIDI_NOTE_ON            0x90
#define MIDI_NOTE_AFTERTOUCH    0xA0
#define MIDI_CONTROL_CHANGE     0xB0
#define MIDI_PROGRAM_CHANGE     0xC0
#define MIDI_CHANNEL_AFTERTOUCH 0xD0
#define MIDI_PITCH_WHEEL        0xE0

/****************************************************************************
 * MIDI system common commands (no MIDI channel)
 ****************************************************************************/
#define MIDI_SYSEX              0xF0
#define MIDI_SYSEX_EOX          0x7F
#define MIDI_TIME_CODE          0xF1
#define MIDI_SONG_POSITION      0xF2
#define MIDI_SONG_SELECT        0xF3
#define MIDI_BEATC_CLOCK        0xF8

/****************************************************************************
 * MIDI control/msg -> QLC input channel mappings
 ****************************************************************************/
// Most MIDI sliderboards use control change messages -> put them first
#define CHANNEL_OFFSET_CONTROL_CHANGE      0
#define CHANNEL_OFFSET_CONTROL_CHANGE_MAX  127

#define CHANNEL_OFFSET_NOTE                128
#define CHANNEL_OFFSET_NOTE_MAX            255

#define CHANNEL_OFFSET_NOTE_AFTERTOUCH     256
#define CHANNEL_OFFSET_NOTE_AFTERTOUCH_MAX 383

#define CHANNEL_OFFSET_CHANNEL_AFTERTOUCH  384
#define CHANNEL_OFFSET_PROGRAM_CHANGE      385
#define CHANNEL_OFFSET_PITCH_WHEEL         386
#define CHANNEL_OFFSET_MBC                 387

#endif
