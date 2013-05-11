#ifndef MIXERGROUP_H
#define MIXERGROUP_H

#include <QWidget>
#include <QList>

class MixerChannel;

class MixerGroup : public QWidget
{
	Q_OBJECT
private:
	QList<MixerChannel*>mixerlist;			///< List with pointers to MixerChannel instances
	int default_min;						///< Default minimum value used for new Mixer
	int default_max;						///< Default maximum value user for new Mixer


public:
	explicit MixerGroup(QWidget *parent = 0);
	void setMixerCount(int number);
	void clear();
	MixerChannel * appendMixer();
	void setRange(int min, int max);

signals:
	void mixerMoved(int val, int id);

public slots:
	void notifyChangedDmxUniverse(int universe, const QByteArray & dmxValues);
	void setRefSliderColorIndex(int colidx);

private slots:
	void on_mixer_moved(int val, int id);

};

#endif // MIXERGROUP_H
