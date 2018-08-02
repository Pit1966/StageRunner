#include "scenestatuswidget.h"
#include "fxsceneitem.h"

#include <QWriteLocker>
#include <QPainter>

#define READLOCK QReadLocker(*rwlock);Q_UNUSED(rwlock);
#define WRITELOCK QWriteLocker(*rwlock);Q_UNUSED(rwlock);

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

	if (scene_hash.contains(scene))
		return updateScene(scene);

	SceneStatusListItem *item = new SceneStatusListItem(scene,sceneListWidget);
	item->setText(scene->name());

	scene_hash.insert(scene,item);

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
			item->setBackgroundColor(palette().base().color());
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
			item->setBackgroundColor(Qt::green);
		}
		else if (flags & (SCENE_STAGE_INTERN | SCENE_STAGE_EXTERN)) {
			item->setBackgroundColor(palette().base().color());
		}
		else if (flags & SCENE_STAGE_LIVE) {
			item->setBackgroundColor(Qt::red);
		}
		else {
			item->setBackgroundColor(Qt::yellow);
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
			removeOrDeactivateScene(scene);
			return false;
		} else {
			updateScene(scene);
			return true;
		}
		return false;
	} else {
		if (scene->isVisible()) {
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
