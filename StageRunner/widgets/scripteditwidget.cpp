#include "scripteditwidget.h"
#include "editormarginwidget.h"

#include <QPainter>
#include <QTextBlock>
#include <QDebug>
#include <QScrollBar>


ScriptHighlighter::ScriptHighlighter(QTextDocument *document)
	: QSyntaxHighlighter(document)
{
}

int ScriptHighlighter::addKeywords(const QStringList &strings)
{
	int count = 0;
	for (const QString &s : strings) {
		if (!m_keywords.contains(s)) {
			m_keywords.insert(s);
			count++;
		}
	}

	return count;
}

int ScriptHighlighter::addParaKeywords(const QStringList &strings)
{
	int count = 0;
	for (const QString &s : strings) {
		if (!m_paraKeywords.contains(s)) {
			m_paraKeywords.insert(s);
			count++;
		}
	}

	return count;
}

void ScriptHighlighter::highlightBlock(const QString &text)
{
	QTextCharFormat keywordFormat;
	if (text.startsWith("#")) {
		keywordFormat.setForeground(Qt::gray);
		setFormat(0,text.size(),keywordFormat);
		return;
	}

	keywordFormat.setFontWeight(QFont::Bold);
	keywordFormat.setForeground(QColor(Qt::blue).lighter(150));

	QTextCharFormat paraFormat;
	paraFormat.setForeground(Qt::blue);

	bool keyWordFound = false;

	QStringList words = text.split(' ');
	int pos = 0;
	int column = 0;
	for (const QString &word : words) {
		if (m_keywords.contains(word) && !keyWordFound) {
			setFormat(pos, word.size(),keywordFormat);
			keyWordFound = true;
		}
		if (column > 0 && m_paraKeywords.contains(word)) {
			setFormat(pos, word.size(),paraFormat);
		}

		pos += word.size() + 1;
		column++;
	}
}



ScriptEditWidget::ScriptEditWidget(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_syntaxHighlighter(new ScriptHighlighter(document()))
	, m_leftMarginWid(new EditorMarginWidget(this))
	, m_rightMarginWid(new DummyWidget(this))
	, m_bgWidget(new BackgroundProgressWidget(this))
{
	viewport()->setAutoFillBackground(false);
	viewport()->raise();

	setLineWrapMode(QPlainTextEdit::NoWrap);

//	m_syntaxHighlighter->addKeywords(QassGlobal::ref().cmdManager()->cmdList());
//	m_syntaxHighlighter->addParaKeywords(QassGlobal::ref().cmdManager()->paraKeywords());

	connect(m_leftMarginWid,SIGNAL(paintEventReceived(QPaintEvent*)),this,SLOT(paintMargin(QPaintEvent*)));
	connect(m_rightMarginWid,SIGNAL(paintEventReceived(QPaintEvent*)),this,SLOT(paintRight(QPaintEvent*)));
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateMarginWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateMargin(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	connect(this, SIGNAL(textChanged()),this,SLOT(onTextChanged()));

	updateMarginWidth(10);
}

QString ScriptEditWidget::scriptText() const
{
	return toPlainText();
}

void ScriptEditWidget::setScriptText(const QString &txt)
{
	setPlainText(txt);
}

bool ScriptEditWidget::isModified() const
{
	return document()->isModified();
}

void ScriptEditWidget::clearModified()
{
	document()->setModified(false);
}

int ScriptEditWidget::lineCount() const
{
	return document()->blockCount();
}

void ScriptEditWidget::addKeywords(const QStringList &keywordStrings)
{
	m_syntaxHighlighter->addKeywords(keywordStrings);
}

void ScriptEditWidget::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect rect = contentsRect();
	int leftMarginWidth = m_leftMarginWid->calcRequiredWidth();
	QRect marginrect = QRect(rect.left(), rect.top(), leftMarginWidth, rect.height());
	m_leftMarginWid->setGeometry(marginrect);

	QRect view = viewport()->rect();
	m_rightMarginWid->setGeometry(view.width() + leftMarginWidth, rect.top(), m_rightMarginWidth, view.height());
	m_bgWidget->setGeometry(rect.left() + leftMarginWidth, rect.top(), view.width()-1, view.height());

	m_bgWidget->setLineSpacing(fontMetrics().descent(),fontMetrics().height());
}

void ScriptEditWidget::setCurrentLineNumber(int num)
{

	if (num == 0)
		m_errorSelections.clear();

	QTextCursor cursor(textCursor());
	cursor.setPosition(0);
	if (num > 0)
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, num);

	setTextCursor(cursor);

	// This is a workaround for drifting output.
	QScrollBar *bar = verticalScrollBar();
	if (bar)
		m_bgWidget->setOffsetLine(bar->value());
}

