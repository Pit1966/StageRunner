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

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

#include "commandsystem.h"
#include "fftrealfixlenwrapper.h"
#include "frqspectrum.h"


class AudioSlot;
class AudioFormat;

template <class T> class PsMovingAverage;

class AudioPlayer : public QObject
{
	Q_OBJECT

protected:
	AudioSlot & m_parentAudioSlot;
	int m_loopTarget;
	int m_loopCnt;
	int m_currentVolume;
	int m_currentPan;						///< value of 0 means, disabled

	int m_maxPan;							// usualy 200
	qreal m_panVolLeft;						// for panning volume left channel (0-1)
	qreal m_panVolRight;					// for panning volume right channel (0-1)

	CtrlCmd m_currentCtrlCmd;
	AUDIO::AudioErrorType m_audioError;
	QString m_audioErrorString;
	QString m_mediaPath;
	QString m_fxName;						///< name of FxAudioItem (this is not the filename of audio, but may be)

	// run time
	int m_currentPlaybackSamplerate;

	// debug and help
	int m_audioIdleEventCount;							///< amount of QAudioDevice::Idle events. Used to check if audio buffer ist to small

	// VU meter
	double frame_energy_peak;
	double sample_peak;

	// FFT
	PsMovingAverage<qreal> *m_leftAvg;
	PsMovingAverage<qreal> *m_rightAvg;
	FFTRealFixLenWrapper m_leftFFT;
	FFTRealFixLenWrapper m_rightFFT;
	FrqSpectrum m_leftSpectrum;
	FrqSpectrum m_rightSpectrum;
	bool m_fftEnabled;

	// delayed start of audio fx
	int m_startDelayedTimerId;

	// special
	bool m_activateSmallBufferWorkAround = false;

public:
	AudioPlayer(AudioSlot &audioChannel);
	virtual AUDIO::AudioOutputType outputType() const = 0;

	inline AUDIO::AudioErrorType audioError() const {return m_audioError;}
	inline const QString & audioErrorString() const {return m_audioErrorString;}
	inline CtrlCmd currentAudioCmd() const {return m_currentCtrlCmd;}
	inline int currentLoop() const {return m_loopCnt;}
	int currentAudioSlotNumber() const;
	bool setStartDelay(int ms);
	inline void setFxName(const QString &name) {m_fxName = name;}

	void activateSmallAudioBufferWorkaround(bool state) {m_activateSmallBufferWorkAround = state;}

	virtual bool setSourceFilename(const QString &path, const QString &fxName);
	virtual void start(int loops) = 0;
	virtual void stop() = 0;
	virtual void pause(bool state) = 0;
	virtual qint64 currentPlayPosUs() const = 0;
	virtual qint64 currentPlayPosMs() const = 0;
	virtual bool seekPlayPosMs(qint64 posMs) = 0;
	virtual void setVolume(int vol, int maxvol) = 0;
	virtual void setPanning(int pan, int maxpan) = 0;
	virtual int volume() const = 0;
	virtual AUDIO::AudioStatus state() const = 0;

	virtual void setAudioBufferSize(int bytes) = 0;
	virtual int audioBufferSize() const = 0;

	virtual void delayedStartEvent() {;}

	virtual void setFFTEnabled(bool state) {m_fftEnabled = state;}
	virtual bool isFFTEnabled() const {return m_fftEnabled;}

//protected:
	// some helper fucntions, that can be used by any backend implementation
	void calcPanning(char *data, int size, const AudioFormat &audioFormat);
	void calcVuLevel(const char *data, int size, const AudioFormat &audioFormat);

protected:
	void timerEvent(QTimerEvent *event);


signals:
	void statusChanged(AUDIO::AudioStatus status);
	void mediaDurationChanged(qint64 ms);
	void vuLevelChanged(qreal left, qreal right);
	void frqSpectrumChanged(FrqSpectrum *spec);

	// not used by all audio backends
	void audioIdleEventsChanged(int count);

};

#endif // AUDIOPLAYER_H
