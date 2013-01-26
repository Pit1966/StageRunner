#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include "audioformat.h"

#include <QIODevice>
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


public slots:
	void start();
	void stop();

private slots:
	void process_decoder_buffer();
	void on_decoding_finished();

signals:
	void readReady();
	void vuLevelChanged(int left, int right);

};

#endif // AUDIOIODEVICE_H
