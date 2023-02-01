#ifndef QT_VERSIONS_H
#define QT_VERSIONS_H

#include <QtCore/qglobal.h>
#include <QMutex>
#include <QString>

#if QT_VERSION_MAJOR >= 6
#define QT_MIDDLE_BUTTON Qt::MiddleButton
#else
#define QT_MIDDLE_BUTTON Qt::MidButton
#endif


#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)

#define QT_SKIP_EMPTY_PARTS QString::SkipEmptyParts

class QRecursiveMutex : public QMutex
{
public:
	QRecursiveMutex()
		: QMutex(QMutex::Recursive)
	{}
};


#else

#define QT_SKIP_EMPTY_PARTS Qt::SkipEmptyParts

#endif

#endif // QT_VERSIONS_H
