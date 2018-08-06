#include "scapplication.h"

#ifdef __unix__
# include "unixsignalcatcher.h"
#endif

#include <QApplication>
// #include <typeinfo>

ScApplication::ScApplication(int &argc, char **argv)
	: QApplication(argc,argv)
{
#ifdef __unix__
	int ret = UnixSignalCatcher::installUnixSignalHandlers();
	qDebug("Install signal handler: %d",ret);

	mySignalCatcher = new UnixSignalCatcher;
#endif

}

ScApplication::~ScApplication()
{
#ifdef __unix__
	delete mySignalCatcher;
#endif
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
