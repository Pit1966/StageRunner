#ifndef UNIVERSEEDITORWIDGET_H
#define UNIVERSEEDITORWIDGET_H

#include "ui_universeeditorwidget.h"

class SR_FixtureList;

class UniverseEditorWidget : public QWidget, private Ui::UniverseEditorWidget
{
	Q_OBJECT
private:
	SR_FixtureList *m_fixtureList;

public:
	explicit UniverseEditorWidget(QWidget *parent = nullptr);
	~UniverseEditorWidget();

private:
	bool copyFixturesToGui(SR_FixtureList *fixlist);

private slots:
	void on_pushButton_addFixture_clicked();
	void on_pushButton_removeFixture_clicked();
	void on_pushButton_close_clicked();
};

#endif // UNIVERSEEDITORWIDGET_H
