/** @file log.h
* @author Peter Steinmeyer QASS GmbH<steinmeyer@qass.net>
*/

#ifndef LOG_H
#define LOG_H

#include <QThread>
#include <QWidget>
#include <QColor>
#include <QFile>
#include <QList>
#include <QVariant>
#include <QMutex>


enum MsgLogType {
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
 * Von Log darf es nur eine Instanz geben
 *
 * Achtung: Es gibt nur eine LOG Instanz die über den extern Pointer *logThread angesprochen werden kann.
 *
 */
class Log : public QThread
{
	Q_OBJECT;

public:

	bool loggingEnabled;				///< zeigt an, das normales Logging in Gui möglich ist
	bool logfileEnabled;						///< zeigt an, dass Logfile geöffnet ist.
	volatile bool stopThreadFlag;
	QAtomicInt mainThreadLifeCount;		///< wird benutzt, um zu erkennen, ob Mainthread hängt

private:
	static QThread *main_thread;		///< Pointer auf Main Thread

	QTextEdit * statuswid;		///< an dieses Widget, wird der Statustext gesendet
	QColor color;				///< aktuelle Textausgabe Farbe
	int log_line_cnt;			///< Die Anzahl der Zeilen, die ins Log Fenster ausgegeben werden (Bei Ereichen von MAX_LOG_LINES, wird dieses gelöscht);

	QMutex logfile_mutex;
	QFile logfile;

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
	void errorPopupMsg(const QString & where, const QString & text);
	void infoPopupMsg(const QString & where, const QString & text);
	bool openLogfileAppend();
	void closeLogfile();
	void flushLogfile();
	bool rotateLog();
	int readAppendCurrentLogFile();
	int readAppendLogFile(const QString & path);

	static void debugCurrentThread();
	static QString getCurrentThreadName();
	inline static QThread * getCurrentThreadPointer() {return currentThread();}
	inline static bool isMainThread() {return (main_thread == currentThread());}


private:
	void run();
	void do_append_log_text(const QString & txt, int level, quint32 type, bool remote_only);
	void do_append_log_error(const QString & txt,int level, quint32 type, bool remote_only);
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
};


#ifdef LOG_CPP
	Log *logThread = 0;
	int debug = 0;
#else
	extern Log *logThread;
	extern int debug;
#endif

#endif // ifdef LOG_H

