#include "scenestatuswidget.h"
#include "fxsceneitem.h"

#include <QWriteLocker>

#define READLOCK QReadLocker(*rwlock);Q_UNUSED(rwlock);
#define WRITELOCK QWriteLocker(*rwlock);Q_UNUSED(rwlock);

SceneStatusWidget::SceneStatusWidget(QWidget *parent) :
	QWidget(parent)
{
	rwlock = new QReadWriteLock(QReadWriteLock::Recursive);
	setupUi(this);
}

SceneStatusWidget::~SceneStatusWidget()
{
	delete rwlock;
}

bool SceneStatusWidget::appendScene(FxSceneItem *scene)
{
	WRITELOCK;

	SceneStatusListItem *item = new SceneStatusListItem(scene,sceneListWidget);
	item->setText(scene->name());
	scene_hash.insert(scene,item);

	updateScene(scene);

	return true;
}

bool SceneStatusWidget::removeScene(FxSceneItem *scene)
{
	bool removed = false;

	WRITELOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item) {
		delete item;
		removed = true;
	}
	scene_hash.remove(scene);

	return removed;
}

bool SceneStatusWidget::updateScene(FxSceneItem *scene)
{
	READLOCK;

	SceneStatusListItem *item = scene_hash.value(scene);
	if (item && scene) {
		quint32 flags = scene->status();
		if (flags & SCENE_ACTIVE) {
			item->setBackgroundColor(Qt::green);
		}
		else if (flags & SCENE_STAGE) {
			item->setBackgroundColor(palette().base().color());
		}
		else if (flags & SCENE_LIVE) {
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
			removeScene(scene);
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


SceneStatusListItem::SceneStatusListItem(FxSceneItem *scene, QListWidget *list)
	: QListWidgetItem(list)
	, myScene(scene)
{
}
