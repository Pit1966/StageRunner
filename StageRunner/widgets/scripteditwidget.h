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

#ifndef SCRIPTEDITWIDGET_H
#define SCRIPTEDITWIDGET_H

#include <QPlainTextEdit>
#include <QHash>
#include <QSyntaxHighlighter>
#include <QSet>


class ScriptHighlighter;
class EditorMarginWidget;
class DummyWidget;
class BackgroundProgressWidget;

class ScriptLineStat
{
public:
	int lineNum = -1;
	int errorCount = 0;
	QString lastMsg;

};

enum STATUS_CODE
{
	SC_UNDEF,
	SC_OK,
	SC_SYSTEM_ERR,
	SC_SCRIPT_RESULT_ERR
};

class ScriptHighlighter : public QSyntaxHighlighter
{
private:
	QSet<QString> m_keywords;
	QSet<QString> m_paraKeywords;

public:
	ScriptHighlighter(QTextDocument* document);
	int addKeywords(const QStringList &strings);
	int addParaKeywords(const QStringList &strings);

protected:
	void highlightBlock(const QString &text);
};


class ScriptEditWidget : public QPlainTextEdit
{
	Q_OBJECT
private:
	ScriptHighlighter *m_syntaxHighlighter;
	EditorMarginWidget *m_leftMarginWid;
	DummyWidget *m_rightMarginWid;
	BackgroundProgressWidget *m_bgWidget;						///< Used to display the wait progress bars in the background of the text
	QHash<int, ScriptLineStat> m_lineStats;						///< A hash that holds line status for all lines.
	QList<QTextEdit::ExtraSelection> m_errorSelections;

	int m_rightMarginWidth = 50;
	int m_blockCount = 0;										///< current line number count
	bool m_isRunning = false;									///< when true, Script is executed

public:
	explicit ScriptEditWidget(QWidget *parent = nullptr);
	QString scriptText() const;
	void setScriptText(const QString &txt);
	bool isModified() const;
	void clearModified();
	int lineCount() const;
	void addKeywords(const QStringList &keywordStrings);

protected:
	void resizeEvent(QResizeEvent *e);

private:

signals:

public slots:
	void setCurrentLineNumber(int num);
	void setLineStatus(STATUS_CODE statusCode, int lineNum, const QString &txt);
	void clearLineStatusSelections();
	void clearLineStatistics();
	void showWaitProgress(int lineNum, int perMille);
	void setRunStatus(bool isRunning);

private slots:
	void updateMarginWidth(int width);
	void updateMargin(const QRect &rect, int dy);
	void paintMargin(QPaintEvent *event);
	void paintRight(QPaintEvent *event);
	void highlightCurrentLine();
	void onTextChanged();

};

#endif // SCRIPTEDITWIDGET_H
