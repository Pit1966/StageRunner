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
#include <QTime>
#include <QAudioDeviceInfo>
#include <QAudioDecoder>

class QAudioFormat;

class AudioDecoder : public QAudioDecoder
{

};

class AudioIODevice : public QIODevice
{
	Q_OBJECT
public:
	AudioIODevice(AudioFormat format, QObject *parent = nullptr);
	~AudioIODevice();

	bool open(OpenMode mode) override;
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	qint64 bytesAvailable() const;

	bool setSourceFilename(const QString & filename);
	void examineQAudioFormat(AudioFormat & form);
	AudioFormat & audioFormat() const {return *audio_format;}

private:
	QString current_filename;
	QTime run_time;
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
	inline bool isDecodingFinished() {return decoding_finished_f;}
	inline int currentLoop() const {return loop_count;}
	inline const QString &lastErrorText() const {return m_lastErrorText;}
	qint64 currentPlayPosMs() const;
	qint64 currentPlayPosUs() const;
	bool seekPlayPosMs(qint64 posMs);
	inline AUDIO::AudioErrorType audioError() const {return audio_error;}
	inline qreal pcm16ToReal(qint16 pcm) { return qreal(pcm * 2) / ((1<<audio_format->sampleSize())-1);}
	inline qint16 realToPcm16(qreal real) { return real * ((1<<audio_format->sampleSize())-1) / 2;}
	inline void setFFTEnabled(bool state) {m_fftEnabled = state;}

	inline static qreal pcm16ToReal(qint16 pcm, const QAudioFormat &audio) {return qreal(pcm * 2) / ((1<<audio.sampleSize())-1);}
	inline static qint16 realToPcm16(qreal real, const QAudioFormat &audio) { return real * ((1<<audio.sampleSize())-1) / 2;}
	inline static qreal realToRealNorm(qreal real, const QAudioFormat &audio) {return real * 2 / ((1<<audio.sampleSize())-1);}
	inline static qreal realNormToReal(qreal realnorm, const QAudioFormat &audio) { return realnorm * ((1<<audio.sampleSize())-1) / 2;}

	inline static qreal pcm32ToReal(qint64 pcm, const QAudioFormat &audio) {return qreal(pcm * 2) / ((qint64(1)<<audio.sampleSize())-1);}

	void setLoopCount(int loops);

	static 	QAudioDeviceInfo getAudioDeviceInfo(const QString &devName, bool *found = nullptr);

public slots:
	void start(int loops = 1);
	void stop();
	void calcVuLevel(const char *data, int size, const QAudioFormat &audioFormat);

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
	void rawDataProcessed(const char *data, int size, const QAudioFormat &audioFormat);

};

#endif // AUDIOIODEVICE_H
