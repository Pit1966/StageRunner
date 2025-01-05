#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>

class AudioControl;
class FxAudioItem;
class Executer;


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
	bool acStartFxAudioStage2(FxAudioItem *fxa, Executer *exec, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);
	bool acStartFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);

signals:

};

#endif // AUDIOWORKER_H
