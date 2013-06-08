#ifndef FXLISTWIDGET_H
#define FXLISTWIDGET_H

#include "ui_fxlistwidget.h"
#include "system/commandsystem.h"

#include <QTableWidgetItem>
#include <QTableWidget>

class FxList;
class FxItem;
class FxSceneItem;
class FxListWidgetItem;


class FxListWidget : public QWidget, private Ui::FxListWidget
{
	Q_OBJECT

private:
	bool is_modified_f;
	bool is_editable_f;
	FxList * myfxlist;
	FxItem * cur_selected_item;
	QList<int>selected_rows;

public:
	FxListWidget(QWidget *parent = 0);
	void setFxList(FxList *fxlist);
	inline FxList * fxList() const {return myfxlist;}
	void setAutoProceedSequence(bool state);
	inline bool isEditable() {return is_editable_f;}
	QList<FxListWidgetItem*> getItemListForRow(int row);

private:
	void init();
	void open_scence_desk(FxSceneItem *fx);
	FxListWidgetItem *new_fxlistwidgetitem(FxItem *fx, const QString & text, int coltype);
	void column_name_double_clicked(FxItem *fx);
	void column_type_double_clicked(FxItem *fx);

public slots:
	void selectFx(FxItem *fx);
	void initRowDrag(FxListWidgetItem *item);
	void refreshList();
	void setEditable(bool state);
	void setRowSelected(int row, bool state);
	void setSingleRowSelected(int row);
	void unselectRows();

private slots:
	void on_fxTable_itemClicked(QTableWidgetItem *item);
	void on_fxTable_itemDoubleClicked(QTableWidgetItem *item);
	void moveRowFromTo(int srcrow, int destrow);
	void on_autoProceedCheck_clicked(bool checked);

	void on_fxTable_itemChanged(QTableWidgetItem *item);
	void if_fxitemwidget_clicked(FxListWidgetItem *listitem);
	void if_fxitemwidget_doubleclicked(FxListWidgetItem *listitem);
	void if_fxitemwidget_edited(FxListWidgetItem *listitem, const QString &text);

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);
	void fxItemSelected(FxItem *);
	void dropEventReceived(QString text, int row);
	void listModified();
	void editableChanged(bool state);
};

#endif // FXLISTWIDGET_H
