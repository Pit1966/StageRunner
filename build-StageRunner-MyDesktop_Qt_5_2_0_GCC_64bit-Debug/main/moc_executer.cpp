/****************************************************************************
** Meta object code from reading C++ file 'executer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/fx/executer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'executer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Executer_t {
    QByteArrayData data[10];
    char stringdata[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Executer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Executer_t qt_meta_stringdata_Executer = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 8),
QT_MOC_LITERAL(2, 18, 0),
QT_MOC_LITERAL(3, 19, 9),
QT_MOC_LITERAL(4, 29, 4),
QT_MOC_LITERAL(5, 34, 7),
QT_MOC_LITERAL(6, 42, 16),
QT_MOC_LITERAL(7, 59, 7),
QT_MOC_LITERAL(8, 67, 2),
QT_MOC_LITERAL(9, 70, 13)
    },
    "Executer\0deleteMe\0\0Executer*\0exec\0"
    "changed\0currentFxChanged\0FxItem*\0fx\0"
    "nextFxChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Executer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06,
       5,    1,   37,    2, 0x06,
       6,    1,   40,    2, 0x06,
       9,    1,   43,    2, 0x06,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void Executer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Executer *_t = static_cast<Executer *>(_o);
        switch (_id) {
        case 0: _t->deleteMe((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 1: _t->changed((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 2: _t->currentFxChanged((*reinterpret_cast< FxItem*(*)>(_a[1]))); break;
        case 3: _t->nextFxChanged((*reinterpret_cast< FxItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Executer* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Executer* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Executer::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Executer::deleteMe)) {
                *result = 0;
            }
        }
        {
            typedef void (Executer::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Executer::changed)) {
                *result = 1;
            }
        }
        {
            typedef void (Executer::*_t)(FxItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Executer::currentFxChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (Executer::*_t)(FxItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Executer::nextFxChanged)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Executer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Executer.data,
      qt_meta_data_Executer,  qt_static_metacall, 0, 0}
};


const QMetaObject *Executer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Executer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Executer.stringdata))
        return static_cast<void*>(const_cast< Executer*>(this));
    return QObject::qt_metacast(_clname);
}

int Executer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Executer::deleteMe(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Executer::changed(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Executer::currentFxChanged(FxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Executer::nextFxChanged(FxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_FxListExecuter_t {
    QByteArrayData data[9];
    char stringdata[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FxListExecuter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FxListExecuter_t qt_meta_stringdata_FxListExecuter = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 17),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 12),
QT_MOC_LITERAL(4, 47, 3),
QT_MOC_LITERAL(5, 51, 17),
QT_MOC_LITERAL(6, 69, 12),
QT_MOC_LITERAL(7, 82, 3),
QT_MOC_LITERAL(8, 86, 16)
    },
    "FxListExecuter\0audioCtrlReceiver\0\0"
    "AudioCtrlMsg\0msg\0lightCtrlReceiver\0"
    "FxSceneItem*\0fxs\0sceneCueReceiver\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxListExecuter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x09,
       5,    1,   32,    2, 0x09,
       8,    1,   35,    2, 0x09,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void FxListExecuter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxListExecuter *_t = static_cast<FxListExecuter *>(_o);
        switch (_id) {
        case 0: _t->audioCtrlReceiver((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 1: _t->lightCtrlReceiver((*reinterpret_cast< FxSceneItem*(*)>(_a[1]))); break;
        case 2: _t->sceneCueReceiver((*reinterpret_cast< FxSceneItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject FxListExecuter::staticMetaObject = {
    { &Executer::staticMetaObject, qt_meta_stringdata_FxListExecuter.data,
      qt_meta_data_FxListExecuter,  qt_static_metacall, 0, 0}
};


const QMetaObject *FxListExecuter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxListExecuter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FxListExecuter.stringdata))
        return static_cast<void*>(const_cast< FxListExecuter*>(this));
    return Executer::qt_metacast(_clname);
}

int FxListExecuter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Executer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
