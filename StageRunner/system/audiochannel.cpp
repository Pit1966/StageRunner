#include "audiochannel.h"

#include "fx/fxaudioitem.h"
#include "system/log.h"
#include "config.h"

#include <QTime>
#include <QApplication>
#ifdef IS_QT5
#include <QMediaContent>
#endif

AudioSlot::AudioSlot()
	: QObject()
{

	run_status = AUDIO_IDLE;
	slotNumber = -1;
	probe_init_f = false;
	frame_energy_peak = 0;
	sample_peak = 0;

#ifndef IS_QT5
	media_obj = new Phonon::MediaObject();
	audio_out = new Phonon::AudioOutput(Phonon::MusicCategory);
	path = Phonon::createPath(media_obj, audio_out);

	connect(media_obj,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(setPhononAudioState(Phonon::State,Phonon::State)));
	connect(media_obj,SIGNAL(finished()),this,SLOT(setPhononFinished()));
#else
	media_player = new QMediaPlayer;
	media_probe = new QAudioProbe;
	connect(media_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(setQtAudioState(QMediaPlayer::State)));
	connect(media_probe,SIGNAL(audioBufferProbed(QAudioBuffer)),this,SLOT(monitorQtAudioStream(QAudioBuffer)));
#endif

}

AudioSlot::~AudioSlot()
{
#ifndef IS_QT5
	delete audio_out;
	delete media_obj;
#else
	delete media_probe;
	delete media_player;
#endif
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa)
{
	LOGTEXT(tr("Start FxAudio: %1 in audio slot %2")
			.arg(fxa->displayName()).arg(slotNumber+1));

	sample_peak = 0;
	frame_energy_peak = 0;

	run_status = AUDIO_INIT;
	run_time.start();

	// Emit Control Msg to send Status of Volume
	AudioCtrlMsg msg(slotNumber);
	msg.volume = fxa->initialVolume;
	emit audioCtrlMsgEmitted(msg);

#ifndef IS_QT5
	Phonon::MediaSource source(fxa->filePath());
	media_obj->setCurrentSource(source);

	media_obj->play();
#else

	media_player->setMedia(QUrl::fromLocalFile(fxa->filePath()));
	media_player->play();
	media_player->setVolume(fxa->initialVolume);
	media_probe->setSource(media_player);
	// qDebug() << "start:" << media_player->mediaStatus() << media_player->errorString();
	// qDebug() << media_player->media().canonicalUrl();
#endif

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			DEBUGTEXT("FxAudio started: '%s'' (%dms delayed)",fxa->displayName().toLatin1().data(),run_time.elapsed());
			ok = true;
			run_time.start();
		}
		else if (run_status == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->displayName().toLatin1().data());
			break;
		}

	}

#ifndef IS_QT5
	if (media_obj->state() == Phonon::StoppedState) {
		DEBUGERROR("FxAudio '%s'' did not start after %dms -> Audio Slot %d set to Error/Idle state"
				   ,fxa->displayName().toLatin1().data(),run_time.elapsed(),slotNumber+1);
		run_status = AUDIO_ERROR;
	}
#endif

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	emit vuLevelChanged(slotNumber,0,0);

	if (run_status != AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
#ifndef IS_QT5
		media_obj->stop();
		run_status = AUDIO_IDLE;
#else
		media_player->stop();
#endif

		return true;
	}

	return false;
}

void AudioSlot::setVolume(qint32 vol)
{
#ifndef IS_QT5
	audioSink()->setVolume((qreal)vol/100);
#else
	media_player->setVolume(vol);
#endif

	LOGTEXT(tr("Change Volume for Audio Fx in slot %1: %2").arg(slotNumber+1).arg(vol));
}

