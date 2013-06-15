#ifndef SCENEDESKWIDGET_H
#define SCENEDESKWIDGET_H

#include "ui_scenedeskwidget.h"

class FxSceneItem;
class FxItem;
class DmxChannel;

class SceneDeskWidget : public QWidget, private Ui::SceneDeskWidget
{
	Q_OBJECT
private:
	FxSceneItem *cur_fxscene;
	FxItem *cur_fx;

	QList<int>selected_tube_ids;		//List of tube Ids currently selected in MixerGroup

	bool scene_is_live_f;
	bool ctrl_pressed_f;
	bool shift_pressed_f;

public:
	SceneDeskWidget(QWidget *parent = 0);
	SceneDeskWidget(FxSceneItem *scene, QWidget *parent = 0);
	~SceneDeskWidget();
	bool setFxScene(FxSceneItem *scene);
	void setControlKey(bool state);
	void setShiftKey(bool state);
	DmxChannel * getTubeFromMixer(const MixerChannel * mixer) const;
	DmxChannel * getTubeAtPos(QPoint pos, MixerChannel **dmxChannel = 0);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	bool hideTube(DmxChannel *tube, MixerChannel *mixer);
	bool hideSelectedTubes();
	int setLabelInSelectedTubes(const QString &text);

private:
	void init();
	void init_gui();
	void closeEvent(QCloseEvent *);

public slots:
	void notifyChangedUniverse(int universe, const QByteArray & dmxValues);
	void setTubeSelected(bool state, int id);
	void setSceneEditable(bool state);
	void setCurrentSceneLiveState(bool state);
	void copyTubeSettingsToGui(int id);

private slots:
	void if_input_was_assigned(FxItem *fx);
	void set_mixer_val_on_moved(int val, int id);
	void on_liveCheck_clicked(bool checked);
	void on_hookedUniverseSpin_valueChanged(int arg1);
	void on_hookedChannelSpin_valueChanged(int arg1);
	void if_mixerDraged(int fromIdx, int toIdx);
	void on_fadeInTimeEdit_textEdited(const QString &arg1);
	void on_fadeOutTimeEdit_textEdited(const QString &arg1);
	void on_editCheck_clicked(bool checked);
	void on_sceneNameEdit_textEdited(const QString &arg1);
	void on_autoHookButton_clicked();

	void on_tubeCommentEdit_textEdited(const QString &arg1);

signals:
	void dmxValueWantsUpdate(int universe, int dmxchannel, int dmxval);
	void modified();

};

#endif // SCENEDESKWIDGET_H
