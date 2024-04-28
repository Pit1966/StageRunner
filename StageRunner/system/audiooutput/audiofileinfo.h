#ifndef AUDIOFILEINFO_H
#define AUDIOFILEINFO_H

#include <QObject>

class QProcess;

class AudioFileInfo : public QObject
{
	Q_OBJECT
private:
	QString m_file;
	QProcess * m_proc	= nullptr;

	int m_durationMs	= 0;

public:
	explicit AudioFileInfo();
	explicit AudioFileInfo(const QString &filePath);
	~AudioFileInfo();

	bool inspectFile();
	int getDurationMs();

signals:

};

#endif // AUDIOFILEINFO_H
