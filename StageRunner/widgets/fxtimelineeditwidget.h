#ifndef FXTIMELINEEDITWIDGET_H
#define FXTIMELINEEDITWIDGET_H

#include "mods/timeline/timelinewidget.h"

#include <QPointer>

using namespace PS_TL;

class FxTimeLineItem;

class FxTimeLineEditWidget : public TimeLineWidget
{
	Q_OBJECT
private:
	QPointer<FxTimeLineItem> m_curFxItem;

public:
	FxTimeLineEditWidget();

	bool setFxTimeLineItem(FxTimeLineItem *fxt);
	bool copyToFxTimeLineItem(FxTimeLineItem *fxt);

protected:
	void closeEvent(QCloseEvent *event) override;

};

#endif // FXTIMELINEEDITWIDGET_H
