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


#ifndef LOG_H
#define LOG_H

#include <QThread>
#include <QWidget>
#include <QColor>
#include <QFile>
#include <QList>
#include <QVariant>
#include <QMutex>
#include <QDateTime>


enum MsgLogType {
	MSG_LOGNONE = 0,
	MSG_DEBUGTXT = 1<<0,
	MSG_DEBUGERR = 1<<1,
	MSG_LOGTXT = 1<<2,
	MSG_LOGERR = 1<<3,
	MSG_DEVELTXT = 1<<4,

	MSG_ALL = MSG_DEBUGTXT + MSG_DEBUGERR + MSG_LOGTXT + MSG_LOGERR + MSG_DEVELTXT
};

enum ErrMsgCode {
	ERR_NONE,
	ERR_DB_OPEN_FAILED,
	ERR_DB_CREATE_TABLE_FAILED,
	ERR_DB_UPDATE_TABLE_FAILED,
	ERR_DB_QUERY_FAILED,
	ERR_DB_NULL_POINTER_QUERY,
	ERR_DB_QUERY_NOTABLE,
	ERR_DB_INVALID_FIELD,
	ERR_DB_QUERY_EXECUTED,
	ERR_DB_QUERY_CONVERT_FAILED,
	ERR_DB_DELETE_WITHOUT_WHERE,
	ERR_DB_REPLACE_WITHOUT_WHERE,
	ERR_DB_UPDATE_WITHOUT_WHERE,
	ERR_DB_REPLACE_AMBIGUOUS

};

class LogEntry
{
public:
	MsgLogType type;
	QString text;
	QDateTime dateTime;
public:
	LogEntry(MsgLogType type = MSG_LOGNONE, const QString &text = QString(), const QDateTime & dateTime = QDateTime())
		: type(type)
		, text(text)
		, dateTime(dateTime)
	{}
	LogEntry(const LogEntry &o)
		: type(o.type)
		, text(o.text)
		, dateTime(o.dateTime)
	{}
};

#define MAX_LOG_LINES 10000
#define MAX_LOG_SIZE 10000000

// Macros zur bequemeren Loggingausgabe
#define LOGTEXT(a) logThread->appendLogText(a)
#define LOGERROR(a) logThread->appendLogError(a)
#define POPUPINFOMSG logThread->infoPopupMsg
#define POPUPERRORMSG logThread->errorPopupMsg
#define DEBUGTEXT logThread->appendDebugText
#define DEVELTEXT logThread->appendDevelText
#define DEBUGERROR logThread->appendDebugError
#define ERRORNOP(w,num) {logThread->appendLogStdError(w,num);error=(num);}
#define ERRORPARA(w,num,para) {logThread->appendLogStdError(w,num,QString("%1").arg(para));error=(num);}

class QTextEdit;

/**
 * In Log befinden sich alle Funktionen und Makros, die eine komfortable Ausgabe von Debug Informationen auf
 * die Konsole und in das LOG Fenster der Anwendung
 * sowie Protokollierung (insbesondere von Fehlfunktionen) in eine Datei zur Verfügung stellen.
 *
 * @attention Von Log darf es nur eine Instanz geben
 * @note Es gibt nur eine LOG Instanz die über den extern Pointer *logThread angesprochen werden kann.
 *
 */
class Log : public QThread
{
	Q_OBJECT

public:

	bool loggingEnabled				= false;		///< zeigt an, das normales Logging in Gui möglich ist
	bool logfileEnabled				= false;		///< zeigt an, dass Logfile geöffnet ist.
	volatile bool stopThreadFlag	= false;
	QAtomicInt mainThreadLifeCount	= 20000;		///< wird benutzt, um zu erkennen, ob Mainthread hängt

private:
	static QThread *m_mainThread;					///< Pointer auf Main Thread

	QTextEdit * m_statusWid			= nullptr;		///< an dieses Widget, wird der Statustext gesendet
	QColor color;									///< aktuelle Textausgabe Farbe
	int m_logLineCount				= 0;			///< Die Anzahl der Zeilen, die ins Log Fenster ausgegeben werden (Bei Ereichen von MAX_LOG_LINES, wird dieses gelöscht);
	QList<LogEntry> m_shadowLog;					///< Messages logged in application startup phase, when GUI is not ready

	QMutex m_logfileMutex;
	QFile m_logfile;

	QWindow *m_currentFocusWindow	= nullptr;
	int m_noFocusCount				= 0;

public:
	Log();
	~Log();
	void initLog(QWidget *wid);
	bool stopLog();
	void appendDebugText(const char *str,...);
	void appendDevelText(const char *str,...);
	void appendDebugError(const char *str,...);
	void appendLogError(const QString & txt, int level = 0);
	void appendLogText(const QString & txt, int level = 0);
	void appendLogStdError(const QString & where, int e_num , const QString & para = "");
	void setColor(const QColor &col);
	QWidget *setStatusWidget(QWidget * wid);
	void errorPopupMsg(const QString & where, const QString & text, QWidget *parentWid = nullptr);
	void infoPopupMsg(const QString & where, const QString & text, QWidget *parentWid = nullptr);
	bool openLogfileAppend();
	void closeLogfile();
	void flushLogfile();
	bool rotateLog();
	int readAppendCurrentLogFile();
	int readAppendLogFile(const QString & path);

	static void debugCurrentThread();
	static QString getCurrentThreadName();
	inline static QThread * getCurrentThreadPointer() {return currentThread();}
	inline static bool isMainThread() {return (m_mainThread == currentThread());}

	inline const QList<LogEntry> & shadowLog() const {return m_shadowLog;}
	QStringList shadowLogStrings(MsgLogType type) const;
	inline int shadowLogSize() const {return m_shadowLog.size();}
	int shadowErrorCount() const;
	int emitShadowLog(bool clearLog = true);

private:
	void run();
	void do_append_log_text(const QString & txt, int level, MsgLogType type, bool remote_only);
	void do_append_log_error(const QString & txt,int level, MsgLogType type, bool remote_only);
	void replaceColorTags(QString &txt);
	void process_line_for_log_output(QString & msg);

public slots:
	void appendText(QString text);

private slots:
	void terminated();

signals:
	void newtext(const QString &);
	void newcolor(const QColor &);
	void clearlog();
	void msgLogged(const QString, quint32);
	void infoMsgReceived(const QString & func, const QString & text);
	void errorMsgReceived(const QString & func, const QString & text);

	void warnMsgSent(const QString &msg, int display_ms);
};

extern const char *error_msg_asc[];

#ifdef LOG_CPP
	Log *logThread = 0;
	int debug = 0;
#else
	extern Log *logThread;
	extern int debug;
#endif

#endif // ifdef LOG_H

