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
	SceneStatusListItem(FxSceneItem *scene, QListWidget *list = 0);

};



class SceneStatusWidget : public QWidget, public Ui::SceneStatusWidget
{
	Q_OBJECT
private:
	QReadWriteLock *rwlock;
	QHash<FxSceneItem*,SceneStatusListItem*>scene_hash;

public:
	SceneStatusWidget(QWidget *parent = 0);
	~SceneStatusWidget();

	bool appendScene(FxSceneItem *scene);
	bool removeScene(FxSceneItem *scene);
	bool updateScene(FxSceneItem *scene);

public slots:
	bool propagateScene(FxSceneItem *scene);
	void propagateSceneFade(FxSceneItem *scene, int perMilleA, int perMilleB);
};

#endif // SCENESTATUSWIDGET_H
