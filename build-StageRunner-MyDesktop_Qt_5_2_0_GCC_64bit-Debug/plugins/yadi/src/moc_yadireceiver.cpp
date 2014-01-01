/****************************************************************************
** Meta object code from reading C++ file 'yadireceiver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../StageRunner/plugins/yadi/src/yadireceiver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'yadireceiver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_YadiReceiver_t {
    QByteArrayData data[13];
    char stringdata[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_YadiReceiver_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_YadiReceiver_t qt_meta_stringdata_YadiReceiver = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 23),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 5),
QT_MOC_LITERAL(4, 44, 17),
QT_MOC_LITERAL(5, 62, 3),
QT_MOC_LITERAL(6, 66, 25),
QT_MOC_LITERAL(7, 92, 7),
QT_MOC_LITERAL(8, 100, 5),
QT_MOC_LITERAL(9, 106, 19),
QT_MOC_LITERAL(10, 126, 23),
QT_MOC_LITERAL(11, 150, 8),
QT_MOC_LITERAL(12, 159, 17)
    },
    "YadiReceiver\0exitReceiverWithFailure\0"
    "\0input\0dmxPacketReceived\0msg\0"
    "dmxInDeviceChannelChanged\0channel\0"
    "value\0dmxInChannelChanged\0"
    "rxDmxPacketSizeReceived\0channels\0"
    "dmxStatusReceived\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_YadiReceiver[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06,
       4,    1,   47,    2, 0x06,
       6,    3,   50,    2, 0x06,
       9,    2,   57,    2, 0x06,
      10,    1,   62,    2, 0x06,
      12,    1,   65,    2, 0x06,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UChar,    3,    7,    8,
    QMetaType::Void, QMetaType::UInt, QMetaType::UChar,    7,    8,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::UChar,    2,

       0        // eod
};

void YadiReceiver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        YadiReceiver *_t = static_cast<YadiReceiver *>(_o);
        switch (_id) {
        case 0: _t->exitReceiverWithFailure((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->dmxPacketReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->dmxInDeviceChannelChanged((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< uchar(*)>(_a[3]))); break;
        case 3: _t->dmxInChannelChanged((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< uchar(*)>(_a[2]))); break;
        case 4: _t->rxDmxPacketSizeReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->dmxStatusReceived((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (YadiReceiver::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::exitReceiverWithFailure)) {
                *result = 0;
            }
        }
        {
            typedef void (YadiReceiver::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::dmxPacketReceived)) {
                *result = 1;
            }
        }
        {
            typedef void (YadiReceiver::*_t)(quint32 , quint32 , uchar );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::dmxInDeviceChannelChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (YadiReceiver::*_t)(quint32 , uchar );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::dmxInChannelChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (YadiReceiver::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::rxDmxPacketSizeReceived)) {
                *result = 4;
            }
        }
        {
            typedef void (YadiReceiver::*_t)(quint8 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&YadiReceiver::dmxStatusReceived)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject YadiReceiver::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_YadiReceiver.data,
      qt_meta_data_YadiReceiver,  qt_static_metacall, 0, 0}
};


const QMetaObject *YadiReceiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *YadiReceiver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_YadiReceiver.stringdata))
        return static_cast<void*>(const_cast< YadiReceiver*>(this));
    return QThread::qt_metacast(_clname);
}

int YadiReceiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void YadiReceiver::exitReceiverWithFailure(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void YadiReceiver::dmxPacketReceived(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void YadiReceiver::dmxInDeviceChannelChanged(quint32 _t1, quint32 _t2, uchar _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void YadiReceiver::dmxInChannelChanged(quint32 _t1, uchar _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void YadiReceiver::rxDmxPacketSizeReceived(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void YadiReceiver::dmxStatusReceived(quint8 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
