#ifndef FXLISTWIDGET_H
#define FXLISTWIDGET_H

#include "ui_fxlistwidget.h"
#include <QTableWidgetItem>

class FxList;
class FxItem;

class FxListWidgetItem : public QTableWidgetItem
{
public:
	enum ColumnType {
		CT_UNDEF,
		CT_NAME,
		CT_ID
	};

	FxItem *linkedFxItem;
	ColumnType columnType;

public:
	FxListWidgetItem(FxItem *fxitem, const QString &text);

};



class FxListWidget : public QWidget, private Ui::FxListWidget
{
	Q_OBJECT

private:
	bool is_modified_f;


public:
	FxListWidget(QWidget *parent = 0);

	void setFxList(const FxList *fxlist);

private slots:
	void on_fxTable_itemClicked(QTableWidgetItem *item);

signals:
	void fxCmdActivated(FxItem *, int);

};

#endif // FXLISTWIDGET_H
