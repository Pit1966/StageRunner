#ifndef FXTIMELINEEDITWIDGET_H
#define FXTIMELINEEDITWIDGET_H

#include "ui_fxtimelineeditwidget.h"
#include "mods/timeline/timelinewidget.h"

#include <QWidget>
#include <QPointer>
#include <QGraphicsSceneDragDropEvent>

using namespace PS_TL;

class FxTimeLineItem;
class FxTimeLineItem;
class TimeLineExecuter;
class AppCentral;
class FxControl;
class FxItem;
class FxTimeLineScene;

class FxTimeLineScene : public TimeLineGfxScene
{
	Q_OBJECT

public:
	FxTimeLineScene(TimeLineWidget *timeLineWidget);

protected:
	void dropEvent(QGraphicsSceneDragDropEvent *event) override;
	void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
	void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
};


// ------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------

class ExtTimeLineWidget : public TimeLineWidget
{
protected:
	FxTimeLineScene *m_fxTimeLineScene	= nullptr;

public:
	bool setFxTimeLineItem(FxTimeLineItem *fxt);
	bool copyToFxTimeLineItem(FxTimeLineItem *fxt);

protected:
	TimeLineBox * createNewTimeLineItem(TimeLineWidget *timeline, int trackId = 1) override;
	TimeLineGfxScene * createTimeLineScene(TimeLineWidget *timeline) override;
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

	static QList<FxTimeLineEditWidget*>m_timelineEditWidgetList;

public:
	FxTimeLineEditWidget(AppCentral *app_central, QWidget *parent = nullptr);
	~FxTimeLineEditWidget();

	static FxTimeLineEditWidget * openTimeLinePanel(FxTimeLineItem *fx, QWidget *parent = nullptr);
	static void destroyAllTimelinePanels();
	static FxTimeLineEditWidget * findParentFxTimeLinePanel(FxItem *fx);

	ExtTimeLineWidget * timeLineWidget() const {return m_timeline;}
	FxTimeLineItem * currentFxItem() const;

	bool setFxTimeLineItem(FxTimeLineItem *fxt);
	bool copyToFxTimeLineItem(FxTimeLineItem *fxt);

public slots:
	void onChildRunStatusChanged(int runStatus);

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:
	void on_runButton_clicked();
	void onCursorPositionChanged(int ms);
	void onMousePositionChanged(int ms);
};

#endif // FXTIMELINEEDITWIDGET_H
