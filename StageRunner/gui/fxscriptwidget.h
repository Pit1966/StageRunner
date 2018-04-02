#ifndef FXSCRIPTWIDGET_H
#define FXSCRIPTWIDGET_H

#include "ui_fxscriptwidget.h"

class FxScriptItem;
class FxScriptWidget;
class FxItem;

class FxScriptWidget : public QWidget, private Ui::FxScriptWidget
{
	Q_OBJECT
private:
	static QList<FxScriptWidget*>m_scriptWidgetList;
	FxScriptItem *m_OriginFxScript;

public:
	static FxScriptWidget * openFxScriptPanel(FxScriptItem *fx, QWidget *parent = 0);
	static void destroyAllScriptPanels();
	static FxScriptWidget * findParentFxScriptWidget(FxItem *fx);


	bool setFxScriptContent(FxScriptItem *fxscr);


protected:
	void changeEvent(QEvent *e);

private:
	FxScriptWidget(FxScriptItem *fxscr, QWidget *parent = 0);
	~FxScriptWidget();

	bool applyChanges();

private slots:
	void on_cancelButton_clicked();
	void on_applyButton_clicked();
	void on_applyCloseButton_clicked();

signals:
	void modified();
};

#endif // FXSCRIPTWIDGET_H
