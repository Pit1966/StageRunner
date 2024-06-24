#ifndef QT6_QT5_COMPAT_H
#define QT6_QT5_COMPAT_H

#include <Qt>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#ifdef IS_QT6

#else


#endif


/**
 * @brief This is an abstraction layer class for QMouseEvent in order to provide
 * Qt5 -> Qt6 compatibility functions
 */
class PMouseEvent : public QMouseEvent
{

public:
#ifdef IS_QT6
	// x() is deprecated since Qt 6.0
	int x() const {
		return qRound(position().x());
	}
#endif
};

/**
 * @brief This is an abstraction layer class for QMouseEvent in order to provide
 * Qt5 -> Qt6 compatibility functions
 */
class PDragEnterEvent : public QDragEnterEvent
{
public:
#ifdef IS_QT6
	QPoint pos() const {
		return position().toPoint();
	}
#endif
};

/**
 * @brief This is an abstraction layer class for QMouseEvent in order to provide
 * Qt5 -> Qt6 compatibility functions
 */
class PDragMoveEvent : public QDragMoveEvent
{
public:
#ifdef IS_QT6
	QPoint pos() const {
		return position().toPoint();
	}
#endif
};

/**
 * @brief This is an abstraction layer class for QMouseEvent in order to provide
 * Qt5 -> Qt6 compatibility functions
 */
class PDropEvent : public QDropEvent
{
public:
#ifdef IS_QT6
	QPoint pos() const {
		return position().toPoint();
	}
#endif
};



#endif // QT6_QT5_COMPAT_H
