/****************************************************************************
** Meta object code from reading C++ file 'appcentral.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/appcontrol/appcentral.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appcentral.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppCentral_t {
    QByteArrayData data[30];
    char stringdata[358];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AppCentral_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AppCentral_t qt_meta_stringdata_AppCentral = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 19),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 12),
QT_MOC_LITERAL(4, 45, 3),
QT_MOC_LITERAL(5, 49, 15),
QT_MOC_LITERAL(6, 65, 5),
QT_MOC_LITERAL(7, 71, 22),
QT_MOC_LITERAL(8, 94, 13),
QT_MOC_LITERAL(9, 108, 7),
QT_MOC_LITERAL(10, 116, 12),
QT_MOC_LITERAL(11, 129, 2),
QT_MOC_LITERAL(12, 132, 7),
QT_MOC_LITERAL(13, 140, 3),
QT_MOC_LITERAL(14, 144, 9),
QT_MOC_LITERAL(15, 154, 4),
QT_MOC_LITERAL(16, 159, 13),
QT_MOC_LITERAL(17, 173, 6),
QT_MOC_LITERAL(18, 180, 16),
QT_MOC_LITERAL(19, 197, 18),
QT_MOC_LITERAL(20, 216, 11),
QT_MOC_LITERAL(21, 228, 20),
QT_MOC_LITERAL(22, 249, 17),
QT_MOC_LITERAL(23, 267, 3),
QT_MOC_LITERAL(24, 271, 7),
QT_MOC_LITERAL(25, 279, 29),
QT_MOC_LITERAL(26, 309, 8),
QT_MOC_LITERAL(27, 318, 5),
QT_MOC_LITERAL(28, 324, 27),
QT_MOC_LITERAL(29, 352, 4)
    },
    "AppCentral\0audioCtrlMsgEmitted\0\0"
    "AudioCtrlMsg\0msg\0editModeChanged\0state\0"
    "inputAssignModeChanged\0inputAssigned\0"
    "FxItem*\0executeFxCmd\0fx\0CtrlCmd\0cmd\0"
    "Executer*\0exec\0executeNextFx\0listID\0"
    "moveToFollowerFx\0moveToForeRunnerFx\0"
    "setEditMode\0setExperimentalAudio\0"
    "testSetDmxChannel\0val\0channel\0"
    "onInputUniverseChannelChanged\0universe\0"
    "value\0storeSelectedFxListWidgetFx\0"
    "item\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppCentral[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06,
       5,    1,   82,    2, 0x06,
       7,    1,   85,    2, 0x06,
       8,    1,   88,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    3,   91,    2, 0x0a,
      16,    1,   98,    2, 0x0a,
      18,    1,  101,    2, 0x0a,
      19,    1,  104,    2, 0x0a,
      20,    1,  107,    2, 0x0a,
      21,    1,  110,    2, 0x0a,
      22,    2,  113,    2, 0x0a,
      25,    3,  118,    2, 0x0a,
      28,    1,  125,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, 0x80000000 | 9,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 12, 0x80000000 | 14,   11,   13,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   23,   24,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UChar,   26,   24,   27,
    QMetaType::Void, 0x80000000 | 9,   29,

       0        // eod
};

void AppCentral::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppCentral *_t = static_cast<AppCentral *>(_o);
        switch (_id) {
        case 0: _t->audioCtrlMsgEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 1: _t->editModeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->inputAssignModeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->inputAssigned((*reinterpret_cast< FxItem*(*)>(_a[1]))); break;
        case 4: _t->executeFxCmd((*reinterpret_cast< FxItem*(*)>(_a[1])),(*reinterpret_cast< CtrlCmd(*)>(_a[2])),(*reinterpret_cast< Executer*(*)>(_a[3]))); break;
        case 5: _t->executeNextFx((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->moveToFollowerFx((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->moveToForeRunnerFx((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setEditMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setExperimentalAudio((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->testSetDmxChannel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->onInputUniverseChannelChanged((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< uchar(*)>(_a[3]))); break;
        case 12: _t->storeSelectedFxListWidgetFx((*reinterpret_cast< FxItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AppCentral::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AppCentral::audioCtrlMsgEmitted)) {
                *result = 0;
            }
        }
        {
            typedef void (AppCentral::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AppCentral::editModeChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (AppCentral::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AppCentral::inputAssignModeChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (AppCentral::*_t)(FxItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AppCentral::inputAssigned)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject AppCentral::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AppCentral.data,
      qt_meta_data_AppCentral,  qt_static_metacall, 0, 0}
};


const QMetaObject *AppCentral::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppCentral::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppCentral.stringdata))
        return static_cast<void*>(const_cast< AppCentral*>(this));
    return QObject::qt_metacast(_clname);
}

int AppCentral::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void AppCentral::audioCtrlMsgEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AppCentral::editModeChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AppCentral::inputAssignModeChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AppCentral::inputAssigned(FxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
