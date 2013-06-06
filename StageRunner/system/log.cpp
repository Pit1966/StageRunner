/** @file log.cpp
* @author Peter Steinmeyer QASS GmbH<steinmeyer@qass.net>
*/
#define LOG_CPP

#include "log.h"
#include "../config.h"
#ifdef unix
#include <unistd.h>
#endif

#include <QTextEdit>
#include <QDateTime>
#include <QProgressDialog>
#include <QTime>
#include <QApplication>



/// Statics
QThread *Log::main_thread = 0;

/// Die Ascii Texte als Auflösung der konstanten Fehlernummern @see ErrMsgCode
const char *error_msg_asc[] = {
	"OK",
	"Failed to open database",				// ERR_DB_OPEN_FAILED
	"Failed to create table in database",	// ERR_DB_CREATE_TABLE_FAILED
	"Failed to update table in database",	// ERR_DB_UPDATE_TABLE_FAILED
	"DB query failed",						// ERR_DB_QUERY_FAILED
	"DB null pointer query",				// ERR_DB_NULL_POINTER_QUERY
	"DB query not valid",					// ERR_DB_QUERY_NOTABLE
	"DB access to invalid field name",		// ERR_DB_INVALID_FIELD
	"DB query already executed",			// ERR_DB_QUERY_EXECUTED
	"DB query could not convert datatype",	// ERR_DB_QUERY_CONVERT_FAILED
	"DB tried DELETE without WHERE clause",	// ERR_DB_DELETE_WITHOUT_WHERE
	"DB tried REPLACE without WHERE clause",// ERR_DB_REPLACE_WITHOUT_WHERE
	"DB tried UPDATE without WHERE clause",	// ERR_DB_UPDATE_WITHOUT_WHERE
	"DB REPLACE is ambiguous",				// ERR_DB_REPLACE_AMBIGUOUS

};

Log::Log()
{
	this->setObjectName("Thread::Log");
	loggingEnabled = false;
	statuswid = 0;
	logfileEnabled = false;
	log_line_cnt = 0;
	stopThreadFlag = false;
	mainThreadLifeCount = 20000;
	main_thread = currentThread();
}


Log::~Log()
{
	closeLogfile();
}

/**
 * \brief LOG Messagesystem starten
 * @param wid Das ist das EditWidget, in das die Ausgaben geleitet werden
 *
 * Desweiteren wird hier die LOG Datei im Filesystem geöffnet, die die Status-, Fehler- und Debugmeldungen
 * empfängt.
 */
void Log::initLog(QWidget *wid) {
	// Hierhin werden die Logging Texte gesendet
	setStatusWidget(wid);

	// Logfile öffnen
	openLogfileAppend();

	if (this->isRunning()) return;	// Thread läuft schon

	stopThreadFlag = false;

	// Thread starten
	this->start();
	qDebug("\n");
	DEVELTEXT("----- StageRunner Log started -----");
}

/**
 * \brief Logging anhalten (Thread beenden)
 * @return true, falls Thread mit Flag beendet werden konnte; falls, falls Thread terminiert werden musste
 */
bool Log::stopLog()
{
	bool ok = true;

	if (isRunning()) {
		// Flag für Beenden setzen
		stopThreadFlag = true;

		// 5 Sekunden auf Beendigung des Threads warten
		QTime wait;
		wait.start();
		while (wait.elapsed() < 5000 && isRunning()) {};

		// Falls immer noch rennt -> mit Gewalt abbrechen
		if (isRunning()) {
			terminate();
			msleep(1000);
			ok = false;
		}

	} else {
		stopThreadFlag = false;
	}

	return ok;
}

void Log::run()
{
	// static bool ready_fail = false;

	while (!stopThreadFlag) {
		msleep(10);
		int cnt = mainThreadLifeCount.fetchAndAddOrdered(-1);
		if (cnt <= 0 && 0) {
			if (cnt == 0) {
				DEBUGERROR("Main thread seems to be busy ... ");
				// ready_fail = true;
			}
		}
	}
	stopThreadFlag = false;
}


void Log::terminated()
{
	loggingEnabled = false;
	stopThreadFlag = false;
}

