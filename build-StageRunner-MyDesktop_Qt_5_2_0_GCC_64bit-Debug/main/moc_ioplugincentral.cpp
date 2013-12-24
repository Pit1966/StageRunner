/****************************************************************************
** Meta object code from reading C++ file 'ioplugincentral.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/appcontrol/ioplugincentral.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ioplugincentral.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_IOPluginCentral_t {
    QByteArrayData data[10];
    char stringdata[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_IOPluginCentral_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_IOPluginCentral_t qt_meta_stringdata_IOPluginCentral = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 20),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 8),
QT_MOC_LITERAL(4, 47, 7),
QT_MOC_LITERAL(5, 55, 5),
QT_MOC_LITERAL(6, 61, 19),
QT_MOC_LITERAL(7, 81, 5),
QT_MOC_LITERAL(8, 87, 28),
QT_MOC_LITERAL(9, 116, 13)
    },
    "IOPluginCentral\0universeValueChanged\0"
    "\0universe\0channel\0value\0onInputValueChanged\0"
    "input\0onPluginConfigurationChanged\0"
    "reOpenPlugins\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IOPluginCentral[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       6,    3,   41,    2, 0x08,
       8,    0,   48,    2, 0x08,
       9,    0,   49,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UChar,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UChar,    7,    4,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void IOPluginCentral::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IOPluginCentral *_t = static_cast<IOPluginCentral *>(_o);
        switch (_id) {
        case 0: _t->universeValueChanged((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< uchar(*)>(_a[3]))); break;
        case 1: _t->onInputValueChanged((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< uchar(*)>(_a[3]))); break;
        case 2: _t->onPluginConfigurationChanged(); break;
        case 3: _t->reOpenPlugins(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (IOPluginCentral::*_t)(quint32 , quint32 , uchar );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IOPluginCentral::universeValueChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject IOPluginCentral::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_IOPluginCentral.data,
      qt_meta_data_IOPluginCentral,  qt_static_metacall, 0, 0}
};


const QMetaObject *IOPluginCentral::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IOPluginCentral::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IOPluginCentral.stringdata))
        return static_cast<void*>(const_cast< IOPluginCentral*>(this));
    return QObject::qt_metacast(_clname);
}

int IOPluginCentral::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void IOPluginCentral::universeValueChanged(quint32 _t1, quint32 _t2, uchar _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
