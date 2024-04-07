#ifndef FXTIMELINEITEM_H
#define FXTIMELINEITEM_H

#include "fxitem.h"

#include <QCoreApplication>

class FxTimeLineItem : public FxItem
{
	Q_DECLARE_TR_FUNCTIONS(FxTimeLineItem)

private:


public:
	FxTimeLineItem(FxList *fxList);
	FxTimeLineItem(const FxTimeLineItem &o);

	qint32 loopValue() const override;
	void setLoopValue(qint32 val) override;
	void resetFx() override;

private:
	void init();

};

#endif // FXTIMELINEITEM_H
