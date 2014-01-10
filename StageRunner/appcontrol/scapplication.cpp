#include "scapplication.h"
#include "unixsignalcatcher.h"

#include <QApplication>
// #include <typeinfo>

ScApplication::ScApplication(int &argc, char **argv)
	: QApplication(argc,argv)
{
	int ret = UnixSignalCatcher::installUnixSignalHandlers();
	qDebug("Install signal handler: %d",ret);

	mySignalCatcher = new UnixSignalCatcher;

}

ScApplication::~ScApplication()
{
	delete mySignalCatcher;
}

//bool ScApplication::notify(QObject *receiver, QEvent *event)
//{
//	try {
//		return QApplication::notify(receiver, event);
//	} catch (std::exception &e) {
//		qFatal("Error %s sending event %s to object %s (%s)",
//			   e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
//			   typeid(*receiver).name());
//	}
//	catch (...) {
//		qFatal("Error <unknown> sending event %s to object %s (%s)",
//			   typeid(*event).name(), qPrintable(receiver->objectName()),
//			   typeid(*receiver).name());
//	}

//	// qFatal aborts, so this isn't really necessary
//	// but you might continue if you use a different logging lib
//	return false;
//}
