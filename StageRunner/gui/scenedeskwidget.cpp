#include "scenedeskwidget.h"
#include "log.h"
#include "fxsceneitem.h"
#include "dmxchannel.h"
#include "appcentral.h"
#include "lightcontrol.h"
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
	connect(faderWidget,SIGNAL(mixerSelected(bool,int)),this,SLOT(setTubeSelected(bool,int)));
}



bool SceneDeskWidget::setFxScene(FxSceneItem *scene)
{
	selected_tube_ids.clear();
	faderWidget->clear();
	QByteArray (&dmxout)[MAX_DMX_UNIVERSE] = AppCentral::instance()->unitLight->dmxOutputValues;

	// First we save the pointer to the scene;
	cur_fx = scene;
	cur_fxscene = scene;

	if (!FxItem::exists(scene)) return false;

	for (int t=0; t<scene->tubes.size(); t++) {
		DmxChannel *dmx = scene->tubes.at(t);
		// We create a new Mixer in the MixerGroup if this tube is visible
		if (dmx->deskVisibleFlag) {
			// Create a new Mixer an fill in relevant data
			MixerChannel *fader = faderWidget->appendMixer();
			// We will need the Id to determine which DmxChannel an incoming fader signal belongs to
			// We have to keep the Id of the mixer channel in sync with Tubes list index!
			fader->setId(t);
			fader->setDmxId(dmx->dmxUniverse,dmx->dmxChannel);
			fader->setRange(0,dmx->targetFullValue);
			fader->setValue(dmx->targetValue);
			fader->setChannelShown(true);

			int ref_val = quint8(dmxout[dmx->dmxUniverse][dmx->dmxChannel]);
			ref_val = ref_val * dmx->targetFullValue / 255;
			fader->setRefValue(ref_val);

			// and of course we need a slot to react on slider movement
			connect(fader,SIGNAL(mixerMoved(int,int)),this,SLOT(set_mixer_val_on_moved(int,int)));
			// and we have to update the indicators for outside DMX level changes
			// connect(this,SIGNAL(dmxValueWantsUpdate(int,int,int)),fader,SLOT(notifyChangedDmxChannel(int,int,int)));
		}
	}

	// Set additional information widgets;
	hookedUniverseSpin->setValue(cur_fxscene->hookedToInputUniverse+1);
	hookedChannelSpin->setValue(cur_fxscene->hookedToInputDmxChannel+1);


	return true;
}

void SceneDeskWidget::setControlKey(bool state)
{
	ctrl_pressed_f = state;
	faderWidget->setMultiSelectEnabled(ctrl_pressed_f | shift_pressed_f);
}

void SceneDeskWidget::setShiftKey(bool state)
{
	shift_pressed_f = state;
	faderWidget->setMultiSelectEnabled(ctrl_pressed_f | shift_pressed_f);
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
	MixerChannel *mixer = faderWidget->findMixerAtPos(faderWidget->mapFrom(this,pos));
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
	} else {
		selected_tube_ids.removeAll(id);
	}
	qDebug() << "tubes selected:" << selected_tube_ids;
}

void SceneDeskWidget::init()
{
	scene_is_live_f = false;
	ctrl_pressed_f = false;
	shift_pressed_f = false;

}

void SceneDeskWidget::closeEvent(QCloseEvent *)
{
	on_liveCheck_clicked(false);
	DEBUGTEXT("Desk CloseEvent");

}


void SceneDeskWidget::notifyChangedUniverse(int universe, const QByteArray &dmxValues)
{
	faderWidget->notifyChangedDmxUniverse(universe,dmxValues);

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


	cur_fxscene->setLive(checked);

	if (checked) {
		faderWidget->setRefSliderColorIndex(1);
		for (int t=0; t<cur_fxscene->tubeCount(); t++) {
			DmxChannel *tube = cur_fxscene->tubes.at(t);
			tube->curValue = tube->targetValue;
		}
		AppCentral::instance()->unitLight->setSceneActive(cur_fxscene);

	} else {
		faderWidget->setRefSliderColorIndex(0);
		if (!cur_fxscene->isOnStage()) {
			for (int t=0; t<cur_fxscene->tubeCount(); t++) {
				DmxChannel *tube = cur_fxscene->tubes.at(t);
				tube->curValue = 0;
			}
		}
	}

}

void SceneDeskWidget::on_hookedUniverseSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxscene)) return;

	cur_fxscene->hookedToInputUniverse = arg1-1;


}

void SceneDeskWidget::on_hookedChannelSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxscene)) return;

	cur_fxscene->hookedToInputDmxChannel = arg1-1;

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
		return faderWidget->removeMixer(mixer);
	}
	return false;
}

bool SceneDeskWidget::hideSelectedTubes()
{
	if (!cur_fxscene) return false;

	bool removed = false;

	for (int t=0; t<selected_tube_ids.size(); t++) {
		int id = selected_tube_ids.at(t);
		removed |= hideTube( cur_fxscene->tubes.at(id), faderWidget->getMixerById(id) );
	}
	selected_tube_ids.clear();

	return removed;
}



void SceneDeskWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QRect fadergroup(faderWidget->mapToGlobal(faderWidget->pos()),faderWidget->size());
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
		break;
	}
}

