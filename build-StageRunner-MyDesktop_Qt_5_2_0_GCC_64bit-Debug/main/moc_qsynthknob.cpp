/****************************************************************************
** Meta object code from reading C++ file 'qsynthknob.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/thirdparty/widget/qsynthknob.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsynthknob.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_qsynthKnob_t {
    QByteArrayData data[12];
    char stringdata[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_qsynthKnob_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_qsynthKnob_t qt_meta_stringdata_qsynthKnob = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 13),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 15),
QT_MOC_LITERAL(4, 42, 13),
QT_MOC_LITERAL(5, 56, 11),
QT_MOC_LITERAL(6, 68, 8),
QT_MOC_LITERAL(7, 77, 8),
QT_MOC_LITERAL(8, 86, 12),
QT_MOC_LITERAL(9, 99, 11),
QT_MOC_LITERAL(10, 111, 11),
QT_MOC_LITERAL(11, 123, 10)
    },
    "qsynthKnob\0doubleClicked\0\0setDefaultValue\0"
    "iDefaultValue\0setDialMode\0DialMode\0"
    "dialMode\0defaultValue\0DefaultMode\0"
    "AngularMode\0LinearMode\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qsynthKnob[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   36, // properties
       1,   42, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x0a,
       5,    1,   33,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // properties: name, type, flags
       8, QMetaType::Int, 0x00095103,
       7, 0x80000000 | 6, 0x0009510b,

 // enums: name, flags, count, data
       6, 0x0,    3,   46,

 // enum data: key, value
       9, uint(qsynthKnob::DefaultMode),
      10, uint(qsynthKnob::AngularMode),
      11, uint(qsynthKnob::LinearMode),

       0        // eod
};

void qsynthKnob::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        qsynthKnob *_t = static_cast<qsynthKnob *>(_o);
        switch (_id) {
        case 0: _t->doubleClicked(); break;
        case 1: _t->setDefaultValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setDialMode((*reinterpret_cast< DialMode(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (qsynthKnob::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&qsynthKnob::doubleClicked)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject qsynthKnob::staticMetaObject = {
    { &QDial::staticMetaObject, qt_meta_stringdata_qsynthKnob.data,
      qt_meta_data_qsynthKnob,  qt_static_metacall, 0, 0}
};


const QMetaObject *qsynthKnob::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qsynthKnob::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qsynthKnob.stringdata))
        return static_cast<void*>(const_cast< qsynthKnob*>(this));
    return QDial::qt_metacast(_clname);
}

int qsynthKnob::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDial::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = getDefaultValue(); break;
        case 1: *reinterpret_cast< DialMode*>(_v) = getDialMode(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setDefaultValue(*reinterpret_cast< int*>(_v)); break;
        case 1: setDialMode(*reinterpret_cast< DialMode*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void qsynthKnob::doubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
