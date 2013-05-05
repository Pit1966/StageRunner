#ifndef SCENEDESKWIDGET_H
#define SCENEDESKWIDGET_H

#include "ui_scenedeskwidget.h"

class FxSceneItem;
class FxItem;

class SceneDeskWidget : public QWidget, private Ui::SceneDeskWidget
{
	Q_OBJECT
private:
	FxSceneItem *cur_fxscene;
	FxItem *cur_fx;

	bool scene_is_live_f;

public:
	SceneDeskWidget(QWidget *parent = 0);
	SceneDeskWidget(FxSceneItem *scene, QWidget *parent = 0);
	~SceneDeskWidget();
	bool setFxScene(FxSceneItem *scene);

private:
	void closeEvent(QCloseEvent *);

private slots:
	void set_mixer_val_on_moved(int val, int id);

	void on_liveCheck_clicked(bool checked);
};

#endif // SCENEDESKWIDGET_H
