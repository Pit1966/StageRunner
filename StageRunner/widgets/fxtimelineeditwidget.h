#ifndef FXTIMELINEEDITWIDGET_H
#define FXTIMELINEEDITWIDGET_H

#include "ui_fxtimelineeditwidget.h"
#include "mods/timeline/timelinewidget.h"

#include <QWidget>
#include <QPointer>

using namespace PS_TL;

class FxTimeLineItem;
class FxTimeLineItem;
class TimeLineExecuter;
class AppCentral;
class FxControl;

class ExtTimeLineWidget : public TimeLineWidget
{

public:
	bool setFxTimeLineItem(FxTimeLineItem *fxt);
	bool copyToFxTimeLineItem(FxTimeLineItem *fxt);

protected:
	TimeLineItem * createNewTimeLineItem(TimeLineWidget *timeline, int trackId = 1) override;
};

// ------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------

class FxTimeLineEditWidget : public QWidget, public Ui::FxTimeLineEditWidget
{
	Q_OBJECT
private:
	AppCentral *m_aC;
	FxControl *m_fxCtrl;
	ExtTimeLineWidget *m_timeline;
	QPointer<FxTimeLineItem> m_curFxItem;
	QPointer<TimeLineExecuter> m_timelineExecuter;

public:
	FxTimeLineEditWidget(AppCentral *app_central);
	~FxTimeLineEditWidget();
	ExtTimeLineWidget * timeLineWidget() const {return m_timeline;}
	FxTimeLineItem * currentFxItem() const;

	bool setFxTimeLineItem(FxTimeLineItem *fxt);
	bool copyToFxTimeLineItem(FxTimeLineItem *fxt);

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:
	void on_runButton_clicked();
};

#endif // FXTIMELINEEDITWIDGET_H
