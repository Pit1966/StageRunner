//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include <QDebug>

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

AudioFormat::SampleFormat AudioFormat::sampleFormat() const
{
#ifdef IS_QT6
	return AudioFormat::SampleFormat(QAudioFormat::sampleFormat());
#else // QT5
	if (sampleSize() == 8) {
		return AudioFormat::Uint8;
	}
	else if (sampleSize() == 16) {
		if (sampleType() == QAudioFormat::SignedInt)  {
			return AudioFormat::Int16;
		}
		else {
			return AudioFormat::Unknown;
		}
	}
	else if (sampleSize() == 32) {
		if (sampleType() == QAudioFormat::Float) {
			return AudioFormat::Float;
		}
		else if (sampleType() == QAudioFormat::SignedInt)  {
			return AudioFormat::Int32;
		}
		else {
			return AudioFormat::Unknown;
		}
	}
	return AudioFormat::Unknown;
#endif
}

void AudioFormat::setSampleFormat(SampleFormat format)
{
#ifdef IS_QT6
	QAudioFormat::setSampleFormat(QAudioFormat::SampleFormat(format));
#else // QT5
	switch (format) {
	case AudioFormat::Uint8:
		setSampleSize(8);
		setSampleType(QAudioFormat::UnSignedInt);
		break;
	case AudioFormat::Int16:
		setSampleSize(16);
		setSampleType(QAudioFormat::SignedInt);
		break;
	case AudioFormat::Int32:
		setSampleSize(32);
		setSampleType(QAudioFormat::SignedInt);
		break;
	case AudioFormat::Float:
		setSampleSize(32);
		setSampleType(QAudioFormat::Float);
		break;
	default:
		qWarning() << "AudioFormat" << format << "not supported";
	}
#endif
}

/**
 * @brief Normalize a 16bit audio sample amplitude to 1.0f
 * @param pcm
 * @return normalized qreal [0.0 - 1.0]
 */
qreal AudioFormat::pcm16ToReal(qint16 pcm16)
{
	return qreal(pcm16 * 2) / 65535;
}

qint16 AudioFormat::realToPcm16(qreal real)
{
	return real * 65535 / 2;
}

/**
 * @brief Normalize a 32bit audio sample amplitude to 1.0f
 * @param pcm
 * @return normalized qreal [0.0 - 1.0]
 */
qreal AudioFormat::pcm32ToReal(qint64 pcm32)
{
	return qreal(pcm32 * 2) / ((1ll<<32) - 1);
}

qint64 AudioFormat::realToPcm32(qreal real)
{
	return real * ((1ll<<32) - 1) / 2;
}

AudioFormat AudioFormat::defaultFormat()
{
	AudioFormat format;
	format.setSampleRate(48000);
	format.setChannelCount(2);
#ifdef IS_QT6
	format.setSampleFormat(AudioFormat::Int16);
#else
	format.setSampleType(QAudioFormat::SignedInt);
	format.setSampleSize(16);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");
#endif

	return format;
}
