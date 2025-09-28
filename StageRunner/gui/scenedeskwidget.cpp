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

QList<SceneDeskWidget*>SceneDeskWidget::m_sceneDeskList;


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
	m_sceneDeskList.removeOne(this);
	DEBUGTEXT("Desk destroyed");
}

void SceneDeskWidget::init_gui()
{
	setAttribute(Qt::WA_DeleteOnClose);

	connect(AppCentral::instance()->unitLight,SIGNAL(outputUniverseChanged(int,QByteArray)),this,SLOT(notifyChangedUniverse(int,QByteArray)));
	connect(faderAreaWidget,SIGNAL(mixerSelected(bool,int)),this,SLOT(setTubeSelected(bool,int)));
	connect(faderAreaWidget,SIGNAL(mixerDraged(int,int)),this,SLOT(if_mixerDraged(int,int)));

	connect(channelCountSpin,SIGNAL(clickedAndChanged(int)),this,SLOT(onChannelCountSpinClickedAndChanged(int)),Qt::QueuedConnection);
	connect(channelCountSpin,SIGNAL(editingFinished()),this,SLOT(onChannelCountSpinEditingFinished()),Qt::QueuedConnection);
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
	m_selectedTubeIds.clear();
	faderAreaWidget->clear();
	// qDebug() << "set fxScene" << scene->tubeCount() << "tubes in scene";

	QByteArray (&dmxout)[MAX_DMX_UNIVERSE] = AppCentral::instance()->unitLight->dmxOutputValues;

	// First we save the pointer to the scene;
	m_originFxScene = const_cast<FxSceneItem*>(scene);

	if (!FxItem::exists(scene)) return false;

	this->blockSignals(true);

	QVector<DmxChannel*>sort;
	sort.fill(nullptr, scene->tubeCount());

	for (int t=0; t<scene->tubes.size(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		tube->tempTubeListIdx = t;
		if (tube->deskPositionIndex >= 0) {
			if (tube->deskPositionIndex >= sort.size()) {
				qWarning("%s position %d not available for tube %d",Q_FUNC_INFO, tube->deskPositionIndex, t);
				tube->deskPositionIndex = -1;
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

	int comUniverse = -1;

	for (int t=0; t<sort.size(); t++) {
		DmxChannel *dmx = sort.at(t);
		if (!dmx) {
			qWarning() << Q_FUNC_INFO << "Missing DmxChannel instance:" << t;
			continue;
		}
		// check if all tubes have the same universe
		if (comUniverse == -1) {
			comUniverse = dmx->dmxUniverse;
		}
		else if (comUniverse != dmx->dmxUniverse) {
			comUniverse = -2;
		}


		// We create a new Mixer in the MixerGroup if this tube is visible
		if (dmx->deskVisibleFlag) {
			// Create a new Mixer an fill in relevant data
			MixerChannel *fader = faderAreaWidget->appendMixer();
			// We will need the Id to determine which DmxChannel an incoming fader signal belongs to
			// We have to keep the Id of the mixer channel in sync with Tubes list index!
			fader->setId(dmx->tempTubeListIdx);
			fader->setDmxId(dmx->dmxUniverse,dmx->dmxChannel, dmx);
			fader->setRange(0,dmx->targetFullValue);
			if (dmx->targetFullValue > faderAreaWidget->defaultMax())
				faderAreaWidget->setDefaultMax(dmx->targetFullValue);

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

	if (comUniverse >= 0) {
		universeSpin->setValue(comUniverse + 1);
		universeSpin->setDisabled(false);
	} else {
		universeSpin->setValue(0);
		universeSpin->setDisabled(true);
	}

	// Set additional information widgets;
	sceneNameEdit->setText(m_originFxScene->name());
	faderGroup->setTitle(m_originFxScene->name());
	hookedUniverseSpin->setValue(m_originFxScene->hookedUniverse()+1);
	hookedChannelSpin->setValue(m_originFxScene->hookedChannel()+1);
	fadeInTimeEdit->setText(QtStaticTools::msToTimeString(m_originFxScene->fadeInTime()));
	fadeOutTimeEdit->setText(QtStaticTools::msToTimeString(m_originFxScene->fadeOutTime()));
	channelCountSpin->setValue(m_originFxScene->tubeCount());

	QRect rect = QtStaticTools::stringToQRect(m_originFxScene->widgetPosition());
	if (!rect.isNull()) {
		setGeometry(rect);
	}

	this->blockSignals(false);
	return true;
}

void SceneDeskWidget::setControlKey(bool state)
{
	m_isCtrlPressed = state;
	faderAreaWidget->setMultiSelectEnabled(m_isCtrlPressed);
}

void SceneDeskWidget::setShiftKey(bool state)
{
	m_isShiftPressed = state;
	faderAreaWidget->setRangeSelectEnabled(m_isShiftPressed);
}

DmxChannel *SceneDeskWidget::getTubeFromMixer(const MixerChannel *mixer) const
{
	for (int t=0; t<m_originFxScene->tubeCount(); t++) {
		DmxChannel *dmx = m_originFxScene->tubes.at(t);
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
	DmxChannel *tube = nullptr;
	// First find the mixer at postion
	MixerChannel *mixer = faderAreaWidget->findMixerAtPos(faderAreaWidget->mapFrom(this,pos));
	if (dmxChannel) {
		*dmxChannel = mixer;
	}

	if (!m_originFxScene) return tube;

	// Now get tube for MixerChannel Id in scene Tube list
	if (mixer->id() >= 0 && mixer->id() < m_originFxScene->tubes.size()) {
		tube = m_originFxScene->tubes.at(mixer->id());
	}
	return tube;
}

void SceneDeskWidget::setTubeSelected(bool state, int id)
{
	if (!m_originFxScene) return;

	if (state) {
		if (!m_selectedTubeIds.contains(id)) {
			if (id >=0 && id < m_originFxScene->tubeCount()) {
				m_selectedTubeIds.append(id);
			}
		}
		copyTubeSettingsToGui(id);
	} else {
		m_selectedTubeIds.removeAll(id);
		copyTubeSettingsToGui(-1);
	}
	qDebug() << "tubes selected:" << m_selectedTubeIds << "id:" << id << state;
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
	if (!FxItem::exists(m_originFxScene)) return;
	if (m_originFxScene->isLive() == state) return;

	m_originFxScene->setLive(state);

	if (state) {
		faderAreaWidget->setRefSliderColorIndex(1);
		for (int t=0; t<m_originFxScene->tubeCount(); t++) {
			DmxChannel *tube = m_originFxScene->tubes.at(t);
			tube->curValue[MIX_INTERN] = tube->targetValue;
		}
		AppCentral::instance()->unitLight->setSceneActive(m_originFxScene);

	} else {
		faderAreaWidget->setRefSliderColorIndex(0);
		if (!m_originFxScene->isOnStageIntern()) {
			for (int t=0; t<m_originFxScene->tubeCount(); t++) {
				DmxChannel *tube = m_originFxScene->tubes.at(t);
				tube->curValue[MIX_INTERN] = 0;
			}
		}
	}
}

void SceneDeskWidget::copyTubeSettingsToGui(int id)
{
	if (!FxItem::exists(m_originFxScene)) return;

	DmxChannel *tube = m_originFxScene->tube(id);
	if (tube) {
		tubeCommentEdit->setText(tube->labelText);
	} else {
		tubeCommentEdit->clear();
	}
}

void SceneDeskWidget::if_input_was_assigned(FxItem *fx)
{
	disconnect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
	if (fx == m_originFxScene) {
		hookedUniverseSpin->setValue(m_originFxScene->hookedUniverse()+1);
		hookedChannelSpin->setValue(m_originFxScene->hookedChannel()+1);
	}
	autoHookButton->clearFocus();
	setSceneEditable(false);
}

void SceneDeskWidget::init()
{
	m_isSceneLive = false;
	m_isCtrlPressed = false;
	m_isShiftPressed = false;

}

void SceneDeskWidget::closeEvent(QCloseEvent *)
{
	setCurrentSceneLiveState(false);

	if (m_originFxScene) {
		m_originFxScene->setWidgetPosition(QtStaticTools::qRectToString(geometry()));
	}

	if (debug) DEBUGTEXT("Close scene desk");
}


void SceneDeskWidget::notifyChangedUniverse(int universe, const QByteArray &dmxValues)
{
	faderAreaWidget->notifyChangedDmxUniverse(universe,dmxValues);

	// if (!FxItem::exists(m_originFxScene)) return;

	// for (int t=0;t<m_originFxScene->tubeCount(); t++) {
	// 	DmxChannel *dmx = m_originFxScene->tubes.at(t);
	// 	if (dmx->dmxUniverse == universe) {
	// 		emit dmxValueWantsUpdate(universe,dmx->dmxChannel,quint8(dmxValues[dmx->dmxChannel]));
	// 	}
	// }
}

void SceneDeskWidget::set_mixer_val_on_moved(int val, int id)
{
	if (!FxItem::exists(m_originFxScene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	DmxChannel *tube = m_originFxScene->tubes.at(id);
	if (val > tube->targetFullValue) {
		val = tube->targetFullValue;
	}
	else if (val < 0) {
		val = 0;
	}

	tube->targetValue = val;
	if (m_isSceneLive) {
		tube->curValue[MIX_INTERN] = val;
	}
}

void SceneDeskWidget::on_liveCheck_clicked(bool checked)
{
	m_isSceneLive = checked;

	if (!FxItem::exists(m_originFxScene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	setCurrentSceneLiveState(checked);
	setSceneEditable(checked);
}

void SceneDeskWidget::on_hookedUniverseSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(m_originFxScene)) return;

	m_originFxScene->hookToUniverse(arg1-1);

}

void SceneDeskWidget::on_hookedChannelSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(m_originFxScene)) return;

	m_originFxScene->hookToChannel(arg1-1);

}

void SceneDeskWidget::if_mixerDraged (int fromIdx, int toIdx)
{
	if (!FxItem::exists(m_originFxScene)) return;
	qDebug("Mixer moved from position %d to %d",fromIdx,toIdx);

	QHBoxLayout *mixlayout = faderAreaWidget->mixerLayout();
	for (int pos=0; pos<mixlayout->count(); pos++) {
		MixerChannel *mix = qobject_cast<MixerChannel*>(mixlayout->itemAt(pos)->widget());
		if (!mix)
			continue;

		for (int t=0; t<m_originFxScene->tubeCount(); t++) {
			DmxChannel *tube = m_originFxScene->tubes.at(t);
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

	m_originFxScene->setModified(true);
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
		setTypeOfSelectedTubes(DMX_POSITION_PAN);
		break;
	case Qt::Key_F6:
		setTypeOfSelectedTubes(DMX_POSITION_TILT);
		break;
	case Qt::Key_F7:
		setTypeOfSelectedTubes(DMX_INTENSITY_DIMMER);
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
 * the corresponding "deskVisibleFlag" for the tube to false
 *
 */
bool SceneDeskWidget::hideTube(DmxChannel *tube, MixerChannel *mixer)
{
	if (!m_originFxScene || !tube) return false;

	tube->deskVisibleFlag = false;
	m_originFxScene->setModified(true);

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
	if (!m_originFxScene) return false;

	bool removed = false;

	for (int t=0; t<m_selectedTubeIds.size(); t++) {
		int id = m_selectedTubeIds.at(t);
		removed |= hideTube( m_originFxScene->tubes.at(id), faderAreaWidget->getMixerById(id) );
	}
	m_selectedTubeIds.clear();

	return removed;
}

bool SceneDeskWidget::setTypeOfSelectedTubes(DmxChannelType type)
{
	if (!m_originFxScene) return false;
	for (int t=0; t<m_selectedTubeIds.size(); t++) {
		int id = m_selectedTubeIds.at(t);
		DmxChannel *tube = m_originFxScene->tubes.at(id);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		tube->dmxType = type;
		mix->setDmxType(type);
		mix->update();
	}
	return true;
}

bool SceneDeskWidget::deleteSelectedTubes()
{
	if (!m_originFxScene) return false;

	for (int t=m_selectedTubeIds.size()-1; t>=0; t--) {
		int id = m_selectedTubeIds.at(t);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		m_originFxScene->removeTube(id);
		faderAreaWidget->removeMixer(mix);
	}


	return setFxScene(m_originFxScene);
}

bool SceneDeskWidget::unhideAllTubes()
{
	if (!m_originFxScene) return false;

	for (int t=0; t<m_originFxScene->tubes.size(); t++) {
		DmxChannel *tube = m_originFxScene->tubes.at(t);
		tube->deskVisibleFlag = true;
	}

	return setFxScene(m_originFxScene);
}

int SceneDeskWidget::setLabelInSelectedTubes(const QString &text)
{
	if (!m_originFxScene) return -1;

	int count = 0;

	for (int t=0; t<m_selectedTubeIds.size(); t++) {
		int id = m_selectedTubeIds.at(t);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		if (mix && id < m_originFxScene->tubeCount()) {
			mix->setLabelText(text);
			m_originFxScene->tubes.at(id)->labelText = text;
			count++;
		}
	}

	return count;
}

int SceneDeskWidget::setUniverseInTubes(int universe)
{
	if (!m_originFxScene) return -1;

	if (universe < 0)
		universe = 0;
	else if (universe >= MAX_DMX_UNIVERSE)
		universe = MAX_DMX_UNIVERSE - 1;

	int changed = 0;

	if (m_selectedTubeIds.isEmpty()) {
		for (int i=0; i<m_originFxScene->tubeCount(); i++) {
			if (m_originFxScene->tubes.at(i)->dmxUniverse != universe) {
				m_originFxScene->tubes.at(i)->dmxUniverse = universe;
				m_originFxScene->setModified(true);

				int id = m_originFxScene->tubes.at(i)->tube;
				MixerChannel *mix = faderAreaWidget->getMixerById(id);
				if (mix)
					mix->setUniverse(universe);

				changed++;
			}
		}
	}
	else {
		for (int t=0; t<m_selectedTubeIds.size(); t++) {
			int id = m_selectedTubeIds.at(t);
			MixerChannel *mix = faderAreaWidget->getMixerById(id);
			if (mix && id < m_originFxScene->tubeCount()) {
				if (m_originFxScene->tubes.at(id)->dmxUniverse != universe) {
					m_originFxScene->tubes.at(id)->dmxUniverse = universe;
					mix->setUniverse(universe);
					changed++;
				}
			}
		}
	}

	if (changed > 0)
		faderAreaWidget->update();

	return changed;
}



void SceneDeskWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QRect fadergroup(faderAreaWidget->mapToGlobal(faderAreaWidget->pos()),faderAreaWidget->size());
	if (!fadergroup.contains(event->globalPos())) return;

	QMenu menu(this);
	QAction *act;
	if (!m_selectedTubeIds.size()) {
		act = menu.addAction(tr("Edit Label"));
		act->setObjectName("3");
		act = menu.addAction(tr("Set DMX Level Value"));
		act->setObjectName("5");
	}
	else {
		act = menu.addAction(tr("Unselect all"));
		act->setObjectName("6");
		act = menu.addAction(tr("Hide selected Channel(s)"));
		act->setObjectName("1");
	}
	act = menu.addAction(tr("Add Channel"));
	act->setObjectName("2");
	act = menu.addAction(tr("Show hidden channels"));
	act->setObjectName("4");

	act = menu.exec(event->globalPos());
	if (!act) return;

	MixerChannel *mixer;
	DmxChannel *tube = getTubeAtPos(event->pos(), &mixer);

	switch (act->objectName().toInt()) {
	case 1:
		if (m_selectedTubeIds.size()) {
			hideSelectedTubes();
		} else {
			hideTube(tube,mixer);
		}
		break;

	case 2:
		m_originFxScene->setTubeCount(m_originFxScene->tubeCount()+1);
		setFxScene(m_originFxScene);
		emit modified();
		break;

	case 3:
		if (mixer && tube) {
			bool ok;
			QString label = QInputDialog::getText(this
												  ,tr("Edit")
												  ,tr("Enter text for channel")
												  ,QLineEdit::Normal
												  ,mixer->labelText()
												  ,&ok);

			if (ok && tube->labelText != label) {
				mixer->setLabelText(label);
				tube->labelText = label;
				m_originFxScene->setModified(true);
				emit modified();
			}
		}
		break;

	case 4:
		unhideAllTubes();
		break;

	case 5:		// set DMX level
		if (mixer && tube) {
			// calculate dmx level from current mixer level
			int curDmxVal = mixer->value() * 255 / mixer->maximum();
			int dmxChan = tube->dmxChannel;
			bool ok;
			int newDmxVal = QInputDialog::getInt(this,
											  tr("Edit"),
											  tr("Enter DMX level [0-255] for DMX channel %1").arg(dmxChan + 1),
											  curDmxVal,
											  0, 255, 1, &ok);
			if (ok && curDmxVal != newDmxVal) {
				int newTargetValue = (float(newDmxVal) + 0.5) * mixer->maximum() / 255;
				newTargetValue = qMin(newTargetValue, mixer->maximum());
				mixer->setValue(newTargetValue);
				mixer->emitCurrentValue();
				tube->targetValue= newTargetValue;
				m_originFxScene->setModified(true);
				emit modified();
			}
		}
		break;

	case 6:		//unselect all selected channels
		auto ids = m_selectedTubeIds;
		while (!ids.isEmpty()) {
			MixerChannel *mix = faderAreaWidget->getMixerById(ids.takeFirst());
			if (mix)
				mix->setSelected(false);
		}
		break;

	}
}


void SceneDeskWidget::on_fadeInTimeEdit_textEdited(const QString &arg1)
{
	if (!m_originFxScene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (m_originFxScene->fadeInTime() != time_ms) {
		m_originFxScene->setFadeInTime(time_ms);
		m_originFxScene->setModified(true);
		emit modified();
	}

}

void SceneDeskWidget::on_fadeOutTimeEdit_textEdited(const QString &arg1)
{
	if (!m_originFxScene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (m_originFxScene->fadeOutTime() != time_ms) {
		m_originFxScene->setFadeOutTime(time_ms);
		m_originFxScene->setModified(true);
		emit modified();
	}
}

void SceneDeskWidget::on_editCheck_clicked(bool checked)
{
	setSceneEditable(checked);
}

void SceneDeskWidget::on_sceneNameEdit_textEdited(const QString &arg1)
{
	if (!m_originFxScene) return;

	m_originFxScene->setName(arg1);
	faderGroup->setTitle(arg1);
	m_originFxScene->setModified(true);
	emit modified();
}

void SceneDeskWidget::on_autoHookButton_clicked()
{
	if (!editCheck->isChecked()) {
		setSceneEditable(true);
	}

	// Set Input assign mode and wait for answer in if_input_was_assigned()
	AppCentral::instance()->setInputAssignMode(m_originFxScene);
	connect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
}

void SceneDeskWidget::on_tubeCommentEdit_textEdited(const QString &arg1)
{
	setLabelInSelectedTubes(arg1);
}


SceneDeskWidget * SceneDeskWidget::openSceneDesk(FxSceneItem *scene, QWidget *parent)
{
	foreach(SceneDeskWidget *desk, m_sceneDeskList) {
		if (desk->m_originFxScene == scene) {
			desk->setWindowState(desk->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
			desk->raise();
			desk->show();
			return 0;
		}
	}

	SceneDeskWidget *scenedesk = new SceneDeskWidget(scene,parent);
	m_sceneDeskList.append( scenedesk );

	scenedesk->setWindowIcon(QPixmap(":/gfx/icons/scene_mixer.png"));
	return scenedesk;
}

void SceneDeskWidget::destroyAllSceneDesks()
{
	while (!m_sceneDeskList.isEmpty()) {
		delete m_sceneDeskList.takeFirst();
	}
}

void SceneDeskWidget::onChannelCountSpinClickedAndChanged(int arg1)
{
	if (!m_originFxScene) return;
	m_originFxScene->setTubeCount(arg1);
	setFxScene(m_originFxScene);
}

void SceneDeskWidget::onChannelCountSpinEditingFinished()
{
	if (!m_originFxScene) return;
	if (channelCountSpin->value() != m_originFxScene->tubeCount()) {
		m_originFxScene->setTubeCount(channelCountSpin->value());
		setFxScene(m_originFxScene);
	}
}

void SceneDeskWidget::on_cloneCurrentInputButton_clicked()
{
	if (!m_originFxScene) return;

	AppCentral::ref().unitLight->fillSceneFromInputUniverses(m_originFxScene);

	setFxScene(m_originFxScene);
}

void SceneDeskWidget::on_cloneCurrentOutputButton_clicked()
{
	if (!m_originFxScene) return;

}


void SceneDeskWidget::on_universeSpin_valueChanged(int arg1)
{
	setUniverseInTubes(arg1 - 1);
}
