#ifndef FXLISTWIDGET_H
#define FXLISTWIDGET_H

#include "ui_fxlistwidget.h"
#include "system/commandsystem.h"

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
	FxList * myfxlist;

public:
	FxListWidget(QWidget *parent = 0);

	void setFxList(FxList *fxlist);

private:
	void init();

private slots:
	void on_fxTable_itemClicked(QTableWidgetItem *item);

	void on_fxTable_itemDoubleClicked(QTableWidgetItem *item);

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);
	void fxItemSelected(FxItem *);

};

#endif // FXLISTWIDGET_H
