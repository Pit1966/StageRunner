#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include "audioformat.h"
#include "commandsystem.h"

#include <QIODevice>
#include <QTime>

#ifdef IS_QT5
#include <QAudioDecoder>
#endif // IS_QT5

class QAudioFormat;

class AudioIODevice : public QIODevice
{
	Q_OBJECT
public:
	AudioIODevice(AudioFormat format, QObject *parent = 0);
	~AudioIODevice();

	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	qint64 bytesAvailable() const;

	bool setSourceFilename(const QString & filename);
	void examineQAudioFormat(AudioFormat & form);
	AudioFormat & audioFormat() const {return *audio_format;}

private:
	void calc_vu_level(const char *data, int size);


private:
	QString current_filename;
	QTime run_time;
	AUDIO::AudioErrorType audio_error;

	AudioFormat *audio_format;
#ifdef IS_QT5
	QAudioDecoder *audio_decoder;
#endif
	QByteArray *audio_buffer;
	qint64 bytes_avail;
	qint64 bytes_read;
	int audio_buffer_count;
	bool decoding_finished_f;
	double frame_energy_peak;
	double sample_peak;

	int loop_target;							// How many loops of the sound to play
	int loop_count;								// Amount of loops the sound has been played already

public:
	inline bool isDecodingFinished() {return decoding_finished_f;}
	inline int currentLoop() const {return loop_count;}
	qint64 currentPlayPosMs() const;
	qint64 currentPlayPosUs() const;
	bool seekPlayPosMs(qint64 posMs);
	inline AUDIO::AudioErrorType audioError() const {return audio_error;}

public slots:
	void start(int loops = 1);
	void stop();

private slots:
	void process_decoder_buffer();
	void on_decoding_finished();
	void if_audio_duration_changed(qint64 duration);
#ifdef IS_QT5
	void if_error_occurred(QAudioDecoder::Error error);
#endif

signals:
	void readReady();
	void vuLevelChanged(int left, int right);
	void audioDurationDetected(qint64 ms);

};

#endif // AUDIOIODEVICE_H
