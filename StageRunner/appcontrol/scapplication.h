#ifndef SCAPPLICATION_H
#define SCAPPLICATION_H

#include <QApplication>

class UnixSignalCatcher;

class ScApplication : public QApplication
{
	Q_OBJECT
private:
	UnixSignalCatcher *mySignalCatcher;

public:
	ScApplication(int &argc, char **argv);
	~ScApplication();
	// bool notify(QObject *receiver, QEvent *event);

signals:

public slots:

};

#endif // SCAPPLICATION_H
