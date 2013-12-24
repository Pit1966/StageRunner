/****************************************************************************
** Meta object code from reading C++ file 'fxcontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/system/fxcontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fxcontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxControl_t {
    QByteArrayData data[12];
    char stringdata[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FxControl_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FxControl_t qt_meta_stringdata_FxControl = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 15),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 9),
QT_MOC_LITERAL(4, 37, 4),
QT_MOC_LITERAL(5, 42, 20),
QT_MOC_LITERAL(6, 63, 15),
QT_MOC_LITERAL(7, 79, 15),
QT_MOC_LITERAL(8, 95, 6),
QT_MOC_LITERAL(9, 102, 23),
QT_MOC_LITERAL(10, 126, 12),
QT_MOC_LITERAL(11, 139, 7)
    },
    "FxControl\0executerChanged\0\0Executer*\0"
    "exec\0startFxAudioPlayList\0FxListExecuter*\0"
    "FxPlayListItem*\0fxplay\0continueFxAudioPlayList\0"
    "FxAudioItem*\0fxaudio\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxControl[] = {

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
       5,    1,   32,    2, 0x0a,
       9,    2,   35,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    0x80000000 | 6, 0x80000000 | 7,    8,
    0x80000000 | 6, 0x80000000 | 7, 0x80000000 | 10,    8,   11,

       0        // eod
};

void FxControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxControl *_t = static_cast<FxControl *>(_o);
        switch (_id) {
        case 0: _t->executerChanged((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 1: { FxListExecuter* _r = _t->startFxAudioPlayList((*reinterpret_cast< FxPlayListItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< FxListExecuter**>(_a[0]) = _r; }  break;
        case 2: { FxListExecuter* _r = _t->continueFxAudioPlayList((*reinterpret_cast< FxPlayListItem*(*)>(_a[1])),(*reinterpret_cast< FxAudioItem*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< FxListExecuter**>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FxControl::*_t)(Executer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxControl::executerChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject FxControl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FxControl.data,
      qt_meta_data_FxControl,  qt_static_metacall, 0, 0}
};


const QMetaObject *FxControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FxControl.stringdata))
        return static_cast<void*>(const_cast< FxControl*>(this));
    return QObject::qt_metacast(_clname);
}

int FxControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void FxControl::executerChanged(Executer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
