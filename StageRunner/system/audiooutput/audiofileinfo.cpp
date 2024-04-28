#include "audiofileinfo.h"

#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

AudioFileInfo::AudioFileInfo()
	: QObject()
{
}

AudioFileInfo::AudioFileInfo(const QString &filePath)
	: QObject()
	, m_file(filePath)
{
}

AudioFileInfo::~AudioFileInfo()
{
	delete m_proc;
}

bool AudioFileInfo::inspectFile()
{
	if (!m_proc)
		m_proc = new QProcess(this);

	QString prog = "mediainfo";

	QStringList paras;
	paras << "--output=JSON" << m_file;

	m_proc->start(prog, paras);
	if (!m_proc->waitForFinished(2000))
		return false;

	if (m_proc->exitCode() != 0) {
		qDebug() << "mediainfo fail exit" << m_proc->readAllStandardError();
		return false;
	}

	QByteArray result = m_proc->readAllStandardOutput();

	QJsonObject json = QJsonDocument::fromJson(result).object();
	QJsonValue media = json.value("media");
	if (media.isUndefined())
		return false;
	QJsonValue tracks = media.toObject().value("track");
	if (tracks.isUndefined())
		return false;
	QJsonValue track = tracks.toArray().at(1);
	if (track.isUndefined())
		return false;

	QJsonValue duration = track.toObject().value("Duration");
	if (duration.isUndefined())
		return false;

	m_durationMs = duration.toString().toDouble() * 1000;

	return true;
}

int AudioFileInfo::getDurationMs()
{
	return m_durationMs;
}
