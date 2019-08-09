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

#include "scenedeskwidget.h"
#include "log.h"
#include "fxsceneitem.h"
#include "dmxchannel.h"
#include "appcentral.h"
#include "lightcontrol.h"
#include "qtstatictools.h"
#include "customwidget/mixergroup.h"
#include "customwidget/mixerchannel.h"

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QInputDialog>

QList<SceneDeskWidget*>SceneDeskWidget::scene_desk_list;


SceneDeskWidget::SceneDeskWidget(FxSceneItem *scene, QWidget *parent)
	: QWidget(parent)
{
	init();
	setupUi(this);
	setFxScene(scene);
	init_gui();
}

SceneDeskWidget::~SceneDeskWidget()
{
	scene_desk_list.removeOne(this);
	DEBUGTEXT("Desk destroyed");
}

void SceneDeskWidget::init_gui()
{
	setAttribute(Qt::WA_DeleteOnClose);

	connect(AppCentral::instance()->unitLight,SIGNAL(outputUniverseChanged(int,QByteArray)),this,SLOT(notifyChangedUniverse(int,QByteArray)));
	connect(faderAreaWidget,SIGNAL(mixerSelected(bool,int)),this,SLOT(setTubeSelected(bool,int)));
	connect(faderAreaWidget,SIGNAL(mixerDraged(int,int)),this,SLOT(if_mixerDraged(int,int)));
}



/**
 * @brief SceneDeskWidget::setFxScene
 * @param scene
 * @return
 *
 * @todo make FxSceneItem *scene const. Scene can not be edited by multiple Widget Instances
 */
