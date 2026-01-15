//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include "mixergroup.h"
#include "mixerchannel.h"
#include "extmimedata.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>

MixerGroup::MixerGroup(QWidget *parent) :
	QWidget(parent)
{
	m_propEnableMultiSelect = false;
	m_propEnableRangeSelect = false;

	m_defaultMinValue = 0;
	m_defaultMaxValue = 100;
	temp_drag_move_idx = -1;
	temp_drag_start_move_idx = -1;
	temp_drag_widget = 0;

	mixerlayout = new QHBoxLayout;
	mixerlayout->setSpacing(0);
	mixerlayout->setContentsMargins(0,0,0,0);
	setLayout(mixerlayout);

	setAcceptDrops(true);

	setMixerCount(12);
}

/**
 * @brief Set Number of Mixers to be displayed
 * @param number
 *
 * This function deletes all current mixers if there number is lower
 * than new one
 */
void MixerGroup::setMixerCount(int number)
{
	while (mixerlist.size() > number) {
		delete mixerlist.takeLast();
	}

	while (mixerlist.size() < number) {
		appendMixer(getNewId());
	}
}

void MixerGroup::clear()
{
	while (mixerlist.size()) {
		delete mixerlist.takeFirst();
	}
}

MixerChannel *MixerGroup::appendMixer(int tubeId)
{
	if (getMixerById(tubeId))
		return nullptr;

	MixerChannel *mixer = new MixerChannel;
	mixer->setId(tubeId);
	mixer->setRange(m_defaultMinValue,m_defaultMaxValue);
	mixer->setSelectable(true);
	mixerlist.append(mixer);
	mixerlayout->addWidget(mixer);
	mixer->setObjectName(QString::number(mixer->id()));
	connect(mixer,SIGNAL(mixerChannelSliderMoved(int,int)),this,SLOT(onMixerChannelMoved(int,int)));
	connect(mixer,SIGNAL(mixerSelected(int,bool)),this,SLOT(onMixerChannelSelected(int,bool)));

	int right_margin = width() - mixerlist.size() * mixer->backGroundWidth();
	if (right_margin > 0) {
		layout()->setContentsMargins(0,0,right_margin,0);
	} else {
		layout()->setContentsMargins(0,0,0,0);
	}

	return mixer;
}

bool MixerGroup::removeMixer(MixerChannel *mixer)
{
	bool removed = mixerlist.removeOne(mixer);

	if (removed) {
		selected_mixer.removeOne(mixer);
		delete mixer;
		return true;
	}

	return false;
}

void MixerGroup::setIdsToMixerListIndex()
{
	for (int t=0; t<mixerlist.size(); t++) {
		mixerlist.at(t)->setId(t);
	}
}

void MixerGroup::setRange(int min, int max)
{
	m_defaultMinValue = min;
	m_defaultMaxValue = max;

	for (int t=0; t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		mixer->setRange(min,max);
		mixer->update();
	}
}

void MixerGroup::setDefaultMax(int max)
{
	m_defaultMaxValue = max;
}

void MixerGroup::setMultiSelectEnabled(bool state)
{
	m_propEnableMultiSelect = state;
}

void MixerGroup::setRangeSelectEnabled(bool state)
{
	m_propEnableRangeSelect = state;
}

MixerChannel *MixerGroup::findMixerAtPos(QPoint pos)
{
	for (int t=0; t<mixerlist.size(); t++) {
		QRect rect(mixerlist.at(t)->pos(),mixerlist.at(t)->size());
		if (rect.contains(pos)) {
			return mixerlist.at(t);
		}
	}
	return nullptr;
}

MixerChannel *MixerGroup::getMixerById(int id)
{
	for (int t=0; t<mixerlist.size(); t++) {
		if (mixerlist.at(t)->id() == id)
			return mixerlist.at(t);
	}
	return nullptr;
}

int MixerGroup::getNewId() const
{
	int newid = -1;
	for (int t=0; t<mixerlist.size(); t++) {
		newid = qMax(mixerlist.at(t)->id(), newid);
	}
	return newid + 1;

}

bool MixerGroup::deleteMixerById(int id)
{
	MixerChannel *mix = nullptr;
	for (int t=0; t<mixerlist.size(); t++) {
		if (mixerlist.at(t)->id() == id) {
			mix = mixerlist.takeAt(t);
			break;
		}
	}

	if (mix)
		delete mix;

	return false;
}

