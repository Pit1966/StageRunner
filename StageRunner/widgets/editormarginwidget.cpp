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

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
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