bool SceneDeskWidget::setFxScene(const FxSceneItem *scene)
{
	setSceneEditable(false);
	selected_tube_ids.clear();
	faderAreaWidget->clear();
	qDebug() << "set fxScene" << scene->tubeCount() << "tubes in scene";

	QByteArray (&dmxout)[MAX_DMX_UNIVERSE] = AppCentral::instance()->unitLight->dmxOutputValues;

	// First we save the pointer to the scene;
	origin_fxscene = const_cast<FxSceneItem*>(scene);

	if (!FxItem::exists(scene)) return false;

	this->blockSignals(true);

	QVector<DmxChannel*>sort;
	sort.fill(nullptr, scene->tubeCount());

	for (int t=0; t<scene->tubes.size(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		tube->tempTubeListIdx = t;
		if (tube->deskPositionIndex >= 0) {
			if (sort.size() < tube->deskPositionIndex) {
				qWarning("position %d not available for tube %d",tube->deskPositionIndex, t);
			} else {
				sort[tube->deskPositionIndex] = tube;
			}
		}
	}

	int posindex = 0;
	for (int t=0; t<scene->tubeCount(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		if (tube->deskPositionIndex < 0) {
			while (posindex < sort.size() && sort.at(posindex) != nullptr) {
				posindex++;
			}
			if (posindex < sort.size())
				sort[posindex] = tube;
		}
	}


	for (int t=0; t<sort.size(); t++) {
		DmxChannel *dmx = sort.at(t);
		// We create a new Mixer in the MixerGroup if this tube is visible
		if (dmx->deskVisibleFlag) {
			// Create a new Mixer an fill in relevant data
			MixerChannel *fader = faderAreaWidget->appendMixer();
			// We will need the Id to determine which DmxChannel an incoming fader signal belongs to
			// We have to keep the Id of the mixer channel in sync with Tubes list index!
			fader->setId(dmx->tempTubeListIdx);
			fader->setDmxId(dmx->dmxUniverse,dmx->dmxChannel, dmx);
			fader->setRange(0,dmx->targetFullValue);
			fader->setValue(dmx->targetValue);
			fader->setChannelShown(true);
			fader->setLabelText(dmx->labelText);
			fader->setDmxType(dmx->dmxChannelType());

			int ref_val = quint8(dmxout[dmx->dmxUniverse][dmx->dmxChannel]);
			ref_val = ref_val * dmx->targetFullValue / 255;
			fader->setRefValue(ref_val);

			// and of course we need a slot to react on slider movement
			connect(fader,SIGNAL(mixerSliderMoved(int,int)),this,SLOT(set_mixer_val_on_moved(int,int)));
			// and we have to update the indicators for outside DMX level changes
			// connect(this,SIGNAL(dmxValueWantsUpdate(int,int,int)),fader,SLOT(notifyChangedDmxChannel(int,int,int)));
		}
	}

	// Set additional information widgets;
	sceneNameEdit->setText(origin_fxscene->name());
	faderGroup->setTitle(origin_fxscene->name());
	hookedUniverseSpin->setValue(origin_fxscene->hookedUniverse()+1);
	hookedChannelSpin->setValue(origin_fxscene->hookedChannel()+1);
	fadeInTimeEdit->setText(QtStaticTools::msToTimeString(origin_fxscene->fadeInTime()));
	fadeOutTimeEdit->setText(QtStaticTools::msToTimeString(origin_fxscene->fadeOutTime()));
	channelCountSpin->setValue(origin_fxscene->tubeCount());

	QRect rect = QtStaticTools::stringToQRect(origin_fxscene->widgetPosition());
	if (!rect.isNull()) {
		setGeometry(rect);
	}

	this->blockSignals(false);
	return true;
}

void SceneDeskWidget::setControlKey(bool state)
{
	ctrl_pressed_f = state;
	faderAreaWidget->setMultiSelectEnabled(ctrl_pressed_f);
}

void SceneDeskWidget::setShiftKey(bool state)
{
	shift_pressed_f = state;
	faderAreaWidget->setRangeSelectEnabled(shift_pressed_f);
}

DmxChannel *SceneDeskWidget::getTubeFromMixer(const MixerChannel *mixer) const
{
	for (int t=0; t<origin_fxscene->tubeCount(); t++) {
		DmxChannel *dmx = origin_fxscene->tubes.at(t);
		if (dmx->dmxChannel == mixer->dmxChannel() && dmx->dmxUniverse == mixer->dmxUniverse()) {
			return dmx;
		}
	}
	return 0;
}

/**
 * @brief Find Tube at mouse position
 * @param pos Mouse position relativ to this widget
 * @param dmxChannel Pointer to variable that will receive the DmxChannel instance at position or NULL if not needed
 * @return Pointer to DmxChannel instance at position or NULL if not found
 */
DmxChannel *SceneDeskWidget::getTubeAtPos(QPoint pos, MixerChannel **dmxChannel)
{
	DmxChannel *tube = 0;
	// First find the mixer at postion
	MixerChannel *mixer = faderAreaWidget->findMixerAtPos(faderAreaWidget->mapFrom(this,pos));
	if (dmxChannel) {
		*dmxChannel = mixer;
	}

	if (!origin_fxscene) return tube;

	// Now get tube for MixerChannel Id in scene Tube list
	if (mixer->id() >= 0 && mixer->id() < origin_fxscene->tubes.size()) {
		tube = origin_fxscene->tubes.at(mixer->id());
	}
	return tube;
}

void SceneDeskWidget::setTubeSelected(bool state, int id)
{
	if (!origin_fxscene) return;

	if (state) {
		if (!selected_tube_ids.contains(id)) {
			if (id >=0 && id < origin_fxscene->tubeCount()) {
				selected_tube_ids.append(id);
			}
		}
		copyTubeSettingsToGui(id);
	} else {
		selected_tube_ids.removeAll(id);
		copyTubeSettingsToGui(-1);
	}
	qDebug() << "tubes selected:" << selected_tube_ids;
}

void SceneDeskWidget::setSceneEditable(bool state)
{
	QList<PsLineEdit*>childs = findChildren<PsLineEdit*>();
	foreach(PsLineEdit* edit, childs) {
		edit->setReadOnly(!state);
		edit->setWarnColor(true);
		edit->setEditable(state);
	}

	editCheck->setChecked(state);
}

void SceneDeskWidget::setCurrentSceneLiveState(bool state)
{
	if (!FxItem::exists(origin_fxscene)) return;
	if (origin_fxscene->isLive() == state) return;

	origin_fxscene->setLive(state);

	if (state) {
		faderAreaWidget->setRefSliderColorIndex(1);
		for (int t=0; t<origin_fxscene->tubeCount(); t++) {
			DmxChannel *tube = origin_fxscene->tubes.at(t);
			tube->curValue[MIX_INTERN] = tube->targetValue;
		}
		AppCentral::instance()->unitLight->setSceneActive(origin_fxscene);

	} else {
		faderAreaWidget->setRefSliderColorIndex(0);
		if (!origin_fxscene->isOnStageIntern()) {
			for (int t=0; t<origin_fxscene->tubeCount(); t++) {
				DmxChannel *tube = origin_fxscene->tubes.at(t);
				tube->curValue[MIX_INTERN] = 0;
			}
		}
	}
}

void SceneDeskWidget::copyTubeSettingsToGui(int id)
{
	if (!FxItem::exists(origin_fxscene)) return;

	DmxChannel *tube = origin_fxscene->tube(id);
	if (tube) {
		tubeCommentEdit->setText(tube->labelText);
	} else {
		tubeCommentEdit->clear();
	}
}

void SceneDeskWidget::if_input_was_assigned(FxItem *fx)
{
	disconnect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
	if (fx == origin_fxscene) {
		hookedUniverseSpin->setValue(origin_fxscene->hookedUniverse()+1);
		hookedChannelSpin->setValue(origin_fxscene->hookedChannel()+1);
	}
	autoHookButton->clearFocus();
	setSceneEditable(false);
}

void SceneDeskWidget::init()
{
	scene_is_live_f = false;
	ctrl_pressed_f = false;
	shift_pressed_f = false;

}

void SceneDeskWidget::closeEvent(QCloseEvent *)
{
	setCurrentSceneLiveState(false);

	if (origin_fxscene) {
		origin_fxscene->setWidgetPosition(QtStaticTools::qRectToString(geometry()));
	}

	if (debug) DEBUGTEXT("Close scene desk");
}


void SceneDeskWidget::notifyChangedUniverse(int universe, const QByteArray &dmxValues)
{
	faderAreaWidget->notifyChangedDmxUniverse(universe,dmxValues);

	if (!FxItem::exists(origin_fxscene)) return;

//	for (int t=0;t<cur_fxscene->tubeCount(); t++) {
//		DmxChannel *dmx = cur_fxscene->tubes.at(t);
//		if (dmx->dmxUniverse == universe) {
//			emit dmxValueWantsUpdate(universe,dmx->dmxChannel,quint8(dmxValues[dmx->dmxChannel]));
//		}
//	}
}

void SceneDeskWidget::set_mixer_val_on_moved(int val, int id)
{
	if (!FxItem::exists(origin_fxscene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	origin_fxscene->tubes.at(id)->targetValue = val;
	if (scene_is_live_f) {
		origin_fxscene->tubes.at(id)->curValue[MIX_INTERN] = val;
	}
}

void SceneDeskWidget::on_liveCheck_clicked(bool checked)
{
	scene_is_live_f = checked;

	if (!FxItem::exists(origin_fxscene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	setCurrentSceneLiveState(checked);
	setSceneEditable(checked);
}

void SceneDeskWidget::on_hookedUniverseSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(origin_fxscene)) return;

	origin_fxscene->hookToUniverse(arg1-1);

}

void SceneDeskWidget::on_hookedChannelSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(origin_fxscene)) return;

	origin_fxscene->hookToChannel(arg1-1);

}

void SceneDeskWidget::if_mixerDraged (int fromIdx, int toIdx)
{
	if (!FxItem::exists(origin_fxscene)) return;
	qDebug("Mixer moved from position %d to %d",fromIdx,toIdx);

	QHBoxLayout *mixlayout = faderAreaWidget->mixerLayout();
	for (int pos=0; pos<mixlayout->count(); pos++) {
		MixerChannel *mix = qobject_cast<MixerChannel*>(mixlayout->itemAt(pos)->widget());
		if (!mix)
			continue;

		for (int t=0; t<origin_fxscene->tubeCount(); t++) {
			DmxChannel *tube = origin_fxscene->tubes.at(t);
			if (tube->tempTubeListIdx == mix->id()) {
				if (tube->tube != pos) {
					tube->deskPositionIndex = pos;
				} else {
					tube->deskPositionIndex = -1;
				}
				continue;
			}
		}
	}

	origin_fxscene->setModified(true);
}


void SceneDeskWidget::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	switch(key) {
	case Qt::Key_Control:
		setControlKey(true);
		break;
	case Qt::Key_Shift:
		setShiftKey(true);
		break;
	case Qt::Key_F5:
		setTypeOfSelectedTubes(DMX_PAN);
		break;
	case Qt::Key_F6:
		setTypeOfSelectedTubes(DMX_INTENSITY);
		break;
	case Qt::Key_Delete:
		deleteSelectedTubes();
		break;
	}
}

void SceneDeskWidget::keyReleaseEvent(QKeyEvent *event)
{
	int key = event->key();
	switch(key) {
	case Qt::Key_Control:
		setControlKey(false);
		break;
	case Qt::Key_Shift:
		setShiftKey(false);
		break;
	}
}

/**
 * @brief Hide the mixer for a tube in the current scene
 * @param tube Pointer to DmxChannel (Tube) instance
 * @param mixer Pointer to associated MixerChannel (Mixer) instance
 * @return true if a Mixer and Tube were found and Mixer was removed
 *
 * This function removes the Mixer from current SceneDesk and sets
 * the corresponding "deskVisableFlag" for the tube to false
 *
 */
bool SceneDeskWidget::hideTube(DmxChannel *tube, MixerChannel *mixer)
{
	if (!origin_fxscene || !tube) return false;

	tube->deskVisibleFlag = false;
	origin_fxscene->setModified(true);

	if (mixer) {
		if (faderAreaWidget->removeMixer(mixer)) {
			emit modified();
			return true;
		}
	}
	return false;
}

bool SceneDeskWidget::hideSelectedTubes()
{
	if (!origin_fxscene) return false;

	bool removed = false;

	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		removed |= hideTube( origin_fxscene->tubes.at(id), faderAreaWidget->getMixerById(id) );
	}
	selected_tube_ids.clear();

	return removed;
}

