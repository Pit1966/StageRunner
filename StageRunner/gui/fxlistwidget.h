#ifndef FXLISTWIDGET_H
#define FXLISTWIDGET_H

#include "ui_fxlistwidget.h"
#include "commandsystem.h"
#include "toolclasses.h"

#include <QTableWidgetItem>
#include <QTableWidget>

class FxList;
class FxItem;
class FxSceneItem;
class FxListWidgetItem;
class FxPlayListItem;
class FxSeqItem;
class Executer;

using namespace AUDIO;

typedef QList<FxListWidgetItem*> WidItemList;

class FxListWidget : public QWidget, private Ui::FxListWidget
{
	Q_OBJECT

private:
	bool is_modified_f;
	bool is_editable_f;
	bool is_standalone_f;

	FxList * myfxlist;
	FxItem * cur_selected_item;
	FxListWidgetItem* cur_clicked_item;
	FxItem * origin_fxitem;
	QList<int>selected_rows;
	static MutexQList<FxListWidget*>globalFxListWidgetList;

	int upper_context_menu_split;

public:
	FxListWidget(QWidget *parent = 0);
	~FxListWidget();
	void setFxList(FxList *fxlist);
	inline FxList * fxList() const {return myfxlist;}
	void setAutoProceedSequence(bool state);
	void setLoop(int loops);
	inline bool isEditable() {return is_editable_f;}
	QList<FxListWidgetItem*> getItemListForRow(int row);
	int getRowThatContainsFxItem(FxItem *fx);
	FxItem *currentSelectedFxItem() const {return cur_selected_item;}
	FxItem *getFxItemAtRow(int row) const;
	void setOriginFx(FxItem *fx);
	FxListWidgetItem * getFxListItemAtPos(QPoint pos);
	FxListWidgetItem * getFxListWidgetItemFor(FxItem *fx);
	void setStandAlone(bool state);
	inline bool isStandAlone() const {return is_standalone_f;}
	bool isFxItemPossibleHere(FxItem *fx);
	void generateDropAllowedFxTypeList(FxItem *fx);

	static FxListWidget * findFxListWidget(PTableWidget *tableWidget);
	static FxListWidget * findFxListWidget(FxList *fxList);
	static FxListWidget * getCreateFxListWidget(FxList *fxList, FxItem *fxItem, bool *created = 0);
	static FxListWidget * findParentFxListWidget(FxItem *fx);
	static bool destroyFxListWidget(FxListWidget *wid);
	static void destroyAllFxListWidgets();

private:
	void init();
	void closeEvent(QCloseEvent *);
	FxListWidgetItem *new_fxlistwidgetitem(FxItem *fx, const QString & text, int coltype);
	void column_name_double_clicked(FxItem *fx);
	void column_type_double_clicked(FxItem *fx);
	void contextMenuEvent(QContextMenuEvent *event);

public slots:
	void selectFx(FxItem *fx);
	void markFx(FxItem *fx);
	void setCurrentFx(FxItem *newfx, FxItem *oldfx);
	void initRowDrag(FxListWidgetItem *item);
	void refreshList();
	void setEditable(bool state);
	void setRowSelected(int row, bool state);
	void setSingleRowSelected(int row);
	void unselectRows();
	void propagateSceneStatus(FxSceneItem *scene);
	void propagateSceneFadeProgress(FxSceneItem *scene, int perMilleA, int perMilleB);
	void propagateAudioStatus(AudioCtrlMsg msg);
	void cloneRowFromPTable(PTableWidget *srcPtable, int srcRow, int destRow, bool removeSrc);
	void onFxItemSelectedInChildWidget(FxItem *fx);

private slots:
	void on_fxTable_itemClicked(QTableWidgetItem *item);
	void on_fxTable_itemDoubleClicked(QTableWidgetItem *item);
	void moveRowFromTo(int srcrow, int destrow);
	void on_autoProceedCheck_clicked(bool checked);
	void on_loopCheck_clicked(bool checked);

	void on_fxTable_itemChanged(QTableWidgetItem *item);
	void if_fxitemwidget_clicked(FxListWidgetItem *listitem);
	void if_fxitemwidget_doubleclicked(FxListWidgetItem *listitem);
	void if_fxitemwidget_tab_pressed(FxListWidgetItem *listitem);
	void if_fxitemwidget_enter_pressed(FxListWidgetItem *listitem);
	void if_fxitemwidget_edited(FxListWidgetItem *listitem, const QString &text);
	void if_fxitemwidget_seeked(FxListWidgetItem *listitem, int perMille);

	void drop_event_receiver(QString str, int row);
	void on_randomCheckBox_clicked(bool checked);
	void on_closeButton_clicked();
	void on_editButton_clicked(bool checked);

signals:
	void fxCmdActivated(FxItem *, CtrlCmd, Executer *);
	void fxItemSelected(FxItem *);
	void fxItemSelectedForEdit(FxItem *);
	void fxItemSelectedInChildFxListWidget(FxItem *);
	void fxTypeColumnDoubleClicked(FxItem *);
	void dropEventReceived(QString text, int row);
	void listModified();
	void editableChanged(bool state);
};

#endif // FXLISTWIDGET_H
