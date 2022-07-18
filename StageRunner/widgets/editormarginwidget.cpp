/***********************************************************************************
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
************************************************************************************/

#include "editormarginwidget.h"
#include <QPainter>
#include <QDebug>

EditorMarginWidget::EditorMarginWidget(QPlainTextEdit *parentEdit)
	: QWidget(parentEdit)
	, m_editWid(parentEdit)
{

}

EditorMarginWidget::~EditorMarginWidget()
{
//	qDebug("destroyed EditorMarginEdit");
}

QSize EditorMarginWidget::sizeHint() const
{
	return QSize(calcRequiredWidth(), 0);
}

int EditorMarginWidget::calcRequiredWidth() const
{
	int digits = 1;
	int max = qMax(1, m_editWid->blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
	space += 5;

	return space;
}

void EditorMarginWidget::paintEvent(QPaintEvent *event)
{
	emit paintEventReceived(event);
}

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

DummyWidget::DummyWidget(QWidget *parent)
	: QWidget(parent)
{
}

void DummyWidget::paintEvent(QPaintEvent *event)
{
	emit paintEventReceived(event);
}


//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------


BackgroundProgressWidget::BackgroundProgressWidget(QWidget *parent)
	: QWidget(parent)
{

}

void BackgroundProgressWidget::scroll(int dx, int dy)
{
	Q_UNUSED(dx)

	m_scrollOffset += dy;

	// qDebug() << "scroll" << m_scrollOffset << dy;

	//QWidget::scroll(dx,dy);
}

/**
 * @brief This is a workaround function to adjust the m_scrollOffsetValue
 * @param lineNo
 */
void BackgroundProgressWidget::setOffsetLine(int lineNo)
{
	if (lineNo == 0)
		m_scrollOffset = 0;
}

void BackgroundProgressWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter p(this);
	p.fillRect(event->rect(), palette().base());

	if (m_progressPerMille > 0) {
		int y = m_lineNum * m_lineHeight + m_lineYOffset + m_scrollOffset;
		int h = m_lineHeight;
		int w = m_progressPerMille * width() / 1000;
		p.fillRect(0,y,w,h, m_barColor);
	}

}



void BackgroundProgressWidget::setProgress(int lineNum, int perMille)
{
//	qDebug("progress %d",perMille);
	bool updateme = false;
	if (m_lineNum != lineNum) {
		m_lineNum = lineNum;
		updateme = true;
	}
	if (m_progressPerMille != perMille) {
		m_progressPerMille = perMille;
		updateme = true;
	}

	if (updateme)
		update();
}
