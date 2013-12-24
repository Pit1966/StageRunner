/****************************************************************************
** Meta object code from reading C++ file 'mixergroup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/customwidget/mixergroup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mixergroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MixerGroup_t {
    QByteArrayData data[18];
    char stringdata[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MixerGroup_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MixerGroup_t qt_meta_stringdata_MixerGroup = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 16),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 3),
QT_MOC_LITERAL(4, 33, 2),
QT_MOC_LITERAL(5, 36, 13),
QT_MOC_LITERAL(6, 50, 5),
QT_MOC_LITERAL(7, 56, 11),
QT_MOC_LITERAL(8, 68, 9),
QT_MOC_LITERAL(9, 78, 7),
QT_MOC_LITERAL(10, 86, 24),
QT_MOC_LITERAL(11, 111, 8),
QT_MOC_LITERAL(12, 120, 9),
QT_MOC_LITERAL(13, 130, 22),
QT_MOC_LITERAL(14, 153, 6),
QT_MOC_LITERAL(15, 160, 14),
QT_MOC_LITERAL(16, 175, 17),
QT_MOC_LITERAL(17, 193, 18)
    },
    "MixerGroup\0mixerSliderMoved\0\0val\0id\0"
    "mixerSelected\0state\0mixerDraged\0"
    "fromIndex\0toIndex\0notifyChangedDmxUniverse\0"
    "universe\0dmxValues\0setRefSliderColorIndex\0"
    "colidx\0on_mixer_moved\0on_mixer_selected\0"
    "multiSelectEnabled\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MixerGroup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       1,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06,
       5,    2,   54,    2, 0x06,
       7,    2,   59,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    2,   64,    2, 0x0a,
      13,    1,   69,    2, 0x0a,
      15,    2,   72,    2, 0x08,
      16,    2,   77,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    6,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QByteArray,   11,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    6,    4,

 // properties: name, type, flags
      17, QMetaType::Bool, 0x00095103,

       0        // eod
};

void MixerGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MixerGroup *_t = static_cast<MixerGroup *>(_o);
        switch (_id) {
        case 0: _t->mixerSliderMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->mixerSelected((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->mixerDraged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->notifyChangedDmxUniverse((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 4: _t->setRefSliderColorIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_mixer_moved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->on_mixer_selected((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MixerGroup::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerGroup::mixerSliderMoved)) {
                *result = 0;
            }
        }
        {
            typedef void (MixerGroup::*_t)(bool , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerGroup::mixerSelected)) {
                *result = 1;
            }
        }
        {
            typedef void (MixerGroup::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerGroup::mixerDraged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject MixerGroup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MixerGroup.data,
      qt_meta_data_MixerGroup,  qt_static_metacall, 0, 0}
};


const QMetaObject *MixerGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MixerGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MixerGroup.stringdata))
        return static_cast<void*>(const_cast< MixerGroup*>(this));
    return QWidget::qt_metacast(_clname);
}

int MixerGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isMultiSelectEnabled(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMultiSelectEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MixerGroup::mixerSliderMoved(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MixerGroup::mixerSelected(bool _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MixerGroup::mixerDraged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
