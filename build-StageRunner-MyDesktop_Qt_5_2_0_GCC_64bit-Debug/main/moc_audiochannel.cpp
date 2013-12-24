/****************************************************************************
** Meta object code from reading C++ file 'audiochannel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/system/audiochannel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiochannel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AudioSlot_t {
    QByteArrayData data[25];
    char stringdata[345];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AudioSlot_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AudioSlot_t qt_meta_stringdata_AudioSlot = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 19),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 12),
QT_MOC_LITERAL(4, 44, 3),
QT_MOC_LITERAL(5, 48, 20),
QT_MOC_LITERAL(6, 69, 7),
QT_MOC_LITERAL(7, 77, 12),
QT_MOC_LITERAL(8, 90, 7),
QT_MOC_LITERAL(9, 98, 8),
QT_MOC_LITERAL(10, 107, 14),
QT_MOC_LITERAL(11, 122, 4),
QT_MOC_LITERAL(12, 127, 5),
QT_MOC_LITERAL(13, 133, 30),
QT_MOC_LITERAL(14, 164, 13),
QT_MOC_LITERAL(15, 178, 5),
QT_MOC_LITERAL(16, 184, 22),
QT_MOC_LITERAL(17, 207, 18),
QT_MOC_LITERAL(18, 226, 25),
QT_MOC_LITERAL(19, 252, 5),
QT_MOC_LITERAL(20, 258, 20),
QT_MOC_LITERAL(21, 279, 24),
QT_MOC_LITERAL(22, 304, 17),
QT_MOC_LITERAL(23, 322, 18),
QT_MOC_LITERAL(24, 341, 2)
    },
    "AudioSlot\0audioCtrlMsgEmitted\0\0"
    "AudioCtrlMsg\0msg\0audioProgressChanged\0"
    "slotnum\0FxAudioItem*\0fxaudio\0perMille\0"
    "vuLevelChanged\0left\0right\0"
    "on_audio_output_status_changed\0"
    "QAudio::State\0state\0on_audio_io_read_ready\0"
    "on_vulevel_changed\0on_fade_out_frame_changed\0"
    "value\0on_fade_out_finished\0"
    "on_volset_timer_finished\0audioCtrlReceiver\0"
    "setAudioDurationMs\0ms\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioSlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06,
       5,    3,   72,    2, 0x06,
      10,    3,   79,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      13,    1,   86,    2, 0x08,
      16,    0,   89,    2, 0x08,
      17,    2,   90,    2, 0x08,
      18,    1,   95,    2, 0x08,
      20,    0,   98,    2, 0x08,
      21,    0,   99,    2, 0x08,
      22,    1,  100,    2, 0x0a,
      23,    1,  103,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7, QMetaType::Int,    6,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    6,   11,   12,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   11,   12,
    QMetaType::Void, QMetaType::QReal,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::LongLong,   24,

       0        // eod
};

void AudioSlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioSlot *_t = static_cast<AudioSlot *>(_o);
        switch (_id) {
        case 0: _t->audioCtrlMsgEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 1: _t->audioProgressChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< FxAudioItem*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->vuLevelChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->on_audio_output_status_changed((*reinterpret_cast< QAudio::State(*)>(_a[1]))); break;
        case 4: _t->on_audio_io_read_ready(); break;
        case 5: _t->on_vulevel_changed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->on_fade_out_frame_changed((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 7: _t->on_fade_out_finished(); break;
        case 8: _t->on_volset_timer_finished(); break;
        case 9: _t->audioCtrlReceiver((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 10: _t->setAudioDurationMs((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAudio::State >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioSlot::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlot::audioCtrlMsgEmitted)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioSlot::*_t)(int , FxAudioItem * , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlot::audioProgressChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioSlot::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlot::vuLevelChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject AudioSlot::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AudioSlot.data,
      qt_meta_data_AudioSlot,  qt_static_metacall, 0, 0}
};


const QMetaObject *AudioSlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioSlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioSlot.stringdata))
        return static_cast<void*>(const_cast< AudioSlot*>(this));
    return QObject::qt_metacast(_clname);
}

int AudioSlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void AudioSlot::audioCtrlMsgEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AudioSlot::audioProgressChanged(int _t1, FxAudioItem * _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AudioSlot::vuLevelChanged(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
