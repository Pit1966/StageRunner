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
