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

#include "audioformat.h"

AudioFormat::AudioFormat() :
	QAudioFormat()
{
}

//AudioFormat::AudioFormat(const AudioFormat &other)
//	: QAudioFormat()
//{
//	*this = other;
//}

AudioFormat::AudioFormat(const QAudioFormat &other)
	: QAudioFormat(other)
{
}

AudioFormat AudioFormat::defaultFormat()
{
	AudioFormat format;
	format.setSampleRate(48000);
	format.setChannelCount(2);
	format.setSampleType(QAudioFormat::SignedInt);
	format.setSampleSize(16);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");

	return format;
}
