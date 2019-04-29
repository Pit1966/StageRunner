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

#ifndef SCENEDESKWIDGET_H
#define SCENEDESKWIDGET_H

#include "ui_scenedeskwidget.h"

#include "dmxtypes.h"

class FxSceneItem;
class FxItem;
class DmxChannel;

class SceneDeskWidget : public QWidget, private Ui::SceneDeskWidget
{
	Q_OBJECT
private:
	static QList<SceneDeskWidget*>scene_desk_list;		//
	FxSceneItem *origin_fxscene;

	QList<int>selected_tube_ids;		//List of tube Ids currently selected in MixerGroup

	bool scene_is_live_f;
	bool ctrl_pressed_f;
	bool shift_pressed_f;

public:
	static SceneDeskWidget *openSceneDesk(FxSceneItem *scene, QWidget *parent = 0);
	static void destroyAllSceneDesks();

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
	bool setTypeOfSelectedTubes(DmxChannelType type);
	bool deleteSelectedTubes();
	bool unhideAllTubes();
	int setLabelInSelectedTubes(const QString &text);

private:
	SceneDeskWidget(FxSceneItem *scene, QWidget *parent = 0);
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
	void on_channelCountSpin_valueChanged(int arg1);
	void on_channelCountSpin_editingFinished();

	void on_cloneCurrentInputButton_clicked();

	void on_cloneCurrentOutputButton_clicked();

signals:
	void dmxValueWantsUpdate(int universe, int dmxchannel, int dmxval);
	void modified();

};

#endif // SCENEDESKWIDGET_H