/**
 * \brief Beliebigen Text in Log Ausgabe schreiben
 * @param text
 */
void Log::appendText(QString text)
{
	do_append_log_text(text, 0, MSG_LOGTXT, false);
}

void Log::appendLogText(const QString & txt, int level)
{

	do_append_log_text(txt, level, MSG_LOGTXT, false);
}

void Log::do_append_log_text (const QString & txt, int level, quint32 type, bool remote_only)
{
	Q_UNUSED(type);

	QString msg;
	switch (level) {
	case 1:
		setColor(QColor(Qt::blue));
		msg = "Kernel";
		break;
	case 2:
		setColor(QColor(Qt::darkGreen));
		msg = "VarLog";
		break;
	case 3:
		setColor(QColor(Qt::darkGray));
		msg = "Debug ";
		break;
	case 4:
		setColor(QColor(Qt::darkBlue));
		msg = "Status";
		break;
	case 5:
		setColor(QColor(Qt::darkCyan));
		msg = "Devel ";
		break;
	default:
		setColor(QColor(Qt::gray));
		msg = "Prog. ";
		break;
	}
	msg += ": ";
	if (level != 2) msg += QDateTime::currentDateTime().toString("dd MMM hh:mm:ss.zzz") + " -> ";
	msg += txt;
	if (msg.right(2) == "/n") msg.chop(2);

	if (!remote_only) {
		if (log_line_cnt++ > MAX_LOG_LINES) {
			emit clearlog();
			log_line_cnt = 0;
			emit newtext(trUtf8("Log display cleared (exceeds max lines limit)\n\n"));
		}
		emit newtext(msg);

		msg += "\n";
		logfile_mutex.lock();
		if (logfileEnabled) {
			logfile.write(msg.toUtf8());
		}
		logfile_mutex.unlock();
	}
}

void Log::flushLogfile()
{
	logfile_mutex.lock();
	if (logfileEnabled) {
		logfile.flush();
	}
	logfile_mutex.unlock();

}

void Log::appendLogError(const QString & txt, int level)
{
	do_append_log_error(txt, level, MSG_LOGERR, false);
}


void Log::do_append_log_error(const QString & txt, int level, quint32 type, bool remote_only)
{
	Q_UNUSED(type);

	setColor(QColor(Qt::darkRed));
	QString msg;
	switch (level) {
	case 1:
		msg = "Kernel";
		break;
	case 2:
		msg = "VarLog";
		break;
	case 3:
		msg = "Debug ";
		break;
	default:
		msg = "Prog. ";
		break;
	}
	msg += ": ";
	if (level != 2) msg += QDateTime::currentDateTime().toString("dd MMM hh:mm:ss.zzz") + " -> ";
	msg += txt;

	if (msg.right(2) == "/n") msg.chop(2);

	if (!remote_only) {
		emit newtext(msg);

		msg += "\n";
		logfile_mutex.lock();
		if (logfileEnabled) {
			logfile.write("E_");
			logfile.write(msg.toUtf8());
		}
		logfile_mutex.unlock();
	}
}

void Log::appendLogStdError(const QString & where, int e_num , const QString & para)
{
	QString text = where + ": " + error_msg_asc[e_num];
	if (para.size()) {
		text += " (";
		text += para;
		text += ")";
	}
	do_append_log_error(text,0,MSG_LOGERR,false);
}

void Log::appendDebugText(const char *str,...)
{
	QString thread = getCurrentThreadName();
	char temp[1000];
	int pn = thread.size();
	if (pn) {
		qstrcpy(temp,thread.toLocal8Bit().data());
	}

	va_list varg_p;
	va_start(varg_p,str);

	qvsnprintf (temp+pn,950,str,varg_p);
	qDebug(temp);
	va_end(varg_p);
	do_append_log_text(QString(temp),3, MSG_DEBUGTXT, false);

}

void Log::appendDevelText(const char *str,...)
{
	QString thread = getCurrentThreadName();
	char temp[1000];
	int pn = thread.size();
	if (pn) {
		qstrcpy(temp,thread.toLocal8Bit().data());
	}

	va_list varg_p;
	va_start(varg_p,str);

	qvsnprintf (temp+pn,950,str,varg_p);
	qDebug(temp);
	va_end(varg_p);
	do_append_log_text(QString(temp),5, MSG_DEVELTXT,false);

}

