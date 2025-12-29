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

#include "scenestatuswidget.h"
#include "fxsceneitem.h"

#include <QWriteLocker>
#include <QPainter>

#define READLOCK QReadLocker lock(rwlock);Q_UNUSED(lock);
#define WRITELOCK QWriteLocker lock(rwlock);Q_UNUSED(lock);
#define UNLOCK lock.unlock();

SceneStatusWidget::SceneStatusWidget(QWidget *parent)
	: QWidget(parent)
	, m_doNotRemove(false)
{
	rwlock = new QReadWriteLock(QReadWriteLock::Recursive);
	setupUi(this);

	setObjectName("SceneStatus");
}

SceneStatusWidget::~SceneStatusWidget()
{
	delete rwlock;
}

bool SceneStatusWidget::appendScene(FxSceneItem *scene)
{
	WRITELOCK;

	if (!scene_hash.contains(scene)) {

		SceneStatusListItem *item = new SceneStatusListItem(scene,sceneListWidget);
		item->setText(scene->name());

		scene_hash.insert(scene,item);
	}

	UNLOCK;

	updateScene(scene);
	return true;
}

bool SceneStatusWidget::removeOrDeactivateScene(FxSceneItem *scene)
{
	bool removed = false;

	WRITELOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item) {
		if (m_doNotRemove) {
			item->setBackground(palette().base().color());
			item->setForeground(Qt::darkGray);
			item->setIcon(QPixmap());
		} else {
			delete item;
			removed = true;
			scene_hash.remove(scene);
		}
	} else {
		scene_hash.remove(scene);
	}

	return removed;
}

void SceneStatusWidget::removeScene(FxSceneItem *scene)
{
	WRITELOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item)
		delete item;

	scene_hash.remove(scene);
}

bool SceneStatusWidget::updateScene(FxSceneItem *scene)
{
	READLOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item && scene) {
		quint32 flags = scene->status();
		if (flags & (SCENE_ACTIVE_INTERN | SCENE_ACTIVE_EXTERN)) {
			item->setBackground(Qt::green);
		}
		else if (flags & (SCENE_STAGE_INTERN | SCENE_STAGE_EXTERN)) {
			item->setBackground(palette().base().color());
		}
		else if (flags & SCENE_STAGE_LIVE) {
			item->setBackground(Qt::red);
		}
		else {
			item->setBackground(Qt::yellow);
		}
		return true;
	}

	return false;
}



bool SceneStatusWidget::propagateScene(FxSceneItem *scene)
{
	READLOCK;

	if (scene_hash.contains(scene)) {
		// Remove the scene from list if it is not on stage, live or active
		// Otherwise update the Color in the list view
		if (!scene->isVisible()) {
			UNLOCK;
			removeOrDeactivateScene(scene);
			return false;
		} else {
//			UNLOCK;
			updateScene(scene);
			return true;
		}
		return false;
	} else {
		if (scene->isVisible()) {
			UNLOCK;
			return appendScene(scene);
		} else {
			return false;
		}
	}
}

void SceneStatusWidget::propagateSceneFade(FxSceneItem *scene, int perMilleA, int perMilleB)
{
	Q_UNUSED(perMilleB);

	READLOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item && scene) {
		int sx = 14;
		int sy = 13;
		int target = (perMilleA * sx * sy + 5) / 1000;
		QPixmap pix(sx,sy);
		pix.fill(Qt::black);
		QPainter p(&pix);
		p.setPen(QColor(Qt::yellow));
		int c = 0;
		int y = 0;
		while (c < target) {
			if (c + sx - 1 < target) {
				p.drawLine(0,y,sx-1,y);
			} else {
				p.drawLine(0,y,target%sx,y);
			}
			c += sx;
			y++;
		}
		item->setIcon(pix);
	}
}


SceneStatusListItem::SceneStatusListItem(FxSceneItem *scene, QListWidget *list)
	: QListWidgetItem(list)
	, myScene(scene)
{
}

void SceneStatusWidget::on_showInactiveCheck_clicked(bool checked)
{
	m_doNotRemove = checked;
	if (!checked) {
		foreach (FxSceneItem *scene, scene_hash.keys()) {
			if (FxItem::exists(scene)) {
				if (!scene->isVisible()) {
					delete scene_hash.value(scene);
					scene_hash.remove(scene);
				}
			} else {
				scene_hash.remove(scene);
			}
		}
	}
}
