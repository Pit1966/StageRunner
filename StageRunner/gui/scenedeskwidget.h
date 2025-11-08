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
#include "tubedata.h"

class FxSceneItem;
class FxItem;
class DmxChannel;

class SceneDeskWidget : public QWidget, private Ui::SceneDeskWidget
{
	Q_OBJECT
private:
	static QList<SceneDeskWidget*>m_sceneDeskList;

	FxSceneItem *m_originFxScene	= nullptr;
	FxSceneItem *m_backupScene		= nullptr;	// Backup copy of the m_originFxScene (for cancel function)
	QList<int>m_selectedTubeIds;				//List of tube Ids currently selected in MixerGroup

	bool m_isSceneLive		= false;
	bool m_isCtrlPressed	= false;
	bool m_isShiftPressed	= false;
	bool m_closeClicked		= false;
	bool m_orgWasModified	= false;

public:
	static SceneDeskWidget *openSceneDesk(FxSceneItem *scene, QWidget *parent = 0);
	static void destroyAllSceneDesks();

	~SceneDeskWidget();
	bool setFxScene(const FxSceneItem *scene);
	void setControlKey(bool state);
	void setShiftKey(bool state);
	DmxChannel * getTubeFromMixer(const MixerChannel * mixer) const;
	DmxChannel * getTubeAtPos(QPoint pos, MixerChannel **dmxChannel = 0);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	bool hideTube(DmxChannel *tube, MixerChannel *mixer);
	bool setDmxTypeForTube(DmxChannel *tube, MixerChannel *mixer);
	bool hideSelectedTubes();
	bool setTypeOfSelectedTubes(DmxChannelType type);
	bool deleteSelectedTubes();
	bool unhideAllTubes();
	int setLabelInSelectedTubes(const QString &text);
	int setUniverseInTubes(int universe);

	// Helper
	// DmxChannel *getSingleSelectedDmxChannelAtPos()

private:
	SceneDeskWidget(FxSceneItem *scene, QWidget *parent = 0);
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
	void onChannelCountSpinClickedAndChanged(int arg1);
	void onChannelCountSpinEditingFinished();

	void on_cloneCurrentInputButton_clicked();
	void on_cloneCurrentOutputButton_clicked();
	void on_universeSpin_valueChanged(int arg1);

	void on_closeButton_clicked();

signals:
	// void dmxValueWantsUpdate(int universe, int dmxchannel, int dmxval);
	void modified();
	void dmxPositionTubeChanged(const TubeData &tubeDat);
};

#endif // SCENEDESKWIDGET_H