bool MixerGroup::selectMixer(MixerChannel *mixer, int id, bool state)
{
	if (!mixerlist.contains(mixer))
		return false;

	if (state) {
		if (!selected_mixer.contains(mixer)) {
			selected_mixer.append(mixer);
			mixer->setSelected(true);
			emit mixerSelected(id, true);
		}
	} else {
		if (selected_mixer.contains(mixer)) {
			selected_mixer.removeAll(mixer);
			mixer->setSelected(false);
			emit mixerSelected(id, false);
		}
	}

	return true;
}

bool MixerGroup::selectMixerRange(MixerChannel *fromMixer, MixerChannel *toMixer, bool state)
{
	// find position in list for the lower and upper mixer
	int from_idx = mixerlist.indexOf(fromMixer);
	int to_idx = mixerlist.indexOf(toMixer);

	// if mixer does not exist we cancel any action
	if (from_idx < 0 || to_idx < 0) return false;

	if (to_idx > from_idx) {
		for (int t=0; t<mixerlist.size();t++) {
			if (t < from_idx || t > to_idx) {
				selectMixer(mixerlist.at(t),mixerlist.at(t)->id(),false);
			} else {
				selectMixer(mixerlist.at(t),mixerlist.at(t)->id(),state);
			}
		}
	} else {
		for (int t=mixerlist.size()-1; t>= 0; t--) {
			if (t > from_idx || t < to_idx) {
				selectMixer(mixerlist.at(t),mixerlist.at(t)->id(),false);
			} else {
				selectMixer(mixerlist.at(t),mixerlist.at(t)->id(),state);
			}
		}
	}

	return true;
}

void MixerGroup::unselectAllMixers()
{
	while (!selectedMixer().isEmpty()) {
		MixerChannel *mix = selected_mixer.takeFirst();
		if (mixerlist.contains(mix)) {
			mix->setSelected(false);
			emit mixerSelected(mix->id(), false);
		}
	}
}

void MixerGroup::resizeEvent(QResizeEvent *event)
{
	if (!mixerlist.size()) return;

	int right_margin = event->size().width() - mixerlist.size() * mixerlist.first()->backGroundWidth();
	if (right_margin > 0) {
		layout()->setContentsMargins(0,0,right_margin,0);
	} else {
		layout()->setContentsMargins(0,0,0,0);
	}
}

void MixerGroup::dragEnterEvent(QDragEnterEvent *qevent)
{
	PDragEnterEvent* event = static_cast<PDragEnterEvent*>(qevent);
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

//	QObject * src = event->source();
	if (extmime && extmime->mixerChannel) {
		MixerChannel *movwid = extmime->mixerChannel;

//		qDebug("Drag enter event Mime:'%s': ObjectName:%s, row:%d, col:%d "
//			   ,mime->text().toLocal8Bit().data(),src->objectName().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);
		event->setDropAction(Qt::MoveAction);
		event->accept();

		// Be sure that drag is not initiated and remove the draged MixerChannel from layout
		if (temp_drag_move_idx < 0) {
			// Find layout item under mouse cursor
			for (int i=0; i<mixerlayout->count(); i++) {
				if (mixerlayout->itemAt(i)->widget()->geometry().contains(event->pos())) {
					temp_drag_move_idx = i;
					break;
				}
			}

			int drag_idx = mixerlayout->indexOf(movwid);
			temp_drag_widget = movwid;
			temp_drag_widget->hide();

			// This removes the current layout item at mouse position
			mixerlayout->takeAt(drag_idx);

			QWidget *wid = new QWidget(this);
			wid->setAutoFillBackground(true);
			wid->setMinimumWidth(movwid->width());
			wid->setMaximumWidth(movwid->width());
			wid->setObjectName("filler");
			mixerlayout->insertWidget(temp_drag_move_idx,wid);

			// This is to prevent original drag start position when reentering after dragLeaveEvent
			if (temp_drag_start_move_idx <0 ) {
				temp_drag_start_move_idx = temp_drag_move_idx;
			}
		}
	} else {
		event->ignore();
	}
}

