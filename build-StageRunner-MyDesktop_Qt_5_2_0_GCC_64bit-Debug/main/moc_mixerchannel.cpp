/****************************************************************************
** Meta object code from reading C++ file 'mixerchannel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/customwidget/mixerchannel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mixerchannel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MixerChannel_t {
    QByteArrayData data[20];
    char stringdata[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MixerChannel_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MixerChannel_t qt_meta_stringdata_MixerChannel = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 16),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 3),
QT_MOC_LITERAL(4, 35, 2),
QT_MOC_LITERAL(5, 38, 7),
QT_MOC_LITERAL(6, 46, 12),
QT_MOC_LITERAL(7, 59, 13),
QT_MOC_LITERAL(8, 73, 5),
QT_MOC_LITERAL(9, 79, 23),
QT_MOC_LITERAL(10, 103, 8),
QT_MOC_LITERAL(11, 112, 10),
QT_MOC_LITERAL(12, 123, 8),
QT_MOC_LITERAL(13, 132, 22),
QT_MOC_LITERAL(14, 155, 6),
QT_MOC_LITERAL(15, 162, 12),
QT_MOC_LITERAL(16, 175, 4),
QT_MOC_LITERAL(17, 180, 12),
QT_MOC_LITERAL(18, 193, 10),
QT_MOC_LITERAL(19, 204, 8)
    },
    "MixerChannel\0mixerSliderMoved\0\0val\0"
    "id\0clicked\0rightClicked\0mixerSelected\0"
    "state\0notifyChangedDmxChannel\0universe\0"
    "dmxchannel\0dmxvalue\0setRefSliderColorIndex\0"
    "colidx\0setLabelText\0text\0channelShown\0"
    "selectable\0selected\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MixerChannel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       3,   74, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06,
       5,    0,   54,    2, 0x06,
       6,    0,   55,    2, 0x06,
       7,    2,   56,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       9,    3,   61,    2, 0x0a,
      13,    1,   68,    2, 0x0a,
      15,    1,   71,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    8,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   10,   11,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::QString,   16,

 // properties: name, type, flags
      17, QMetaType::Bool, 0x00095103,
      18, QMetaType::Bool, 0x00095103,
      19, QMetaType::Bool, 0x00095103,

       0        // eod
};

void MixerChannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MixerChannel *_t = static_cast<MixerChannel *>(_o);
        switch (_id) {
        case 0: _t->mixerSliderMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->clicked(); break;
        case 2: _t->rightClicked(); break;
        case 3: _t->mixerSelected((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->notifyChangedDmxChannel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->setRefSliderColorIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->setLabelText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MixerChannel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerChannel::mixerSliderMoved)) {
                *result = 0;
            }
        }
        {
            typedef void (MixerChannel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerChannel::clicked)) {
                *result = 1;
            }
        }
        {
            typedef void (MixerChannel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerChannel::rightClicked)) {
                *result = 2;
            }
        }
        {
            typedef void (MixerChannel::*_t)(bool , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MixerChannel::mixerSelected)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject MixerChannel::staticMetaObject = {
    { &QAbstractSlider::staticMetaObject, qt_meta_stringdata_MixerChannel.data,
      qt_meta_data_MixerChannel,  qt_static_metacall, 0, 0}
};


const QMetaObject *MixerChannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MixerChannel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MixerChannel.stringdata))
        return static_cast<void*>(const_cast< MixerChannel*>(this));
    return QAbstractSlider::qt_metacast(_clname);
}

int MixerChannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSlider::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< bool*>(_v) = channelShown(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isSelectable(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isSelected(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setChannelShown(*reinterpret_cast< bool*>(_v)); break;
        case 1: setSelectable(*reinterpret_cast< bool*>(_v)); break;
        case 2: setSelected(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MixerChannel::mixerSliderMoved(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MixerChannel::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MixerChannel::rightClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void MixerChannel::mixerSelected(bool _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