bool SceneDeskWidget::setTypeOfSelectedTubes(DmxChannelType type)
{
	if (!origin_fxscene) return false;
	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		DmxChannel *tube = origin_fxscene->tubes.at(id);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		tube->dmxType = type;
		mix->setDmxType(type);
		mix->update();
	}
	return true;
}

bool SceneDeskWidget::deleteSelectedTubes()
{
	if (!origin_fxscene) return false;

	for (int t=selected_tube_ids.size()-1; t>=0; t--) {
		int id = selected_tube_ids.at(t);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		origin_fxscene->removeTube(id);
		faderAreaWidget->removeMixer(mix);
	}


	return setFxScene(origin_fxscene);
}

bool SceneDeskWidget::unhideAllTubes()
{
	if (!origin_fxscene) return false;

	for (int t=0; t<origin_fxscene->tubes.size(); t++) {
		DmxChannel *tube = origin_fxscene->tubes.at(t);
		tube->deskVisibleFlag = true;
	}

	return setFxScene(origin_fxscene);
}

int SceneDeskWidget::setLabelInSelectedTubes(const QString &text)
{
	if (!origin_fxscene) return -1;

	int count = 0;

	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		if (mix && id < origin_fxscene->tubeCount()) {
			mix->setLabelText(text);
			origin_fxscene->tubes.at(id)->labelText = text;
			count++;
		}
	}

	return count;
}



void SceneDeskWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QRect fadergroup(faderAreaWidget->mapToGlobal(faderAreaWidget->pos()),faderAreaWidget->size());
	if (!fadergroup.contains(event->globalPos())) return;

	QMenu menu(this);
	QAction *act;
	if (!selected_tube_ids.size()) {
		act = menu.addAction(tr("Edit Label"));
		act->setObjectName("3");
	}
	act = menu.addAction(tr("Hide Channel(s)"));
	act->setObjectName("1");
	act = menu.addAction(tr("Add Channel"));
	act->setObjectName("2");
	act = menu.addAction(tr("Show hidden channels"));
	act->setObjectName("4");

	act = menu.exec(event->globalPos());
	if (!act) return;

	MixerChannel *mixer;
	DmxChannel *tube = getTubeAtPos(event->pos(),&mixer);

	switch (act->objectName().toInt()) {
	case 1:
		if (selected_tube_ids.size()) {
			hideSelectedTubes();
		} else {
			hideTube(tube,mixer);
		}
		break;

	case 2:
		origin_fxscene->setTubeCount(origin_fxscene->tubeCount()+1);
		setFxScene(origin_fxscene);
		emit modified();
		break;

	case 3:
		mixer->setLabelText(QInputDialog::getText(this
												  ,tr("Edit")
												  ,tr("Enter text for channel")
												  ,QLineEdit::Normal
												  ,mixer->labelText()));
		break;

	case 4:
		unhideAllTubes();
		break;
	}
}


