#include "scenedeskwidget.h"
#include "log.h"
#include "fxsceneitem.h"
#include "dmxchannel.h"
#include "customwidget/mixergroup.h"
#include "customwidget/mixerchannel.h"


SceneDeskWidget::SceneDeskWidget(QWidget *parent) :
	QWidget(parent)
{
	cur_fx = 0;
	cur_fxscene = 0;
	scene_is_live_f = false;
	setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
}

SceneDeskWidget::SceneDeskWidget(FxSceneItem *scene, QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	setFxScene(scene);

	setAttribute(Qt::WA_DeleteOnClose);
}

SceneDeskWidget::~SceneDeskWidget()
{
	DEBUGTEXT("Desk destroyed");
}

bool SceneDeskWidget::setFxScene(FxSceneItem *scene)
{
	faderWidget->clear();

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
			fader->setRange(0,dmx->targetFullValue);
			fader->setValue(dmx->targetValue);

			// and of course we need a slot to react on slider movement
			connect(fader,SIGNAL(mixerMoved(int,int)),this,SLOT(set_mixer_val_on_moved(int,int)));
		}
	}

	return true;
}

void SceneDeskWidget::closeEvent(QCloseEvent *)
{
	on_liveCheck_clicked(false);
	DEBUGTEXT("Desk CloseEvent");

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


	if (checked) {
		for (int t=0; t<cur_fxscene->tubeCount(); t++) {
			DmxChannel *tube = cur_fxscene->tubes.at(t);
			tube->curValue = tube->targetValue;
		}
	} else {
		for (int t=0; t<cur_fxscene->tubeCount(); t++) {
			DmxChannel *tube = cur_fxscene->tubes.at(t);
			tube->curValue = 0;
		}
	}

}
