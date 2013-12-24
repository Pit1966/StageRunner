/****************************************************************************
** Meta object code from reading C++ file 'fxitemobj.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/fx/fxitemobj.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fxitemobj.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxItemObj_t {
    QByteArrayData data[9];
    char stringdata[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FxItemObj_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FxItemObj_t qt_meta_stringdata_FxItemObj = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 14),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 7),
QT_MOC_LITERAL(4, 34, 2),
QT_MOC_LITERAL(5, 37, 7),
QT_MOC_LITERAL(6, 45, 3),
QT_MOC_LITERAL(7, 49, 9),
QT_MOC_LITERAL(8, 59, 4)
    },
    "FxItemObj\0playFxPlayList\0\0FxItem*\0fx\0"
    "CtrlCmd\0cmd\0Executer*\0exec\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxItemObj[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 7,    4,    6,    8,

       0        // eod
};

void FxItemObj::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxItemObj *_t = static_cast<FxItemObj *>(_o);
        switch (_id) {
        case 0: _t->playFxPlayList((*reinterpret_cast< FxItem*(*)>(_a[1])),(*reinterpret_cast< CtrlCmd(*)>(_a[2])),(*reinterpret_cast< Executer*(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject FxItemObj::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FxItemObj.data,
      qt_meta_data_FxItemObj,  qt_static_metacall, 0, 0}
};


const QMetaObject *FxItemObj::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxItemObj::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FxItemObj.stringdata))
        return static_cast<void*>(const_cast< FxItemObj*>(this));
    return QObject::qt_metacast(_clname);
}

int FxItemObj::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
