#ifndef UNIVERSEEDITORWIDGET_H
#define UNIVERSEEDITORWIDGET_H

#include "ui_universeeditorwidget.h"
#include <QTableWidgetItem>

class SR_FixtureList;

class DMXTableWidgetItem : public QTableWidgetItem
{
public:
	int dmxChan = -1;

public:
	DMXTableWidgetItem(int type = Type);
	DMXTableWidgetItem(int dmxChan, const QString &text, int type = Type);
};

class UniverseEditorWidget : public QWidget, private Ui::UniverseEditorWidget
{
	Q_OBJECT
private:
	SR_FixtureList *m_fixtureList;
	QString m_lastFixturePath;

public:
	explicit UniverseEditorWidget(QWidget *parent = nullptr);
	~UniverseEditorWidget();

	bool saveToFilesystem(const QString &path);
	bool loadFromFilesystem(const QString &path);

	static QString defaultFilepath();

private:
	bool copyFixturesToGui(SR_FixtureList *fixlist);

private slots:
	void on_pushButton_close_clicked();
	void on_selectDeviceButton_clicked();
	void on_addDeviceButton_clicked();
	void on_removeDeviceButton_clicked();
	void on_pushButton_saveLayout_clicked();
	void on_pushButton_loadLayout_clicked();
};

#endif // UNIVERSEEDITORWIDGET_H
