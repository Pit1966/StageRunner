#ifndef QT_VERSIONS_H
#define QT_VERSIONS_H

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)

QT_BEGIN_NAMESPACE

enum SplitBehaviorFlags {
	KeepEmptyParts = 0,
	SkipEmptyParts = 0x1,
};

QT_END_NAMESPACE

typedef QMutex QRecursiveMutex;
#endif

#endif // QT_VERSIONS_H
