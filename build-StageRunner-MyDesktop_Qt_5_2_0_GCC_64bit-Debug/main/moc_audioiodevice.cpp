/****************************************************************************
** Meta object code from reading C++ file 'audioiodevice.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/system/audioiodevice.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audioiodevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AudioIODevice_t {
    QByteArrayData data[18];
    char stringdata[218];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AudioIODevice_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AudioIODevice_t qt_meta_stringdata_AudioIODevice = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 9),
QT_MOC_LITERAL(2, 24, 0),
QT_MOC_LITERAL(3, 25, 14),
QT_MOC_LITERAL(4, 40, 4),
QT_MOC_LITERAL(5, 45, 5),
QT_MOC_LITERAL(6, 51, 21),
QT_MOC_LITERAL(7, 73, 2),
QT_MOC_LITERAL(8, 76, 5),
QT_MOC_LITERAL(9, 82, 5),
QT_MOC_LITERAL(10, 88, 4),
QT_MOC_LITERAL(11, 93, 22),
QT_MOC_LITERAL(12, 116, 20),
QT_MOC_LITERAL(13, 137, 25),
QT_MOC_LITERAL(14, 163, 8),
QT_MOC_LITERAL(15, 172, 17),
QT_MOC_LITERAL(16, 190, 20),
QT_MOC_LITERAL(17, 211, 5)
    },
    "AudioIODevice\0readReady\0\0vuLevelChanged\0"
    "left\0right\0audioDurationDetected\0ms\0"
    "start\0loops\0stop\0process_decoder_buffer\0"
    "on_decoding_finished\0if_audio_duration_changed\0"
    "duration\0if_error_occurred\0"
    "QAudioDecoder::Error\0error\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioIODevice[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06,
       3,    2,   65,    2, 0x06,
       6,    1,   70,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       8,    1,   73,    2, 0x0a,
       8,    0,   76,    2, 0x2a,
      10,    0,   77,    2, 0x0a,
      11,    0,   78,    2, 0x08,
      12,    0,   79,    2, 0x08,
      13,    1,   80,    2, 0x08,
      15,    1,   83,    2, 0x08,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    4,    5,
    QMetaType::Void, QMetaType::LongLong,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,   14,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void AudioIODevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioIODevice *_t = static_cast<AudioIODevice *>(_o);
        switch (_id) {
        case 0: _t->readReady(); break;
        case 1: _t->vuLevelChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->audioDurationDetected((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 3: _t->start((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->start(); break;
        case 5: _t->stop(); break;
        case 6: _t->process_decoder_buffer(); break;
        case 7: _t->on_decoding_finished(); break;
        case 8: _t->if_audio_duration_changed((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 9: _t->if_error_occurred((*reinterpret_cast< QAudioDecoder::Error(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAudioDecoder::Error >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioIODevice::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioIODevice::readReady)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioIODevice::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioIODevice::vuLevelChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioIODevice::*_t)(qint64 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioIODevice::audioDurationDetected)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject AudioIODevice::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_AudioIODevice.data,
      qt_meta_data_AudioIODevice,  qt_static_metacall, 0, 0}
};


const QMetaObject *AudioIODevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioIODevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioIODevice.stringdata))
        return static_cast<void*>(const_cast< AudioIODevice*>(this));
    return QIODevice::qt_metacast(_clname);
}

int AudioIODevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AudioIODevice::readReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void AudioIODevice::vuLevelChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AudioIODevice::audioDurationDetected(qint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
