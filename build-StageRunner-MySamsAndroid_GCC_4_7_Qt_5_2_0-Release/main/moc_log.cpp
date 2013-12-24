/****************************************************************************
** Meta object code from reading C++ file 'log.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/system/log.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'log.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Log_t {
    QByteArrayData data[12];
    char stringdata[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Log_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Log_t qt_meta_stringdata_Log = {
    {
QT_MOC_LITERAL(0, 0, 3),
QT_MOC_LITERAL(1, 4, 7),
QT_MOC_LITERAL(2, 12, 0),
QT_MOC_LITERAL(3, 13, 8),
QT_MOC_LITERAL(4, 22, 8),
QT_MOC_LITERAL(5, 31, 9),
QT_MOC_LITERAL(6, 41, 15),
QT_MOC_LITERAL(7, 57, 4),
QT_MOC_LITERAL(8, 62, 4),
QT_MOC_LITERAL(9, 67, 16),
QT_MOC_LITERAL(10, 84, 10),
QT_MOC_LITERAL(11, 95, 10)
    },
    "Log\0newtext\0\0newcolor\0clearlog\0msgLogged\0"
    "infoMsgReceived\0func\0text\0errorMsgReceived\0"
    "appendText\0terminated\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Log[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06,
       3,    1,   57,    2, 0x06,
       4,    0,   60,    2, 0x06,
       5,    2,   61,    2, 0x06,
       6,    2,   66,    2, 0x06,
       9,    2,   71,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    1,   76,    2, 0x0a,
      11,    0,   79,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

       0        // eod
};

void Log::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Log *_t = static_cast<Log *>(_o);
        switch (_id) {
        case 0: _t->newtext((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->newcolor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 2: _t->clearlog(); break;
        case 3: _t->msgLogged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 4: _t->infoMsgReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->errorMsgReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->appendText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->terminated(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Log::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::newtext)) {
                *result = 0;
            }
        }
        {
            typedef void (Log::*_t)(const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::newcolor)) {
                *result = 1;
            }
        }
        {
            typedef void (Log::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::clearlog)) {
                *result = 2;
            }
        }
        {
            typedef void (Log::*_t)(const QString , quint32 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::msgLogged)) {
                *result = 3;
            }
        }
        {
            typedef void (Log::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::infoMsgReceived)) {
                *result = 4;
            }
        }
        {
            typedef void (Log::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Log::errorMsgReceived)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject Log::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Log.data,
      qt_meta_data_Log,  qt_static_metacall, 0, 0}
};


const QMetaObject *Log::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Log::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Log.stringdata))
        return static_cast<void*>(const_cast< Log*>(this));
    return QThread::qt_metacast(_clname);
}

int Log::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Log::newtext(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Log::newcolor(const QColor & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Log::clearlog()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Log::msgLogged(const QString _t1, quint32 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Log::infoMsgReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Log::errorMsgReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