void SceneDeskWidget::on_fadeInTimeEdit_textEdited(const QString &arg1)
{
	if (!origin_fxscene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (origin_fxscene->fadeInTime() != time_ms) {
		origin_fxscene->setFadeInTime(time_ms);
		origin_fxscene->setModified(true);
		emit modified();
	}

}

void SceneDeskWidget::on_fadeOutTimeEdit_textEdited(const QString &arg1)
{
	if (!origin_fxscene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (origin_fxscene->fadeOutTime() != time_ms) {
		origin_fxscene->setFadeOutTime(time_ms);
		origin_fxscene->setModified(true);
		emit modified();
	}
}

void SceneDeskWidget::on_editCheck_clicked(bool checked)
{
	setSceneEditable(checked);
}

void SceneDeskWidget::on_sceneNameEdit_textEdited(const QString &arg1)
{
	if (!origin_fxscene) return;

	origin_fxscene->setName(arg1);
	faderGroup->setTitle(arg1);
	origin_fxscene->setModified(true);
	emit modified();
}

void SceneDeskWidget::on_autoHookButton_clicked()
{
	if (!editCheck->isChecked()) {
		setSceneEditable(true);
	}

	// Set Input assign mode and wait for answer in if_input_was_assigned()
	AppCentral::instance()->setInputAssignMode(origin_fxscene);
	connect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
}

void SceneDeskWidget::on_tubeCommentEdit_textEdited(const QString &arg1)
{
	setLabelInSelectedTubes(arg1);
}


SceneDeskWidget * SceneDeskWidget::openSceneDesk(FxSceneItem *scene, QWidget *parent)
{
	foreach(SceneDeskWidget *desk, scene_desk_list) {
		if (desk->origin_fxscene == scene) {
			desk->setWindowState(desk->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
			desk->raise();
			desk->show();
			return 0;
		}
	}

	SceneDeskWidget *scenedesk = new SceneDeskWidget(scene,parent);
	scene_desk_list.append( scenedesk );

	scenedesk->setWindowIcon(QPixmap(":/gfx/icons/scene_mixer.png"));
	return scenedesk;
}

void SceneDeskWidget::destroyAllSceneDesks()
{
	while (!scene_desk_list.isEmpty()) {
		delete scene_desk_list.takeFirst();
	}
}

void SceneDeskWidget::on_channelCountSpin_valueChanged(int arg1)
{
	Q_UNUSED(arg1)

//	if (!origin_fxscene) return;
//	origin_fxscene->setTubeCount(arg1);
//	setFxScene(origin_fxscene);
}

void SceneDeskWidget::on_channelCountSpin_editingFinished()
{
	if (!origin_fxscene) return;
	origin_fxscene->setTubeCount(channelCountSpin->value());
	setFxScene(origin_fxscene);
}

void SceneDeskWidget::on_cloneCurrentInputButton_clicked()
{
	if (!origin_fxscene) return;

	AppCentral::ref().unitLight->fillSceneFromInputUniverses(origin_fxscene);

	setFxScene(origin_fxscene);
}

void SceneDeskWidget::on_cloneCurrentOutputButton_clicked()
{
	if (!origin_fxscene) return;

}
