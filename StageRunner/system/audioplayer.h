#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QAudioFormat>

#include "commandsystem.h"
#include "fftrealfixlenwrapper.h"
#include "frqspectrum.h"


class AudioSlot;

template <class T> class PsMovingAverage;

class AudioPlayer : public QObject
{
	Q_OBJECT

protected:
	AudioSlot & m_parentAudioSlot;
	int m_loopTarget;
	int m_loopCnt;
	int m_currentVolume;
	CtrlCmd m_currentCtrlCmd;
	AUDIO::AudioErrorType m_audioError;
	QString m_audioErrorString;
	QString m_mediaPath;

	// run time
	int m_currentPlaybackSamplerate;

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

public:
	AudioPlayer(AudioSlot &audioChannel);
	virtual AUDIO::AudioOutputType outputType() const = 0;

	inline AUDIO::AudioErrorType audioError() const {return m_audioError;}
	inline const QString & audioErrorString() const {return m_audioErrorString;}
	inline CtrlCmd currentAudioCmd() const {return m_currentCtrlCmd;}
	inline int currentLoop() const {return m_loopCnt;}

	virtual bool setSourceFilename(const QString &path);
	virtual void start(int loops) = 0;
	virtual void stop() = 0;
	virtual void pause(bool state) = 0;
	virtual qint64 currentPlayPosUs() const = 0;
	virtual qint64 currentPlayPosMs() const = 0;
	virtual bool seekPlayPosMs(qint64 posMs) = 0;
	virtual void setVolume(int vol, int maxvol) = 0;
	virtual int volume() const = 0;
	virtual AUDIO::AudioStatus state() const = 0;

	virtual void setAudioBufferSize(int bytes) = 0;
	virtual int audioBufferSize() const = 0;

	inline void setFFTEnabled(bool state) {m_fftEnabled = state;}
	inline bool isFFTEnabled() const {return m_fftEnabled;}

//protected:
	// some helper fucntions, that can be used by any backend implementation
	void calcVuLevel(const char *data, int size, const QAudioFormat &audioFormat);


signals:
	void statusChanged(AUDIO::AudioStatus status);
	void mediaDurationChanged(qint64 ms);
	void vuLevelChanged(qreal left, qreal right);
	void frqSpectrumChanged(FrqSpectrum *spec);

};

#endif // AUDIOPLAYER_H
