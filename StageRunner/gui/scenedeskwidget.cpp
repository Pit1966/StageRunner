#include "scenedeskwidget.h"
#include "log.h"
#include "fxsceneitem.h"
#include "dmxchannel.h"
#include "appcentral.h"
#include "lightcontrol.h"
#include "customwidget/mixergroup.h"
#include "customwidget/mixerchannel.h"


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
}


bool SceneDeskWidget::setFxScene(FxSceneItem *scene)
{
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

void SceneDeskWidget::init()
{
	scene_is_live_f = false;
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
