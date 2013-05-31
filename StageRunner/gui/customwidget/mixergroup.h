#ifndef MIXERGROUP_H
#define MIXERGROUP_H

#include <QWidget>
#include <QList>

class MixerChannel;

class MixerGroup : public QWidget
{
	Q_OBJECT
public:
	Q_PROPERTY(bool multiSelectEnabled READ isMultiSelectEnabled WRITE setMultiSelectEnabled)

private:
	QList<MixerChannel*>mixerlist;			///< List with pointers to MixerChannel instances
	QList<MixerChannel*>selected_mixer;
	int default_min;						///< Default minimum value used for new Mixer
	int default_max;						///< Default maximum value user for new Mixer

	bool prop_multiselect_f;				///< If true, multiple mixer can be selected at the same time

public:
	explicit MixerGroup(QWidget *parent = 0);
	void setMixerCount(int number);
	void clear();
	MixerChannel * appendMixer();
	bool removeMixer(MixerChannel *mixer, bool renumberIds = false);
	void setIdsToMixerListIndex();
	void setRange(int min, int max);
	inline bool isMultiSelectEnabled() const {return prop_multiselect_f;}
	void setMultiSelectEnabled(bool state);
	MixerChannel *findMixerAtPos(QPoint pos);
	MixerChannel *getMixerById(int id);
	QList<MixerChannel*> & selectedMixer() {return selected_mixer;}
	void unselectAllMixers();

protected:
	void resizeEvent(QResizeEvent *event);

public slots:
	void notifyChangedDmxUniverse(int universe, const QByteArray & dmxValues);
	void setRefSliderColorIndex(int colidx);

private slots:
	void on_mixer_moved(int val, int id);
	void on_mixer_selected(bool state, int id);

signals:
	void mixerMoved(int val, int id);
	void mixerSelected(bool state, int id);
};

#endif // MIXERGROUP_H
