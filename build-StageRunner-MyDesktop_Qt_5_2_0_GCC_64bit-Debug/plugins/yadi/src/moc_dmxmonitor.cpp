/****************************************************************************
** Meta object code from reading C++ file 'dmxmonitor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../StageRunner/plugins/yadi/src/dmxmonitor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dmxmonitor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DmxMonitor_t {
    QByteArrayData data[10];
    char stringdata[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_DmxMonitor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_DmxMonitor_t qt_meta_stringdata_DmxMonitor = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 13),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 11),
QT_MOC_LITERAL(4, 38, 8),
QT_MOC_LITERAL(5, 47, 13),
QT_MOC_LITERAL(6, 61, 3),
QT_MOC_LITERAL(7, 65, 5),
QT_MOC_LITERAL(8, 71, 25),
QT_MOC_LITERAL(9, 97, 8)
    },
    "DmxMonitor\0monitorClosed\0\0DmxMonitor*\0"
    "instance\0setValueInBar\0bar\0value\0"
    "setDetectedChannelsNumber\0channels\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DmxMonitor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    2,   32,    2, 0x0a,
       8,    1,   37,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UChar,    6,    7,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void DmxMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DmxMonitor *_t = static_cast<DmxMonitor *>(_o);
        switch (_id) {
        case 0: _t->monitorClosed((*reinterpret_cast< DmxMonitor*(*)>(_a[1]))); break;
        case 1: _t->setValueInBar((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< uchar(*)>(_a[2]))); break;
        case 2: _t->setDetectedChannelsNumber((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DmxMonitor* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DmxMonitor::*_t)(DmxMonitor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DmxMonitor::monitorClosed)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject DmxMonitor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DmxMonitor.data,
      qt_meta_data_DmxMonitor,  qt_static_metacall, 0, 0}
};


const QMetaObject *DmxMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DmxMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DmxMonitor.stringdata))
        return static_cast<void*>(const_cast< DmxMonitor*>(this));
    return QWidget::qt_metacast(_clname);
}

int DmxMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DmxMonitor::monitorClosed(DmxMonitor * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
