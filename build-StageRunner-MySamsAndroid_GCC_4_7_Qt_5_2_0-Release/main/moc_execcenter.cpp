/****************************************************************************
** Meta object code from reading C++ file 'execcenter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/fx/execcenter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'execcenter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ExecCenter_t {
    QByteArrayData data[10];
    char stringdata[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ExecCenter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ExecCenter_t qt_meta_stringdata_ExecCenter = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 15),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 9),
QT_MOC_LITERAL(4, 38, 15),
QT_MOC_LITERAL(5, 54, 15),
QT_MOC_LITERAL(6, 70, 14),
QT_MOC_LITERAL(7, 85, 4),
QT_MOC_LITERAL(8, 90, 17),
QT_MOC_LITERAL(9, 108, 19)
    },
    "ExecCenter\0executerCreated\0\0Executer*\0"
    "executerDeleted\0executerChanged\0"
    "deleteExecuter\0exec\0test_remove_queue\0"
    "on_executer_changed\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ExecCenter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06,
       4,    1,   47,    2, 0x06,
       5,    1,   50,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       6,    1,   53,    2, 0x0a,
       8,    0,   56,    2, 0x08,
       9,    1,   57,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    7,

       0        // eod
};

void ExecCenter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ExecCenter *_t = static_cast<ExecCenter *>(_o);
        switch (_id) {
        case 0: _t->executerCreated((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 1: _t->executerDeleted((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 2: _t->executerChanged((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 3: _t->deleteExecuter((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 4: _t->test_remove_queue(); break;
        case 5: _t->on_executer_changed((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ExecCenter::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExecCenter::executerCreated)) {
                *result = 0;
            }
        }
        {
            typedef void (ExecCenter::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExecCenter::executerDeleted)) {
                *result = 1;
            }
        }
        {
            typedef void (ExecCenter::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExecCenter::executerChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject ExecCenter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ExecCenter.data,
      qt_meta_data_ExecCenter,  qt_static_metacall, 0, 0}
};


const QMetaObject *ExecCenter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExecCenter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExecCenter.stringdata))
        return static_cast<void*>(const_cast< ExecCenter*>(this));
    return QObject::qt_metacast(_clname);
}

int ExecCenter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void ExecCenter::executerCreated(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ExecCenter::executerDeleted(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ExecCenter::executerChanged(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
