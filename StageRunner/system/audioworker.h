#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>

class AudioControl;

class AudioWorker : public QObject
{
	Q_OBJECT
public:
	volatile bool isValid	= false;

private:
	AudioControl *m_audioControl;

public:
	explicit AudioWorker(AudioControl *unitAudio, QObject *parent = nullptr);

public slots:
	void initMediaPlayerInstances();

signals:

};

#endif // AUDIOWORKER_H