#ifndef IS_QT5
void AudioSlot::setPhononAudioState(Phonon::State newstate, Phonon::State oldstate)
{
	// Q_UNUSED(oldstate);
	// AudioStatus oldstat = run_status;

	switch (newstate) {
	case Phonon::LoadingState:
		DEBUGTEXT("AudioSlot:%d Loading",slotNumber+1);
		run_status = AUDIO_INIT;
		break;
	case Phonon::PlayingState:
		DEBUGTEXT("AudioSlot:%d Playing",slotNumber+1);
		run_status = AUDIO_RUNNING;
		probe_init_f = true;
		break;
	case Phonon::StoppedState:
		DEBUGTEXT("AudioSlot:%d Stopped",slotNumber+1);
		if (run_status != AUDIO_INIT) {
			run_status = AUDIO_IDLE;
		}
		break;
	case Phonon::PausedState:
		DEBUGTEXT("AudioSlot:%d Paused",slotNumber+1);
		if (run_status != AUDIO_INIT) {
			run_status = AUDIO_IDLE;
		}
		break;
	case Phonon::ErrorState:
		DEBUGTEXT("AudioSlot:%d Error",slotNumber+1);
		run_status = AUDIO_ERROR;
		break;
	case Phonon::BufferingState:
		DEBUGTEXT("AudioSlot:%d Buffering",slotNumber+1);
		run_status = AUDIO_INIT;
		break;

	default:
		break;
	}

	if (oldstate != newstate) {
		qDebug("emit status: %d %d",run_status, slotNumber);
		AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status);
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::setPhononFinished()
{
	DEBUGTEXT("AudioSlot:%d finished",slotNumber+1);
}
#else
void AudioSlot::setQtAudioState(QMediaPlayer::State state)
{
	AudioStatus current_state = run_status;

	switch (state) {
	case QMediaPlayer::StoppedState:
		DEBUGTEXT("AudioSlot:%d Stopped",slotNumber+1);
		// qDebug() << "max peak:" << sample_peak << " max frame energy:" << frame_energy_peak;
		run_status = AUDIO_IDLE;
		emit vuLevelChanged(slotNumber,0,0);
		break;
	case QMediaPlayer::PlayingState:
		DEBUGTEXT("AudioSlot:%d Playing",slotNumber+1);
		run_status = AUDIO_RUNNING;
		probe_init_f = true;
		break;
	case QMediaPlayer::PausedState:
		DEBUGTEXT("AudioSlot:%d Paused",slotNumber+1);
		run_status = AUDIO_IDLE;
		break;
	}

	if (current_state != run_status) {
		// qDebug("emit status: %d %d",run_status, slotNumber);
		AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status);
		emit audioCtrlMsgEmitted(msg);
	}

}

void AudioSlot::monitorQtAudioStream(QAudioBuffer audiobuf)
{
	QAudioFormat form = audiobuf.format();
	int samplesize = form.sampleSize();

	int samples = audiobuf.sampleCount();
	int frames = audiobuf.frameCount();
	if (frames <= 0) return;

	int channels = form.channelCount();
	int samplerate = form.sampleRate();
	QString codec = form.codec();

	if (probe_init_f) {
		qDebug("samples:%d/%d (channels:%d) samplerate:%d - codec:%s",samples,frames,channels,samplerate,codec.toLatin1().data());
		qDebug() << "SampleType" << form.sampleType() << "Samplesize" << form.sampleSize();
	}

	qint64 left = 0;
	qint64 right = 0;

	switch (form.sampleType()) {
	case QAudioFormat::SignedInt:
	case QAudioFormat::UnSignedInt:
		{
			qint64 energy[4] = {0,0,0,0};
			switch (samplesize) {
			case 16:
				{
					const qint16 *dat = audiobuf.constData<qint16>();
					for (int chan = 0; chan < channels; chan++) {
						for (int frame = 0; frame<frames; frame++) {
							qint16 val = dat[frame*channels+chan];
							if (val > sample_peak) sample_peak = val;
							if (val > 0) {
								energy[chan] += val;
							} else {
								energy[chan] -= val;
							}

						}
					}
				}
				break;
			default:
				if (probe_init_f) {
					DEBUGERROR("Sampletype in audiostream not supported");
				}
				break;
			}
			left = energy[0];
			right = energy[1];
		}
		break;
	case QAudioFormat::Float:
		{
			double energy[4] = {0,0,0,0};
			switch (samplesize) {
			case 32:
				{
					const float *dat = audiobuf.constData<float>();
					for (int chan = 0; chan < channels; chan++) {
						for (int frame = 0; frame<frames; frame++) {
							float val = dat[frame*channels+chan];
							if (val > sample_peak) sample_peak = val;
							val *= 32768;
							if (val > 0) {
								energy[chan] += val;
							} else {
								energy[chan] -= val;
							}

						}
					}
				}
				break;
			default:
				if (probe_init_f) {
					DEBUGERROR("Sampletype in audiostream not supported");
				}
				break;
			}
			left = energy[0];
			right = energy[1];
		}
		break;
	case QAudioFormat::Unknown:
		if (probe_init_f) {
			DEBUGERROR("Sampletype in audiostream unknown");
		}
		break;
	}

//	qDebug("left:%lli right:%lli",left/frames,right/frames);
	if (left/frames > frame_energy_peak) frame_energy_peak = left/frames;

	if (run_status == AUDIO_RUNNING) emit vuLevelChanged(slotNumber, left/frames, right/frames);
	probe_init_f = false;
}

#endif
