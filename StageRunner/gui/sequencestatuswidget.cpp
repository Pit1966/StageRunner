#include "sequencestatuswidget.h"
#include "fxseqitem.h"
#include "executer.h"

#define READLOCK QReadLocker(*rwlock);Q_UNUSED(rwlock);
#define WRITELOCK QWriteLocker(*rwlock);Q_UNUSED(rwlock);


SeqStatusListItem::SeqStatusListItem(Executer *exec, FxItem *fx, QListWidget *list)
	: QListWidgetItem(list)
	, myExec(exec)
	, myFx(fx)
{
}

SequenceStatusWidget::SequenceStatusWidget(QWidget *parent) :
	QWidget(parent)
{
	rwlock = new QReadWriteLock(QReadWriteLock::Recursive);
	setupUi(this);
}

SequenceStatusWidget::~SequenceStatusWidget()
{
	delete rwlock;
}


bool SequenceStatusWidget::appendSequence(FxSeqItem *seq)
{
	///@todo: Find Executer for initialisation of SeqStatusListItem
	WRITELOCK;
	SeqStatusListItem *item = new SeqStatusListItem(0,seq,seqListWidget);
	item->setText(seq->name());
	fx_hash.insert(seq,item);

	return true;
}

bool SequenceStatusWidget::removeSequence(FxSeqItem *seq)
{
	bool removed = false;
	WRITELOCK;

	SeqStatusListItem *item = fx_hash.value(seq);
	if (item) {
		delete item;
		removed = true;
	}
	fx_hash.remove(seq);

	return removed;
}

bool SequenceStatusWidget::updateSequence(FxSeqItem *seq)
{
	WRITELOCK;
	SeqStatusListItem *item = fx_hash.value(seq);
	if (item) {


		return true;
	}

	return false;
}

bool SequenceStatusWidget::appendExecuter(Executer *exec)
{
	WRITELOCK;
	FxItem *fx = exec->originFx();
	if (fx && fx->fxType() != FX_SEQUENCE) return false;

	QString name;

	SeqStatusListItem *item = new SeqStatusListItem(exec,fx,seqListWidget);
	exec_hash.insert(exec,item);

	if (fx) {
		fx_hash.insert(fx,item);
		name = fx->name();
	} else {
		name = "FxSequence";
	}
	item->setText(name);

	return true;
}

bool SequenceStatusWidget::removeExecuter(Executer *exec)
{
	bool removed = false;
	WRITELOCK;

	FxItem *fx = exec->originFx();

	SeqStatusListItem *item = exec_hash.value(exec);
	if (item) {
		delete item;
		removed = true;
	}

	exec_hash.remove(exec);
	fx_hash.remove(fx);

	return removed;

}

bool SequenceStatusWidget::updateExecuter(Executer *exec)
{
	WRITELOCK;
	SeqStatusListItem *item = exec_hash.value(exec);
	if (item) {
		FxItem *fx = exec->originFx();
		if (fx != item->fxItem()) {
			fx_hash.remove(item->fxItem());
			fx_hash.insert(fx,item);
			item->setFxItem(fx);
			if (fx) {
				item->setText(fx->name());
			}
		}
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
			item->setBackgroundColor(Qt::green);
			break;
		case Executer::EXEC_PAUSED:
			item->setBackgroundColor(Qt::red);
			break;
		default:
			item->setBackgroundColor(palette().base().color());
		}
		return true;
	}

	return false;
}

void SequenceStatusWidget::update_display()
{
	execHashSizeNum->setText(QString::number(exec_hash.size()));
	fxHashSizeNum->setText(QString::number(fx_hash.size()));
}

void SequenceStatusWidget::propagateExecuter(Executer *exec)
{
	READLOCK;

	if (exec_hash.contains(exec)) {
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
		case Executer::EXEC_PAUSED:
			updateExecuter(exec);
			break;
		default:
			removeExecuter(exec);
		}
	} else {
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
		case Executer::EXEC_PAUSED:
			appendExecuter(exec);
			break;
		default:
			break;
		}
	}
	update_display();
}

void SequenceStatusWidget::insertExecuter(Executer *exec)
{
	appendExecuter(exec);
	update_display();
}

void SequenceStatusWidget::scratchExecuter(Executer *exec)
{
	removeExecuter(exec);
	update_display();
}
