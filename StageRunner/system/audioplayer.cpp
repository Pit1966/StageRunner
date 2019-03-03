#include "audioplayer.h"
#include "log.h"
#include "audioslot.h"
#include "psmovingaverage.h"

/// @todo remove me
#include "audioiodevice.h"

using namespace AUDIO;

AudioPlayer::AudioPlayer(AudioSlot &audioChannel)
	: QObject()
	, m_parentAudioSlot(audioChannel)
	, m_loopTarget(1)
	, m_loopCnt(1)
	, m_currentVolume(100)
	, m_currentCtrlCmd(CMD_NONE)
	, m_audioError(AUDIO_ERR_NONE)
	, m_currentPlaybackSamplerate(0)
	, frame_energy_peak(0)
	, sample_peak(0)
	, m_leftAvg(new PsMovingAverage<qreal>(4))
	, m_rightAvg(new PsMovingAverage<qreal>(4))
	, m_fftEnabled(false)
{
}

bool AudioPlayer::setSourceFilename(const QString &path)
{
	if (path != m_mediaPath) {
		m_mediaPath = path;
	}
	return true;
}

void AudioPlayer::calcVuLevel(const char *data, int size, const QAudioFormat &audioFormat)
{
	//	static QTime checktime;
	//	qDebug("calcLast %dms",checktime.elapsed());
	//	checktime.start();

	qreal left = 0;
	qreal right = 0;
	qreal energy[4] = {0,0,0,0};
	qreal peak[4] = {0,0,0,0};

	int channels = audioFormat.channelCount();
	qint64 frames = size / channels;

	if (frames == 0) return;
	// qDebug() << "calcVuLevel" << size << QThread::currentThread()->objectName();

	switch (audioFormat.sampleType()) {
	case QAudioFormat::SignedInt:
	case QAudioFormat::UnSignedInt:
		switch (audioFormat.sampleSize()) {
		case 16:
			{
				const qint16 *dat = reinterpret_cast<const qint16*>(data);
				frames /= 2;

				for (int chan = 0; chan < channels; chan++) {
					for (int frame = 0; frame<frames; frame++) {
						const qint16 val = dat[frame*channels+chan];
						const qreal valF = AudioIODevice::pcm16ToReal(val,audioFormat);
						if (valF > sample_peak)
							sample_peak = valF;
						if (valF > peak[chan])
							peak[chan] = valF;

						energy[chan] += valF * valF;
						if (m_fftEnabled) {
							switch (chan) {
							case 0:
								m_leftFFT.appendToBuffer(valF);
								break;
							case 1:
								m_rightFFT.appendToBuffer(valF);
								break;
							}
						}
					}
				}
			}
			break;
		case 32:
			{
				const qint32 *dat = reinterpret_cast<const qint32*>(data);
				frames /= 4;

				for (int chan = 0; chan < channels; chan++) {
					for (int frame = 0; frame<frames; frame++) {
						const qint32 val = dat[frame*channels+chan];
						const qreal valF = AudioIODevice::pcm32ToReal(val,audioFormat);
						if (valF > sample_peak)
							sample_peak = valF;
						if (valF > peak[chan])
							peak[chan] = valF;

						energy[chan] += valF * valF;
						if (m_fftEnabled) {
							switch (chan) {
							case 0:
								m_leftFFT.appendToBuffer(valF);
								break;
							case 1:
								m_rightFFT.appendToBuffer(valF);
								break;
							}
						}
					}
				}
			}
			break;
		default:
			DEBUGERROR("Sampletype in audiostream not supported");
			break;
		}
		left = sqrt(energy[0] / frames);
		right = sqrt(energy[1] / frames);
		break;

	case QAudioFormat::Float:
		{
			switch (audioFormat.sampleSize()) {
			case 32:
				{
					const float *dat = reinterpret_cast<const float*>(data);
					frames /= 4;

					for (int chan = 0; chan < channels; chan++) {
						for (int frame = 0; frame<frames; frame++) {
							const float val = dat[frame*channels+chan];
							// const qreal valF = AudioIODevice::realToRealNorm(val,audioFormat);
							const float valF = val;

							if (valF > sample_peak)
								sample_peak = valF;
							if (valF > peak[chan])
								peak[chan] = valF;

							energy[chan] += valF * valF;
							if (m_fftEnabled) {
								switch (chan) {
								case 0:
									m_leftFFT.appendToBuffer(valF);
									break;
								case 1:
									m_rightFFT.appendToBuffer(valF);
									break;
								}
							}
						}
					}
				}
				break;
			default:
				DEBUGERROR("Sampletype in audiostream not supported");
				break;
			}
			left = sqrt(energy[0] / frames);
			right = sqrt(energy[1] / frames);
		}
		break;
	case QAudioFormat::Unknown:
		DEBUGERROR("Sampletype in audiostream unknown");
		break;
	}

	// qDebug("left:%f right:%f",left,right);
	if (left/frames > frame_energy_peak) frame_energy_peak = left/frames;

	m_leftAvg->append(left);
	m_rightAvg->append(right);

	// double vl = m_leftAvg->currentAvg();
	// double vr = m_rightAvg->currentAvg();

	emit vuLevelChanged(left * 1.8,right * 1.8);

	if (m_fftEnabled) {
		while (m_leftFFT.bufferFilled()) {
			m_leftFFT.calculateFFT();

			m_leftSpectrum.fillSpectrumFFTQVectorArray(m_leftFFT.fftComplexArray());
			if (audioFormat.sampleRate() > m_currentPlaybackSamplerate) {
				m_currentPlaybackSamplerate = audioFormat.sampleRate();
				m_leftSpectrum.setMaxFrequency(double(m_currentPlaybackSamplerate) / 2);
			}
			emit frqSpectrumChanged(&m_leftSpectrum);
		}
	} else {
		m_leftFFT.clearBuffer();
	}
	if (m_fftEnabled) {
		while (m_rightFFT.bufferFilled()) {
			m_rightFFT.calculateFFT();

			m_rightSpectrum.fillSpectrumFFTQVectorArray(m_rightFFT.fftComplexArray());
			if (audioFormat.sampleRate() > m_currentPlaybackSamplerate) {
				m_currentPlaybackSamplerate = audioFormat.sampleRate();
				m_leftSpectrum.setMaxFrequency(double(m_currentPlaybackSamplerate) / 2);
			}
			emit frqSpectrumChanged(&m_rightSpectrum);
		}
	} else {
		m_rightFFT.clearBuffer();
	}
}
