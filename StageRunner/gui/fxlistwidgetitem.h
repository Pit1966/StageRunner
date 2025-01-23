//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef FXLISTWIDGETITEM_H
#define FXLISTWIDGETITEM_H

#include "ui_fxlistwidgetitem.h"
#include "commandsystem.h"
#include "tool/qt6_qt5_compat.h"

class FxItem;

class FxListWidgetItem : public QWidget, public Ui::FxListWidgetItem
{
	Q_OBJECT
public:
	enum ColumnType {
		CT_UNDEF,
		CT_NAME,
		CT_ID,
		CT_KEY,
		CT_FX_TYPE,
		CT_FADEIN_TIME,
		CT_FADEOUT_TIME,
		CT_PRE_DELAY,
		CT_POST_DELAY,
		CT_HOLD_TIME,
		CT_MOVE_TIME
	};

	FxItem *linkedFxItem;
	QString itemText;
	ColumnType columnType;

	int myRow;
	int myColumn;

private:
	bool m_isEditable;
	bool m_isNeverEditable;
	bool m_isSelected;
	bool m_isMarked;
	bool m_isSeeked;
	bool m_isStatusBlinking;
	bool m_blinkState;

	int m_currentButton;
	int m_activationIndicatorA;
	int m_activationIndicatorB;
	QPoint m_dragBeginPos;
	QPalette m_orginalPalette;
	QColor m_colorIndicatorA;
	QColor m_colorIndicatorB;
	QColor m_colorMarked;
	QColor m_colorStatus;
	QString m_statusText;
	QFont m_statusFont;

	// tmp
	QTimer *m_blinkTimer;

public:
	FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype = CT_UNDEF);
	FxListWidgetItem(const FxListWidgetItem & other);
	~FxListWidgetItem();
	void setText(const QString &txt);
	inline QString text() const {return itemText;}
	QPoint dragBeginPos() const {return m_dragBeginPos;}
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
	void setTextColor(const QString &col);
	void setTextCentered(bool state);

private:
	void init();

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *qevent) override;
	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;


public slots:
	void setEditable(bool state);
	void setNeverEditable(bool state);
	void setSelected(bool state);
	void setMarked(bool state);
	void setActivationProgress(int perMilleA, int perMilleB);
	void setActivationProgressB(int perMilleB);
	void setActivationProgressA(int perMilleA);
	void setStatusText(const QString &txt);
	void setFxStatus(FxItem *fx, const QString &msg);
	void setStatusBlinkEnabled(bool state);

private slots:
	void onBlinkTimer();
	void if_edit_item_clicked();
	void if_edit_item_doubleclicked();
	void if_edit_item_edited(const QString & text);
	void if_edit_item_tab_pressed();
	void if_edit_item_enter_pressed();
	void if_label_item_doubleclicked();

signals:
	void itemClicked(FxListWidgetItem *);
	void itemDoubleClicked(FxListWidgetItem *);
	void itemTextEdited(FxListWidgetItem *, const QString &);
	void draged(FxListWidgetItem * item);
	void seekToPerMille(FxListWidgetItem * item, int perMille);
	void tabPressedInEdit(FxListWidgetItem * item);
	void enterPressedInEdit(FxListWidgetItem * item);
};

#endif // FXLISTWIDGETITEM_H
