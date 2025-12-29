//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef SCENESTATUSWIDGET_H
#define SCENESTATUSWIDGET_H

#include "ui_scenestatuswidget.h"
#include <QReadWriteLock>
#include <QHash>
#include <QListWidget>

class FxSceneItem;

class SceneStatusListItem : public QListWidgetItem
{
protected:
	FxSceneItem *myScene;

public:
	SceneStatusListItem(FxSceneItem *scene, QListWidget *list = nullptr);

};



class SceneStatusWidget : public QWidget, public Ui::SceneStatusWidget
{
	Q_OBJECT
private:
	bool m_doNotRemove;
	QReadWriteLock *rwlock;
	QHash<FxSceneItem*,SceneStatusListItem*>scene_hash;

public:
	SceneStatusWidget(QWidget *parent = nullptr);
	~SceneStatusWidget();

	bool appendScene(FxSceneItem *scene);
	bool updateScene(FxSceneItem *scene);
	bool removeOrDeactivateScene(FxSceneItem *scene);

public slots:
	void removeScene(FxSceneItem *scene);
	bool propagateScene(FxSceneItem *scene);
	void propagateSceneFade(FxSceneItem *scene, int perMilleA, int perMilleB);
private slots:
	void on_showInactiveCheck_clicked(bool checked);
};

#endif // SCENESTATUSWIDGET_H
