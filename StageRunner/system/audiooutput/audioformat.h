//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

#include <QAudioFormat>

class AudioFormat : public QAudioFormat
{
public:
	enum SampleFormat {
		Unknown,
		Uint8,
		Int16,
		Int32,
		Float
	};

	AudioFormat();
//	AudioFormat(const AudioFormat &other);
	AudioFormat(const QAudioFormat &other);

	SampleFormat sampleFormat() const;			// compat function Qt6 / Qt5

	static qreal pcm16ToReal(qint16 pcm16);
	static qint16 realToPcm16(qreal real);
	static qreal pcm32ToReal(qint64 pcm32);
	static qint64 realToPcm32(qreal real);
	static AudioFormat defaultFormat();

};

#endif // AUDIOFORMAT_H
