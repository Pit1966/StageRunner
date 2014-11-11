#ifndef MIXERGROUP_H
#define MIXERGROUP_H

#include <QWidget>
#include <QList>
#include <QHBoxLayout>

class MixerChannel;

class MixerGroup : public QWidget
{
	Q_OBJECT
public:
	Q_PROPERTY(bool multiSelectEnabled READ isMultiSelectEnabled WRITE setMultiSelectEnabled)

private:
	QHBoxLayout *mixerlayout;
	QList<MixerChannel*>mixerlist;			///< List with pointers to MixerChannel instances
	QList<MixerChannel*>selected_mixer;
	int default_min;						///< Default minimum value used for new Mixer
	int default_max;						///< Default maximum value user for new Mixer

	bool m_propEnableMultiSelect;			///< If true, multiple mixer can be selected at the same time
	bool m_propEnableRangeSelect;			///< If true, multiple mixer can be selected in a range

	int temp_drag_start_move_idx;
	int temp_drag_move_idx;
	QWidget *temp_drag_widget;

public:
	explicit MixerGroup(QWidget *parent = 0);
	void setMixerCount(int number);
	void clear();
	MixerChannel * appendMixer();
	bool removeMixer(MixerChannel *mixer, bool renumberIds = false);
	void setIdsToMixerListIndex();
	void setRange(int min, int max);
	inline bool isMultiSelectEnabled() const {return m_propEnableMultiSelect;}
	void setMultiSelectEnabled(bool state);
	void setRangeSelectEnabled(bool state);
	MixerChannel *findMixerAtPos(QPoint pos);
	MixerChannel *getMixerById(int id);
	bool selectMixer(MixerChannel *mixer, int id, bool state);
	bool selectMixerRange(MixerChannel *fromMixer, MixerChannel *toMixer, bool state);
	QList<MixerChannel*> & selectedMixer() {return selected_mixer;}
	void unselectAllMixers();

protected:
	void resizeEvent(QResizeEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);

public slots:
	void notifyChangedDmxUniverse(int universe, const QByteArray & dmxValues);
	void setRefSliderColorIndex(int colidx);

private slots:
	void on_mixer_moved(int val, int id);
	void on_mixer_selected(bool state, int id);

signals:
	void mixerSliderMoved(int val, int id);
	void mixerSelected(bool state, int id);
	void mixerDraged(int fromIndex, int toIndex);

};

#endif // MIXERGROUP_H
