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


SceneDeskWidget::SceneDeskWidget(QWidget *parent) :
	QWidget(parent)
{
	cur_fx = 0;
	cur_fxscene = 0;
	init ();
	setupUi(this);
	init_gui();
}

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
	DEBUGTEXT("Desk destroyed");
}

void SceneDeskWidget::init_gui()
{
	setAttribute(Qt::WA_DeleteOnClose);

	connect(AppCentral::instance()->unitLight,SIGNAL(outputUniverseChanged(int,QByteArray)),this,SLOT(notifyChangedUniverse(int,QByteArray)));
	connect(faderAreaWidget,SIGNAL(mixerSelected(bool,int)),this,SLOT(setTubeSelected(bool,int)));
	connect(faderAreaWidget,SIGNAL(mixerDraged(int,int)),this,SLOT(if_mixerDraged(int,int)));
}



bool SceneDeskWidget::setFxScene(FxSceneItem *scene)
{
	setSceneEditable(false);
	selected_tube_ids.clear();
	faderAreaWidget->clear();

	QByteArray (&dmxout)[MAX_DMX_UNIVERSE] = AppCentral::instance()->unitLight->dmxOutputValues;

	// First we save the pointer to the scene;
	cur_fx = scene;
	cur_fxscene = scene;

	if (!FxItem::exists(scene)) return false;

	QList<DmxChannel*>sort;
	for (int t=0; t<scene->tubes.size(); t++) {
		sort.append(0);
	}

	for (int t=0; t<scene->tubes.size(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		if (tube->deskPositionIndex >= 0) {
			tube->tempTubeListIdx = t;
			sort[tube->deskPositionIndex] = tube;
		}
	}

	int posindex = 0;
	for (int t=0; t<scene->tubes.size(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		if (tube->deskPositionIndex < 0) {
			tube->tempTubeListIdx = t;
			while (sort.at(posindex) != 0) {
				posindex++;
			}
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
			dmx->tempDeskPosIdx = t;
			fader->setDmxId(dmx->dmxUniverse,dmx->dmxChannel);
			fader->setRange(0,dmx->targetFullValue);
			fader->setValue(dmx->targetValue);
			fader->setChannelShown(true);

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
	sceneNameEdit->setText(cur_fxscene->name());
	faderGroup->setTitle(cur_fxscene->name());
	hookedUniverseSpin->setValue(cur_fxscene->hookedUniverse()+1);
	hookedChannelSpin->setValue(cur_fxscene->hookedChannel()+1);
	fadeInTimeEdit->setText(QtStaticTools::msToTimeString(cur_fxscene->defaultFadeInTime));
	fadeOutTimeEdit->setText(QtStaticTools::msToTimeString(cur_fxscene->defaultFadeOutTime));

	return true;
}

void SceneDeskWidget::setControlKey(bool state)
{
	ctrl_pressed_f = state;
	faderAreaWidget->setMultiSelectEnabled(ctrl_pressed_f | shift_pressed_f);
}

void SceneDeskWidget::setShiftKey(bool state)
{
	shift_pressed_f = state;
	faderAreaWidget->setMultiSelectEnabled(ctrl_pressed_f | shift_pressed_f);
}

DmxChannel *SceneDeskWidget::getTubeFromMixer(const MixerChannel *mixer) const
{
	for (int t=0; t<cur_fxscene->tubeCount(); t++) {
		DmxChannel *dmx = cur_fxscene->tubes.at(t);
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

	if (!cur_fxscene) return tube;

	// Now get tube for MixerChannel Id in scene Tube list
	if (mixer->id() >= 0 && mixer->id() < cur_fxscene->tubes.size()) {
		tube = cur_fxscene->tubes.at(mixer->id());
	}
	return tube;
}

void SceneDeskWidget::setTubeSelected(bool state, int id)
{
	if (!cur_fxscene) return;

	if (state) {
		if (!selected_tube_ids.contains(id)) {
			if (id >=0 && id < cur_fxscene->tubeCount()) {
				selected_tube_ids.append(id);
			}
		}
		copyTubeSettingsToGui(id);
	} else {
		selected_tube_ids.removeAll(id);
		copyTubeSettingsToGui(-1);
	}
	// qDebug() << "tubes selected:" << selected_tube_ids;
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
	if (!FxItem::exists(cur_fxscene)) return;
	if (cur_fxscene->isLive() == state) return;

	cur_fxscene->setLive(state);

	if (state) {
		faderAreaWidget->setRefSliderColorIndex(1);
		for (int t=0; t<cur_fxscene->tubeCount(); t++) {
			DmxChannel *tube = cur_fxscene->tubes.at(t);
			tube->curValue = tube->targetValue;
		}
		AppCentral::instance()->unitLight->setSceneActive(cur_fxscene);

	} else {
		faderAreaWidget->setRefSliderColorIndex(0);
		if (!cur_fxscene->isOnStage()) {
			for (int t=0; t<cur_fxscene->tubeCount(); t++) {
				DmxChannel *tube = cur_fxscene->tubes.at(t);
				tube->curValue = 0;
			}
		}
	}
}

void SceneDeskWidget::copyTubeSettingsToGui(int id)
{
	if (!FxItem::exists(cur_fxscene)) return;

	DmxChannel *tube = cur_fxscene->tube(id);
	if (tube) {
		tubeCommentEdit->setText(tube->labelText);
	} else {
		tubeCommentEdit->clear();
	}
}

void SceneDeskWidget::if_input_was_assigned(FxItem *fx)
{
	disconnect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
	if (fx == cur_fxscene) {
		hookedUniverseSpin->setValue(cur_fxscene->hookedUniverse()+1);
		hookedChannelSpin->setValue(cur_fxscene->hookedChannel()+1);
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
	DEBUGTEXT("Close scene desk");

}


void SceneDeskWidget::notifyChangedUniverse(int universe, const QByteArray &dmxValues)
{
	faderAreaWidget->notifyChangedDmxUniverse(universe,dmxValues);

	if (!FxItem::exists(cur_fxscene)) return;

//	for (int t=0;t<cur_fxscene->tubeCount(); t++) {
//		DmxChannel *dmx = cur_fxscene->tubes.at(t);
//		if (dmx->dmxUniverse == universe) {
//			emit dmxValueWantsUpdate(universe,dmx->dmxChannel,quint8(dmxValues[dmx->dmxChannel]));
//		}
//	}
}

void SceneDeskWidget::set_mixer_val_on_moved(int val, int id)
{
	if (!FxItem::exists(cur_fxscene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	cur_fxscene->tubes.at(id)->targetValue = val;
	if (scene_is_live_f) {
		cur_fxscene->tubes.at(id)->curValue = val;
	}
}

void SceneDeskWidget::on_liveCheck_clicked(bool checked)
{
	scene_is_live_f = checked;

	if (!FxItem::exists(cur_fxscene)) {
		DEBUGERROR("Scene in Desk does not exist anymore!");
		return;
	}

	setCurrentSceneLiveState(checked);
	setSceneEditable(checked);
}

void SceneDeskWidget::on_hookedUniverseSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxscene)) return;

	cur_fxscene->hookToUniverse(arg1-1);

}

void SceneDeskWidget::on_hookedChannelSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxscene)) return;

	cur_fxscene->hookToChannel(arg1-1);

}

void SceneDeskWidget::if_mixerDraged (int fromIdx, int toIdx)
{
	if (!FxItem::exists(cur_fxscene)) return;

	qDebug("Mixer moved from position %d to %d",fromIdx,toIdx);
	DmxChannel *from_tube = 0;
	DmxChannel *to_tube = 0;
	for (int t=0; t<cur_fxscene->tubeCount(); t++) {
		DmxChannel *tube = cur_fxscene->tubes.at(t);
		if (tube->tempDeskPosIdx == fromIdx) {
			from_tube = tube;
		}
		if (tube->tempDeskPosIdx == toIdx) {
			to_tube = tube;
		}
	}

	if (from_tube && to_tube) {
		from_tube->tempDeskPosIdx = toIdx;
		if (from_tube->tempTubeListIdx == toIdx) {
			from_tube->deskPositionIndex = -1;
		} else {
			from_tube->deskPositionIndex = toIdx;
		}

		to_tube->tempDeskPosIdx = fromIdx;
		if (to_tube->tempTubeListIdx == fromIdx) {
			to_tube->deskPositionIndex = -1;
		} else {
			to_tube->deskPositionIndex = fromIdx;
		}\
		cur_fxscene->setModified(true);
	}

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
	if (!cur_fxscene || !tube) return false;

	tube->deskVisibleFlag = false;
	cur_fxscene->setModified(true);

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
	if (!cur_fxscene) return false;

	bool removed = false;

	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		removed |= hideTube( cur_fxscene->tubes.at(id), faderAreaWidget->getMixerById(id) );
	}
	selected_tube_ids.clear();

	return removed;
}

int SceneDeskWidget::setLabelInSelectedTubes(const QString &text)
{
	if (!cur_fxscene) return -1;

	int count = 0;

	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		MixerChannel *mix = faderAreaWidget->getMixerById(id);
		if (mix && id < cur_fxscene->tubeCount()) {
			mix->setLabelText(text);
			cur_fxscene->tubes.at(id)->labelText = text;
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
	act = menu.addAction(tr("Hide Channel(s)"));
	act->setObjectName("1");
	act = menu.addAction(tr("Add Channel"));
	act->setObjectName("2");

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
		cur_fxscene->setTubeCount(cur_fxscene->tubeCount()+1);
		setFxScene(cur_fxscene);
		emit modified();
		break;
	}
}


void SceneDeskWidget::on_fadeInTimeEdit_textEdited(const QString &arg1)
{
	if (!cur_fxscene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxscene->defaultFadeInTime != time_ms) {
		cur_fxscene->defaultFadeInTime = time_ms;
		cur_fxscene->setModified(true);
		emit modified();
	}

}

void SceneDeskWidget::on_fadeOutTimeEdit_textEdited(const QString &arg1)
{
	if (!cur_fxscene) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxscene->defaultFadeOutTime != time_ms) {
		cur_fxscene->defaultFadeOutTime = time_ms;
		cur_fxscene->setModified(true);
		emit modified();
	}
}

void SceneDeskWidget::on_editCheck_clicked(bool checked)
{
	setSceneEditable(checked);
}

void SceneDeskWidget::on_sceneNameEdit_textEdited(const QString &arg1)
{
	if (!cur_fxscene) return;

	cur_fxscene->setName(arg1);
	faderGroup->setTitle(arg1);
	cur_fxscene->setModified(true);
	emit modified();
}

void SceneDeskWidget::on_autoHookButton_clicked()
{
	if (!editCheck->isChecked()) {
		setSceneEditable(true);
	}

	// Set Input assign mode and wait for answer in if_input_was_assigned()
	AppCentral::instance()->setInputAssignMode(cur_fxscene);
	connect(AppCentral::instance(),SIGNAL(inputAssigned(FxItem*)),this,SLOT(if_input_was_assigned(FxItem*)));
}

void SceneDeskWidget::on_tubeCommentEdit_textEdited(const QString &arg1)
{
	setLabelInSelectedTubes(arg1);
}

