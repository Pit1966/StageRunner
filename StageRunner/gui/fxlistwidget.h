#ifndef FXLISTWIDGET_H
#define FXLISTWIDGET_H

#include "ui_fxlistwidget.h"
#include "system/commandsystem.h"

#include <QTableWidgetItem>
#include <QTableWidget>

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
	FxItem * cur_selected_item;

public:
	FxListWidget(QWidget *parent = 0);
	void setFxList(FxList *fxlist);
	inline FxList * fxList() const {return myfxlist;}
	void setAutoProceedSequence(bool state);

public slots:
	void selectFx(FxItem *fx);


private:
	void init();

public slots:
	void refreshList();

private slots:
	void on_fxTable_itemClicked(QTableWidgetItem *item);
	void on_fxTable_itemDoubleClicked(QTableWidgetItem *item);
	void moveRowFromTo(int srcrow, int destrow);
	void on_autoProceedCheck_clicked(bool checked);

	void on_fxTable_itemChanged(QTableWidgetItem *item);

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);
	void fxItemSelected(FxItem *);
	void dropEventReceived(QString text, int row);
	void listModified();

};

#endif // FXLISTWIDGET_H
