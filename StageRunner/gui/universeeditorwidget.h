#ifndef UNIVERSEEDITORWIDGET_H
#define UNIVERSEEDITORWIDGET_H

#include "ui_universeeditorwidget.h"
#include "config.h"

#include <QTableWidgetItem>

class SR_FixtureList;
class FxSceneItem;

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
	SR_FixtureList *m_fixtureList = nullptr;			///< current selected universe
	SR_FixtureList *m_universeLayouts[MAX_DMX_UNIVERSE];		///< fixture lists for universes
	QString m_lastFixturePath;

	int m_universe = 0;
	int m_currentTargetDmxAddr[MAX_DMX_UNIVERSE] = {};

public:
	explicit UniverseEditorWidget(QWidget *parent = nullptr);
	~UniverseEditorWidget();

	bool saveToFilesystem(const QString &path, bool saveCurrent = false);
	bool loadFromFilesystem(const QString &path);

	static QString defaultFilepath();
	static FxSceneItem * createSceneFromFixtureList(SR_FixtureList *fixList, uint universe);

private:
	bool copyFixturesToGui(SR_FixtureList *fixlist);

private slots:
	void on_pushButton_close_clicked();
	void on_selectDeviceButton_clicked();
	void on_addDeviceButton_clicked();
	void on_removeDeviceButton_clicked();
	void on_pushButton_saveLayout_clicked();
	void on_pushButton_loadLayout_clicked();
	void on_pushButton_createTemplate_clicked();
	void on_universeTable_cellClicked(int row, int column);
	void on_universeTable_cellChanged(int row, int column);
	void on_universeTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_universeSpin_valueChanged(int arg1);
	void on_saveAsButton_clicked();
	void on_loadButton_clicked();
};

#endif // UNIVERSEEDITORWIDGET_H
