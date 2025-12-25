#ifndef UNIVERSEEDITORWIDGET_H
#define UNIVERSEEDITORWIDGET_H

#include "ui_universeeditorwidget.h"
#include "config.h"

#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include <QPainter>

class SR_FixtureList;
class FxSceneItem;

// -------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------

class DmxItemBgDelegate : public QStyledItemDelegate
{
private:
	QWidget * m_parentWid;
	QColor m_baseColor  = QColor(0x2a2a2a);
	QColor m_alt1Color   = QColor(0x232424);
	QColor m_alt2Color   = QColor(0x3a3a3a);

public:
	using QStyledItemDelegate::QStyledItemDelegate;

	DmxItemBgDelegate(QWidget *parent)
		: QStyledItemDelegate(parent)
		, m_parentWid(parent)
	{
		if (parent) {
			m_baseColor = m_parentWid->palette().base().color();
			m_alt1Color = m_baseColor;
			m_alt2Color = m_parentWid->palette().alternateBase().color();
			// m_alt1Color = m_baseColor.darker(150);
			// m_alt2Color = m_baseColor.darker(200);
		}
	}

	void paint(QPainter *p, const QStyleOptionViewItem &opt,
			   const QModelIndex &idx) const override
	{
		int devIdx = -1;
		if (idx.data(Qt::UserRole + 1).isValid())
			devIdx = idx.data(Qt::UserRole + 1).toInt();

		QStyleOptionViewItem o(opt);
		QColor bg = (devIdx % 2) ? m_alt1Color : m_alt2Color;

		if (devIdx < 0) {
			bg = QColor(0x006600);
		}

		p->fillRect(o.rect, bg);
		QStyledItemDelegate::paint(p, o, idx); // Text/Selection etc.
	}
};

// -------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------


class DMXTableWidgetItem : public QTableWidgetItem
{
public:
	int devIdx = -1;
	int dmxChan = -1;

public:
	DMXTableWidgetItem(int devIdx, int dmxChan, const QString &text, int type = Type);
};

// -------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------

class UniverseEditorWidget : public QWidget, private Ui::UniverseEditorWidget
{
	Q_OBJECT
private:
	SR_FixtureList *m_fixtureList = nullptr;			///< current selected universe
	SR_FixtureList *m_universeLayouts[MAX_DMX_UNIVERSE];		///< fixture lists for universes
	QString m_lastFixturePath;

	int m_universe = 0;
	int m_currentTargetDmxAddr[MAX_DMX_UNIVERSE] = {};		///< current selected DMX address. This begins with 1

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
	void on_universeSpin_valueChanged(int arg1);
	void on_saveAsButton_clicked();
	void on_loadButton_clicked();
};

#endif // UNIVERSEEDITORWIDGET_H