void MixerGroup::dragMoveEvent(QDragMoveEvent *qevent)
{
	PDragEnterEvent* event = static_cast<PDragEnterEvent*>(qevent);
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	// QObject * src = event->source();
	if (extmime && extmime->mixerChannel) {
		// MixerChannel *movwid = extmime->mixerChannel;
		// search Widget (hopefully a MixerChannel) under mouse cursor
		for (int i=0; i<mixerlayout->count(); i++) {
			if (mixerlayout->itemAt(i)->widget()->geometry().contains(event->pos())) {
				if (temp_drag_move_idx != i) {
					if (temp_drag_move_idx >= 0) {
						QLayoutItem *fillitem;
						QLayoutItem *overitem;
						if (temp_drag_move_idx < i) {
							overitem = mixerlayout->takeAt(i);
							fillitem = mixerlayout->takeAt(temp_drag_move_idx);
							mixerlayout->insertItem(temp_drag_move_idx,overitem);
							mixerlayout->insertItem(i,fillitem);
						} else {
							fillitem = mixerlayout->takeAt(temp_drag_move_idx);
							overitem = mixerlayout->takeAt(i);
							mixerlayout->insertItem(i,fillitem);
							mixerlayout->insertItem(temp_drag_move_idx,overitem);
						}

					} else {
						qDebug() << "Widget not Found!!";
					}

					temp_drag_move_idx = i;
				}
				return;
			}
		}
	}
}

void MixerGroup::dragLeaveEvent(QDragLeaveEvent *event)
{
	Q_UNUSED(event);
	if (temp_drag_move_idx >= 0) {
		QLayoutItem *item = mixerlayout->takeAt(temp_drag_move_idx);
		delete item->widget();
		delete item;
		temp_drag_move_idx = -1;
	}
	if (temp_drag_start_move_idx) {
		if (temp_drag_widget) {
			mixerlayout->insertWidget(temp_drag_start_move_idx,temp_drag_widget);
			temp_drag_widget->show();
		}
	}
}

void MixerGroup::dropEvent(QDropEvent *event)
{
	Q_UNUSED(event);
	if (temp_drag_move_idx >= 0) {
		QLayoutItem *item = mixerlayout->takeAt(temp_drag_move_idx);
		delete item->widget();
		delete item;

		if (temp_drag_start_move_idx >= 0) {
			if (temp_drag_widget) {
				mixerlayout->insertWidget(temp_drag_move_idx,temp_drag_widget);
				temp_drag_widget->show();
				temp_drag_widget = 0;
			}
			if (temp_drag_move_idx != temp_drag_start_move_idx) {
				emit mixerDraged(temp_drag_start_move_idx,temp_drag_move_idx);
			}
		}

		temp_drag_move_idx = -1;
	}

	temp_drag_start_move_idx = -1;
}

void MixerGroup::onMixerChannelMoved(int id, int val)
{
	if (id >= 0) {
		if (val < 0)  {
			val = 0;
		}
		else if (val > m_defaultMaxValue) {
			val = m_defaultMaxValue;
		}

		MixerChannel *mix = getMixerById(id);
		if (mix && selected_mixer.contains(mix)) {
			for (int i=0; i<selected_mixer.size(); i++) {
				MixerChannel *m = selected_mixer.at(i);
				if (m->id() != id) {
					m->setValue(val);
					// qDebug() << i << "id" << m->id() << "=" << m << "val" << val;
				}

				emit mixerGroupSliderMoved(m->id(), val);
			}
		}
		else {
			emit mixerGroupSliderMoved(id, val);
		}
	}
}

void MixerGroup::onMixerChannelSelected(int id, bool state)
{
	static bool recursive_blocker = false;
	if (recursive_blocker == true)
		return;

	recursive_blocker = true;
//	qDebug() << Q_FUNC_INFO << "mix:" << id << state;
	MixerChannel *mixer = qobject_cast<MixerChannel*>(sender());
	if (!mixer) return;

	if (m_propEnableRangeSelect) {
		if (selected_mixer.size()) {
			selectMixerRange(selected_mixer.at(0),mixer,true);
		}
	}

	if (state && !m_propEnableMultiSelect && !m_propEnableRangeSelect)
		unselectAllMixers();

	selectMixer(mixer,id,state);

	recursive_blocker = false;
}

void MixerGroup::notifyChangedDmxUniverse(int universe, const QByteArray &dmxValues)
{
	for (int t=0;t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		if (mixer->dmxUniverse() == universe) {
			int target_value = quint8(dmxValues[mixer->dmxAddr()]) * mixer->maximum() / 255;
			if (mixer->refValue() != target_value) {
				mixer->setRefValue(target_value);
				mixer->update();
			}
		}
	}
}

void MixerGroup::setRefSliderColorIndex(int colidx)
{
	for (int t=0;t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		mixer->setRefSliderColorIndex(colidx);
	}
}
