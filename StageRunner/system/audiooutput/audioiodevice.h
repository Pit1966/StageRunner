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

#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include "audioformat.h"
#include "commandsystem.h"
#include "psmovingaverage.h"
#include "frqspectrum.h"
#include "fftrealfixlenwrapper.h"

#include <QIODevice>
#include <QElapsedTimer>
#include <QAudioDecoder>
#include <QMutex>

#ifdef IS_QT5
#	include <QAudioDeviceInfo>
#endif


class AudioFormat;

class AudioDecoder : public QAudioDecoder
{
	Q_OBJECT
public:
	enum Error
	{
		NoError,
		ResourceError,
		FormatError,
		AccessDeniedError,
		NotSupportedError
	};
	Q_ENUM(Error)

	bool isDecoding() const;
	void setSourceFilename(const QString &filename);
	QString sourceFilename() const;

};

class AudioIODevice : public QIODevice
{
	Q_OBJECT
public:
	AudioIODevice(AudioFormat format, QObject *parent = nullptr);
	~AudioIODevice();

	bool open(OpenMode mode) override;
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;
	qint64 bytesAvailable() const override;

	bool setSourceFilename(const QString & filename);
#ifndef IS_QT6
	void examineAudioFormat(AudioFormat & form);
#endif
	AudioFormat & audioFormat() const {return *audio_format;}

	void setPanning(int pan, int maxPan);

private:
	QString current_filename;
	QElapsedTimer run_time;
	QMutex m_mutex;
	AUDIO::AudioErrorType audio_error;
	AudioFormat *audio_format;
	AudioDecoder *audio_decoder;
	QByteArray *audio_buffer;
	qint64 bytes_avail;
	qint64 bytes_read;
	int audio_buffer_count;
	bool decoding_finished_f;
	double frame_energy_peak;
	double sample_peak;

	int m_currentPlaybackSamplerate;
	int m_currentPan;							// a value of 0 means, panning is disabled
	int m_maxPan;								// usualy 200
	qreal m_panVolLeft;							// for panning volume left channel (0-1)
	qreal m_panVolRight;						// for panning volume right channel (0-1)

	/// @todo move this to AudioPlayer
	int loop_target;							// How many loops of the sound to play
	int loop_count;								// Amount of loops the sound has been played already

	PsMovingAverage<qreal> *m_leftAvg;
	PsMovingAverage<qreal> *m_rightAvg;
	FFTRealFixLenWrapper m_leftFFT;
	FFTRealFixLenWrapper m_rightFFT;
	FrqSpectrum m_leftSpectrum;
	FrqSpectrum m_rightSpectrum;
	bool m_fftEnabled;
	QString m_lastErrorText;					// last error that occured. At the moment this is only audio service missing at startup

public:
	inline bool isDecodingFinished() const {return decoding_finished_f;}
	bool isDecoding() const;
	inline int currentLoop() const {return loop_count;}
	inline const QString &lastErrorText() const {return m_lastErrorText;}
	qint64 currentPlayPosMs() const;
	qint64 currentPlayPosUs() const;
	bool seekPlayPosMs(qint64 posMs);
	inline AUDIO::AudioErrorType audioError() const {return audio_error;}
	inline void setFFTEnabled(bool state) {m_fftEnabled = state;}
	inline bool isFFTEnabled() const {return m_fftEnabled;}

	void setLoopCount(int loops);

#ifndef IS_QT6
	static 	QAudioDeviceInfo getAudioDeviceInfo(const QString &devName, bool *found = nullptr);
#endif

protected:
	void calcPanning(char *data, int size, const AudioFormat &audioFormat);

public slots:
	void start(int loops = 1);
	void stop();
	void calcVuLevel(const char *data, int size, const AudioFormat &audioFormat);

private slots:
	void process_decoder_buffer();
	void on_decoding_finished();
	void if_audio_duration_changed(qint64 duration);
	void if_error_occurred(QAudioDecoder::Error error);

signals:
	void readReady();
	void vuLevelChanged(qreal left, qreal right);
	void frqSpectrumChanged(FrqSpectrum *spec);
	void audioDurationDetected(qint64 ms);
	void rawDataProcessed(const char *data, int size, const AudioFormat &audioFormat);

};

#endif // AUDIOIODEVICE_H
