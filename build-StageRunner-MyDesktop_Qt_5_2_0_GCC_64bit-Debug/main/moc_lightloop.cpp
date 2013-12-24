/****************************************************************************
** Meta object code from reading C++ file 'lightloop.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/appcontrol/lightloop.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lightloop.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LightLoop_t {
    QByteArrayData data[14];
    char stringdata[189];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_LightLoop_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_LightLoop_t qt_meta_stringdata_LightLoop = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 18),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 12),
QT_MOC_LITERAL(4, 43, 5),
QT_MOC_LITERAL(5, 49, 4),
QT_MOC_LITERAL(6, 54, 24),
QT_MOC_LITERAL(7, 79, 9),
QT_MOC_LITERAL(8, 89, 9),
QT_MOC_LITERAL(9, 99, 13),
QT_MOC_LITERAL(10, 113, 17),
QT_MOC_LITERAL(11, 131, 16),
QT_MOC_LITERAL(12, 148, 20),
QT_MOC_LITERAL(13, 169, 18)
    },
    "LightLoop\0sceneStatusChanged\0\0"
    "FxSceneItem*\0scene\0stat\0"
    "sceneFadeProgressChanged\0perMilleA\0"
    "perMilleB\0sceneCueReady\0startProcessTimer\0"
    "stopProcessTimer\0processPendingEvents\0"
    "processFxSceneItem\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LightLoop[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06,
       6,    3,   54,    2, 0x06,
       9,    1,   61,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    0,   64,    2, 0x0a,
      11,    0,   65,    2, 0x0a,
      12,    0,   66,    2, 0x0a,
      13,    1,   67,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::UInt,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    4,    7,    8,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, 0x80000000 | 3,    4,

       0        // eod
};

void LightLoop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LightLoop *_t = static_cast<LightLoop *>(_o);
        switch (_id) {
        case 0: _t->sceneStatusChanged((*reinterpret_cast< FxSceneItem*(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 1: _t->sceneFadeProgressChanged((*reinterpret_cast< FxSceneItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->sceneCueReady((*reinterpret_cast< FxSceneItem*(*)>(_a[1]))); break;
        case 3: _t->startProcessTimer(); break;
        case 4: _t->stopProcessTimer(); break;
        case 5: _t->processPendingEvents(); break;
        case 6: { bool _r = _t->processFxSceneItem((*reinterpret_cast< FxSceneItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LightLoop::*_t)(FxSceneItem * , quint32 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LightLoop::sceneStatusChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (LightLoop::*_t)(FxSceneItem * , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LightLoop::sceneFadeProgressChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (LightLoop::*_t)(FxSceneItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LightLoop::sceneCueReady)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject LightLoop::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LightLoop.data,
      qt_meta_data_LightLoop,  qt_static_metacall, 0, 0}
};


const QMetaObject *LightLoop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LightLoop::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LightLoop.stringdata))
        return static_cast<void*>(const_cast< LightLoop*>(this));
    return QObject::qt_metacast(_clname);
}

int LightLoop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LightLoop::sceneStatusChanged(FxSceneItem * _t1, quint32 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LightLoop::sceneFadeProgressChanged(FxSceneItem * _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LightLoop::sceneCueReady(FxSceneItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