void Log::appendDebugError(const char *str,...)
{
	QString thread = getCurrentThreadName();
	char temp[1000];
	int pn = thread.size();
	if (pn) {
		qstrcpy(temp,thread.toLocal8Bit().data());
	}

	va_list varg_p;
	va_start(varg_p,str);
	qvsnprintf (temp+pn,950,str,varg_p);
	qDebug (temp);
	va_end(varg_p);
	do_append_log_error(QString(temp),3, MSG_DEBUGERR, false);
}

void Log::debugCurrentThread()
{
	QThread *cur_thread = currentThread();
	if (cur_thread) {
		QString oname = cur_thread->objectName();
		if (oname.size()) qDebug ("     <- %s",oname.toUtf8().data());
	}
}


QString Log::getCurrentThreadName()
{
	QThread *cur_thread = currentThread();
	QString thread_txt;
	if (cur_thread) {
		thread_txt = cur_thread->objectName();
		if (thread_txt.size()) thread_txt  += ": ";
	}
	return thread_txt;
}


/**
 * \brief Farbe der Logmeldungen setzen
 * @param col QColor
 */
void Log::setColor(const QColor &col)
{
	if (1/*col != color*/) { ///< @todo
		if (!loggingEnabled) return;
		emit newcolor(col);
		color = col;
	}
}

/**
 * \brief Logmessage Ziel setzen
 * @param wid Pointer auf Widget, an das die Signale mit den Logmeldungen geschickt werden soll
 * @return QWidget Pointer auf das bisherige Ausgebeziel der Logmeldungen (bzw. NULL, falls neu)
 *
 * Das Empfänger Widget muss die Slots "append(const QString &)" und "setTextColor(cons QColor &)" besitzen,
 * ist also sinnvollerweise eine QTextEdit Instanz oder ein Objekt einer eigenen Klasse, die diese Slots vorweisen kann.
 */
QWidget * Log::setStatusWidget(QWidget * wid)
{
	QWidget * old_widget = 0;
	if (statuswid) {
		this->disconnect();
		old_widget = statuswid;
	}
	statuswid = (QTextEdit *)wid;
	if (statuswid != 0) {

		loggingEnabled = true;
		statuswid->setUndoRedoEnabled(false);
		connect(this,SIGNAL(newtext(const QString &)),statuswid,SLOT(append(const QString &)),Qt::QueuedConnection);
		connect(this,SIGNAL(newcolor(const QColor &)),statuswid,SLOT(setTextColor(const QColor &)),Qt::QueuedConnection);
		connect(this,SIGNAL(clearlog()),statuswid,SLOT(clear()),Qt::QueuedConnection);
	}
	else {
		loggingEnabled = false;
	}

	return old_widget;
}



/*!
	\fn Log::openLogfileAppend()
 */
bool Log::openLogfileAppend()
{
	bool ok = true;
	if (logfileEnabled) closeLogfile();

	logfile_mutex.lock();
	logfile.setFileName(LOG_FILE_PATH);
	if (logfile.open(QIODevice::Append)) {
		logfileEnabled = true;
		ok = true;
	} else {
		ok = false;
	}
	logfile_mutex.unlock();
	return ok;
}


/*!
	\fn Log::closeLogfile()
 */
void Log::closeLogfile()
{
	logfile_mutex.lock();
	if (logfileEnabled) {
		logfileEnabled = false;
		logfile.close();
	}
	logfile_mutex.unlock();
}

void Log::errorPopupMsg(const QString & where, const QString & text)
{
	// Für Log eine Kopie der Meldung machen, die keine Zeilensprünge enthält
	QString logtext = text;
	logtext.replace("\n"," > ");
	LOGERROR(QString("%1: %2").arg(where,logtext));
	emit errorMsgReceived(where, text);
}
void Log::infoPopupMsg(const QString & where, const QString & text)
{
	// Für Log eine Kopie der Meldung machen, die keine Zeilensprünge enthält
	QString logtext = text;
	logtext.replace("\n"," > ");
	LOGTEXT(QString("%1: %2").arg(where,logtext));
	emit infoMsgReceived(where, text);
}



