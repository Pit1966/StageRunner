#include "mixergroup.h"
#include "mixerchannel.h"
#include "extmimedata.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>

MixerGroup::MixerGroup(QWidget *parent) :
	QWidget(parent)
{
	default_min = 0;
	default_max = 100;
	temp_drag_move_idx = -1;
	temp_drag_start_move_idx = -1;
	temp_drag_widget = 0;

	mixerlayout = new QHBoxLayout;
	mixerlayout->setSpacing(0);
	mixerlayout->setMargin(0);
	setLayout(mixerlayout);

	setAcceptDrops(true);

	setMixerCount(12);
}

/**
 * @brief Set Number of Mixers to be displayed
 * @param number
 *
 * This function deletes all current mixers if there numbe is lower
 * than new one
 */
void MixerGroup::setMixerCount(int number)
{
	while (mixerlist.size() > number) {
		delete mixerlist.takeLast();
	}

	while (mixerlist.size() < number) {
		appendMixer();
	}
}

void MixerGroup::clear()
{
	while (mixerlist.size()) {
		delete mixerlist.takeFirst();
	}
}

MixerChannel *MixerGroup::appendMixer()
{
	MixerChannel *mixer = new MixerChannel;
	mixer->setId(mixerlist.size());
	mixer->setRange(default_min,default_max);
	mixer->setSelectable(true);
	mixerlist.append(mixer);
	mixerlayout->addWidget(mixer);
	mixer->setObjectName(QString::number(mixer->id()));
	connect(mixer,SIGNAL(mixerSliderMoved(int,int)),this,SLOT(on_mixer_moved(int,int)));
	connect(mixer,SIGNAL(mixerSelected(bool,int)),this,SLOT(on_mixer_selected(bool,int)));

	int right_margin = width() - mixerlist.size() * mixer->backGroundWidth();
	if (right_margin > 0) {
		layout()->setContentsMargins(0,0,right_margin,0);
	} else {
		layout()->setContentsMargins(0,0,0,0);
	}

	return mixer;
}

bool MixerGroup::removeMixer(MixerChannel *mixer, bool renumberIds)
{
	bool removed = mixerlist.removeOne(mixer);

	if (removed) {
		selected_mixer.removeOne(mixer);
		delete mixer;
	}

	if (removed && renumberIds) {
		setIdsToMixerListIndex();
	}
	return removed;
}

void MixerGroup::setIdsToMixerListIndex()
{
	for (int t=0; t<mixerlist.size(); t++) {
		mixerlist.at(t)->setId(t);
	}
}

void MixerGroup::setRange(int min, int max)
{
	default_min = min;
	default_max = max;

	for (int t=0; t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		mixer->setRange(min,max);
		mixer->update();
	}
}

void MixerGroup::setMultiSelectEnabled(bool state)
{
	prop_multiselect_f = state;
}

MixerChannel *MixerGroup::findMixerAtPos(QPoint pos)
{
	for (int t=0; t<mixerlist.size(); t++) {
		QRect rect(mixerlist.at(t)->pos(),mixerlist.at(t)->size());
		if (rect.contains(pos)) {
			return mixerlist.at(t);
		}
	}
	return 0;
}

MixerChannel *MixerGroup::getMixerById(int id)
{
	for (int t=0; t<mixerlist.size(); t++) {
		if (mixerlist.at(t)->id() == id)
			return mixerlist.at(t);
	}
	return 0;
}

void MixerGroup::unselectAllMixers()
{
	for (int t=0; t<selected_mixer.size(); t++) {
		selected_mixer.takeFirst()->setSelected(false);
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

void MixerGroup::dragEnterEvent(QDragEnterEvent *event)
{
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

void MixerGroup::dragMoveEvent(QDragMoveEvent *event)
{
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	QObject * src = event->source();
	if (extmime && extmime->mixerChannel) {
		MixerChannel *movwid = extmime->mixerChannel;
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

void MixerGroup::on_mixer_moved(int val, int id)
{
	if (id >= 0) {
		emit mixerSliderMoved(val, id);
	}
}

void MixerGroup::on_mixer_selected(bool state, int id)
{
	MixerChannel *mixer = qobject_cast<MixerChannel*>(sender());
	if (mixer) {
		if (!prop_multiselect_f)
			unselectAllMixers();

		if (state) {
			selected_mixer.append(mixer);
		} else {
			selected_mixer.removeOne(mixer);
		}
	}

	if (id >= 0) {
		emit mixerSelected(state, id);
	}
}

void MixerGroup::notifyChangedDmxUniverse(int universe, const QByteArray &dmxValues)
{
	for (int t=0;t<mixerlist.size(); t++) {
		MixerChannel *mixer = mixerlist.at(t);
		if (mixer->dmxUniverse() == universe) {
			int target_value = quint8(dmxValues[mixer->dmxChannel()]) * mixer->maximum() / 255;
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