/**
 * @brief ScriptEditWidget::setLineStatus
 * @param statusCode
 * @param lineNum Script Line Number
 * @param txt
 */
void ScriptEditWidget::setLineStatus(STATUS_CODE statusCode, int lineNum, const QString &txt)
{
	if (lineNum < 0) return;

//	qDebug() << "Linestatus" << int(statusCode) << lineNum << txt;

	if (statusCode > SC_OK) {
		QTextCursor cursor(textCursor());
		cursor.setPosition(0);
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNum);

		QColor lineColor = QColor(Qt::red).lighter(160);
		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = cursor;
		selection.cursor.clearSelection();

		m_errorSelections.append(selection);
		setExtraSelections(m_errorSelections);

		if (!m_lineStats.contains(lineNum)) {
			ScriptLineStat newstat;
			newstat.lineNum = lineNum;
			m_lineStats.insert(lineNum, newstat);
		}
		ScriptLineStat &linestat = m_lineStats[lineNum];
		linestat.lastMsg = txt;
		if (statusCode == SC_SCRIPT_RESULT_ERR)
			linestat.errorCount++;

		m_rightMarginWid->update();
	}
}

void ScriptEditWidget::clearLineStatusSelections()
{
	m_errorSelections.clear();
	highlightCurrentLine();
}

void ScriptEditWidget::clearLineStatistics()
{
	m_lineStats.clear();
	clearLineStatusSelections();
}

void ScriptEditWidget::showWaitProgress(int lineNum, int perMille)
{
	m_bgWidget->setProgress(lineNum, perMille);
}

void ScriptEditWidget::setRunStatus(bool isRunning)
{
	if (m_isRunning != isRunning) {
		m_isRunning = isRunning;

		if (!isRunning) {
			m_bgWidget->setProgress(0,0);
		} else {
			clearLineStatistics();
		}

		highlightCurrentLine();
		setReadOnly(isRunning);
	}
}

void ScriptEditWidget::updateMarginWidth(int width)
{
	int neededWidth = qMax(width, m_leftMarginWid->calcRequiredWidth());

	setViewportMargins(neededWidth,0,m_rightMarginWidth,0);
}

/**
 * @brief ScriptEditWidget::updateMargin
 * @param rect
 * @param dy
 *
 * This slot is invoked when the editors viewport has been scrolled.
 * The QRect given as argument is the part of the editing area that is do be updated (redrawn).
 * dy holds the number of pixels the view has been scrolled vertically.
 */
void ScriptEditWidget::updateMargin(const QRect &rect, int dy)
{
	if (dy) {
		m_leftMarginWid->scroll(0, dy);
		m_rightMarginWid->scroll(0, dy);
		m_bgWidget->scroll(0,dy);
	} else {
		m_leftMarginWid->update(0, rect.y(), m_leftMarginWid->width(), rect.height());
		m_rightMarginWid->update(0, rect.y(), m_rightMarginWid->width(), rect.height());
		m_bgWidget->update();
	}

	if (rect.contains(viewport()->rect()))
		updateMarginWidth(10);
}

void ScriptEditWidget::paintMargin(QPaintEvent *event)
{
	QPainter painter(m_leftMarginWid);
	painter.fillRect(event->rect(), Qt::lightGray);
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();

	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	// qDebug() << "block" << blockBoundingGeometry(block) << top;
	// qDebug() << "content rect" << contentsRect() << event->rect();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			// left margin
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_leftMarginWid->width() - 5, fontMetrics().height(), Qt::AlignRight, number);

		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}

}

void ScriptEditWidget::paintRight(QPaintEvent *event)
{
	// qDebug() << "paintright" << event->rect();
	QRect rect = event->rect();

	QPainter p(m_rightMarginWid);
	// p.fillRect(width()-m_rightMarginWidth, 0, m_rightMarginWidth, height(), Qt::yellow);
	p.fillRect(rect, Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();

	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {

			// right margin
			if (m_lineStats.contains(blockNumber)) {
				const ScriptLineStat &linestat = m_lineStats.value(blockNumber);
				QString err = QString("E%1").arg(linestat.errorCount, 4, 10, QLatin1Char('0'));
				p.setPen(Qt::darkRed);
				p.drawText(0, top, m_rightMarginWid->width() - 5, fontMetrics().height(), Qt::AlignRight, err);

			}

		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void ScriptEditWidget::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections = m_errorSelections;

	if (!m_isRunning) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = palette().highlight().color();

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();

		extraSelections.prepend(selection);
	}

	setExtraSelections(extraSelections);
}

void ScriptEditWidget::onTextChanged()
{
	if (blockCount() != m_blockCount) {
		m_blockCount = blockCount();
		clearLineStatistics();
	}
}