bool Log::rotateLog()
{
	bool rotate = true;

	QStringList messages;

	logfile_mutex.lock();

	if (logfileEnabled) {
		if (logfile.size() >= MAX_LOG_SIZE) {
			logfile.close();
			QString old_path = LOG_FILE_PATH;
			QString log_path = old_path;
			old_path += ".old";
			if ( QFile::exists(old_path) ) {
				if (QFile::remove(old_path) ) {
					messages.append(trUtf8("Logfile: Remove: '%1'\n").arg(old_path));
				} else {
					messages.append(trUtf8("Logfile: Could not remove: '%1'\n").arg(old_path));
				}
			}

			if ( QFile::exists(log_path) ) {
				if (QFile::rename(log_path,old_path) ) {
					messages.append(trUtf8("Logfile: Rename: '%1' to '%2'\n").arg(log_path).arg(old_path));
					rotate = true;
				} else {
					messages.append(trUtf8("Logfile: Could not rename: '%1'\n").arg(log_path));
					rotate = false;
				}
			}

			if (logfile.open(QIODevice::Append)) {
				logfileEnabled = true;
				rotate = true;
			} else {
				logfileEnabled = false;
				rotate = false;
			}
		}
	}
	logfile_mutex.unlock();

	for (int t=0;t<messages.size();t++) {
		LOGTEXT(messages.at(t));
	}

	return rotate;
}

int Log::readAppendCurrentLogFile()
{
	int lines = 0;
	lines += readAppendLogFile(QString("%1.old").arg(LOG_FILE_PATH));
	lines += readAppendLogFile(QString(LOG_FILE_PATH));
	return lines;
}

int Log::readAppendLogFile(const QString & path)
{
	emit newtext(QString("Read StageRunner Log ---------------------------------------- %1").arg(path));
	QFile file(path);
	QProgressDialog progress(trUtf8("Load StageRunner log files..."),trUtf8("Cancel"),0,file.size());
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	int count = 0;
	int lines = 0;
	int update = 0;
	if (file.open(QIODevice::ReadOnly)) {
		while (!file.atEnd() && !progress.wasCanceled()) {
			QByteArray line = file.readLine();
			count += line.size();
			lines++;
			QString msg = QString::fromUtf8(line);
			process_line_for_log_output(msg);
			msg.chop(1);
			emit newtext(msg);
			if (++update > 50) {
				update = 0;
				progress.setValue(count);
				if (QThread::currentThread() == main_thread) QApplication::processEvents();
			}
		}
	} else {
		DEBUGERROR("Log::readAppendLogFile: Could not open logfile: %s",file.errorString().toLatin1().data());
		lines = -1;
	}

	return lines;
}

void Log::process_line_for_log_output(QString & msg)
{
	if (msg.left(2) == "Ke") {
		setColor(QColor(Qt::blue));
	}
	else if (msg.left(2) == "Va") {
		setColor(QColor(Qt::darkGreen));
	}
	else if (msg.left(3) == "Dev") {
		setColor(QColor(Qt::darkCyan));
	}
	else if (msg.left(3) == "Deb") {
		setColor(QColor(Qt::darkGray));
	}
	else if (msg.left(2) == "St") {
		setColor(QColor(Qt::darkBlue));
	}
	else if (msg.left(4) == "E_Ke") {
		msg = msg.mid(2);
		setColor(QColor(Qt::darkRed));
	}
	else if (msg.left(4) == "E_Va") {
		msg = msg.mid(2);
		setColor(QColor(Qt::darkRed));
	}
	else if (msg.left(4) == "E_De") {
		msg = msg.mid(2);
		setColor(QColor(Qt::darkRed));
	}
	else if (msg.left(4) == "E_St") {
		msg = msg.mid(2);
		setColor(QColor(Qt::darkRed));
	}
	else if (msg.left(4) == "E_Pr") {
		msg = msg.mid(2);
		setColor(QColor(Qt::darkRed));
	}
	else {
		setColor(QColor(Qt::gray));
	}

}
