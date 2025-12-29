/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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
************************************************************************************/

#ifndef EDITORMARGINWIDGET_H
#define EDITORMARGINWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>

class EditorMarginWidget : public QWidget
{
	Q_OBJECT

private:
	QPlainTextEdit *m_editWid;

public:
	EditorMarginWidget(QPlainTextEdit *parentEdit);
	~EditorMarginWidget();

	QSize sizeHint() const;

	int calcRequiredWidth() const;

protected:
	void paintEvent(QPaintEvent *event);

signals:
	void paintEventReceived(QPaintEvent *event);
};

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

class DummyWidget : public QWidget
{
	Q_OBJECT
public:
	DummyWidget(QWidget *parent);

protected:
	void paintEvent(QPaintEvent *event);

signals:
	void paintEventReceived(QPaintEvent *event);


};

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

class BackgroundProgressWidget : public QWidget
{
	Q_OBJECT
private:
	int m_lineNum = 0;
	int m_progressPerMille = 0;
	int m_lineHeight = 10;
	int m_lineYOffset = 0;
	int m_scrollOffset = 0;
	QColor m_barColor = QColor(Qt::yellow).lighter(130);

public:
	BackgroundProgressWidget(QWidget *parent);
	inline void setLineSpacing(int offset, int height) {m_lineYOffset = offset; m_lineHeight = height;}
	void scroll(int dx, int dy);
	void setOffsetLine(int lineNo);

protected:
	void paintEvent(QPaintEvent *event);

public slots:
	void setProgress(int lineNum, int perMille);

};


#endif // EDITORMARGINWIDGET_H
