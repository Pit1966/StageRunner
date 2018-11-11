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
