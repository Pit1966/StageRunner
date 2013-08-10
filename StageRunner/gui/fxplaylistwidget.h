#ifndef FXPLAYLISTWIDGET_H
#define FXPLAYLISTWIDGET_H

#include "ui_fxplaylistwidget.h"

class FxPlayListItem;

class FxPlayListWidget : public QWidget, private Ui::FxPlayListWidget
{
	Q_OBJECT

private:
	FxPlayListItem * playlist_item;

public:
	FxPlayListWidget(FxPlayListItem * play_list_item, QWidget *parent = 0);
	void setFxPlayListItem(FxPlayListItem *fxplay);

private:
	void closeEvent(QCloseEvent *);


private slots:
	void on_closeButton_clicked();


};

#endif // FXPLAYLISTWIDGET_H
