#ifndef SEQUENCESTATUSWIDGET_H
#define SEQUENCESTATUSWIDGET_H

#include "ui_sequencestatuswidget.h"
#include <QReadWriteLock>
#include <QHash>
#include <QListWidget>

class FxSeqItem;
class FxItem;
class Executer;

class SeqStatusListItem : public QListWidgetItem
{
protected:
	Executer *myExec;
	FxItem *myFx;

public:
	SeqStatusListItem(Executer *exec, FxItem *fx, QListWidget *list = 0);
	inline FxItem * fxItem() const {return myFx;}
	inline void setFxItem(FxItem *fx) {myFx = fx;}
};


class SequenceStatusWidget : public QWidget, public Ui::SequenceStatusWidget
{
	Q_OBJECT
private:
	QReadWriteLock *rwlock;
	QHash<FxItem*,SeqStatusListItem*>fx_hash;
	QHash<Executer*,SeqStatusListItem*>exec_hash;

public:
	SequenceStatusWidget(QWidget *parent = 0);
	~SequenceStatusWidget();

	bool appendSequence(FxSeqItem *seq);
	bool removeSequence(FxSeqItem *seq);
	bool updateSequence(FxSeqItem *seq);

	bool appendExecuter(Executer *exec);
	bool removeExecuter(Executer *exec);
	bool updateExecuter(Executer *exec);

private:
	void update_display();

public slots:
	void propagateExecuter(Executer *exec);
	void insertExecuter(Executer *exec);
	void scratchExecuter(Executer *exec);

};

#endif // SEQUENCESTATUSWIDGET_H
