//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef MIXERGROUP_H
#define MIXERGROUP_H

#include <QWidget>
#include <QList>
#include <QHBoxLayout>

#include "tool/qt6_qt5_compat.h"

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
	int m_defaultMinValue;					///< Default minimum value used for new Mixer
	int m_defaultMaxValue;					///< Default maximum value used for new Mixer

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
	void setDefaultMax(int max);
	int defaultMax() const {return m_defaultMaxValue;}
	inline bool isMultiSelectEnabled() const {return m_propEnableMultiSelect;}
	void setMultiSelectEnabled(bool state);
	void setRangeSelectEnabled(bool state);
	MixerChannel *findMixerAtPos(QPoint pos);
	MixerChannel *getMixerById(int id);
	bool selectMixer(MixerChannel *mixer, int id, bool state);
	bool selectMixerRange(MixerChannel *fromMixer, MixerChannel *toMixer, bool state);
	QList<MixerChannel*> & selectedMixer() {return selected_mixer;}
	void unselectAllMixers();
	QHBoxLayout * mixerLayout() const {return mixerlayout;}

protected:
	void resizeEvent(QResizeEvent *qevent) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *qevent) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

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
