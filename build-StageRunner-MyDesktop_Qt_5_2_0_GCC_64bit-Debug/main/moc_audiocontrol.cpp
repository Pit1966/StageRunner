/****************************************************************************
** Meta object code from reading C++ file 'audiocontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/system/audiocontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiocontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AudioControl_t {
    QByteArrayData data[30];
    char stringdata[375];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AudioControl_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AudioControl_t qt_meta_stringdata_AudioControl = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 19),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 12),
QT_MOC_LITERAL(4, 47, 3),
QT_MOC_LITERAL(5, 51, 25),
QT_MOC_LITERAL(6, 77, 14),
QT_MOC_LITERAL(7, 92, 7),
QT_MOC_LITERAL(8, 100, 4),
QT_MOC_LITERAL(9, 105, 5),
QT_MOC_LITERAL(10, 111, 25),
QT_MOC_LITERAL(11, 137, 12),
QT_MOC_LITERAL(12, 150, 12),
QT_MOC_LITERAL(13, 163, 3),
QT_MOC_LITERAL(14, 167, 9),
QT_MOC_LITERAL(15, 177, 4),
QT_MOC_LITERAL(16, 182, 20),
QT_MOC_LITERAL(17, 203, 14),
QT_MOC_LITERAL(18, 218, 11),
QT_MOC_LITERAL(19, 230, 4),
QT_MOC_LITERAL(20, 235, 17),
QT_MOC_LITERAL(21, 253, 7),
QT_MOC_LITERAL(22, 261, 14),
QT_MOC_LITERAL(23, 276, 17),
QT_MOC_LITERAL(24, 294, 17),
QT_MOC_LITERAL(25, 312, 15),
QT_MOC_LITERAL(26, 328, 3),
QT_MOC_LITERAL(27, 332, 9),
QT_MOC_LITERAL(28, 342, 9),
QT_MOC_LITERAL(29, 352, 21)
    },
    "AudioControl\0audioCtrlMsgEmitted\0\0"
    "AudioCtrlMsg\0msg\0audioThreadCtrlMsgEmitted\0"
    "vuLevelChanged\0slotnum\0left\0right\0"
    "vu_level_changed_receiver\0startFxAudio\0"
    "FxAudioItem*\0fxa\0Executer*\0exec\0"
    "restartFxAudioInSlot\0stopAllFxAudio\0"
    "stopFxAudio\0slot\0fadeoutAllFxAudio\0"
    "time_ms\0fadeoutFxAudio\0audioCtrlRepeater\0"
    "audioCtrlReceiver\0setMasterVolume\0vol\0"
    "setVolume\0getVolume\0setVolumeFromDmxLevel\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioControl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  124,    2, 0x06,
       5,    1,  127,    2, 0x06,
       6,    3,  130,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    3,  137,    2, 0x08,
      11,    2,  144,    2, 0x0a,
      16,    1,  149,    2, 0x0a,
      17,    0,  152,    2, 0x0a,
      18,    1,  153,    2, 0x0a,
      18,    1,  156,    2, 0x0a,
      20,    1,  159,    2, 0x0a,
      20,    0,  162,    2, 0x2a,
      22,    2,  163,    2, 0x0a,
      22,    2,  168,    2, 0x0a,
      22,    1,  173,    2, 0x2a,
      22,    2,  176,    2, 0x0a,
      22,    1,  181,    2, 0x2a,
      23,    1,  184,    2, 0x0a,
      24,    1,  187,    2, 0x0a,
      25,    1,  190,    2, 0x0a,
      27,    2,  193,    2, 0x0a,
      28,    1,  198,    2, 0x0a,
      29,    2,  201,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    7,    8,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    7,    8,    9,
    QMetaType::Bool, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Bool, QMetaType::Int,    7,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Bool, QMetaType::Int,   21,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   21,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,   21,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Int,   15,   21,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   26,
    QMetaType::Int, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   26,

       0        // eod
};

void AudioControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioControl *_t = static_cast<AudioControl *>(_o);
        switch (_id) {
        case 0: _t->audioCtrlMsgEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 1: _t->audioThreadCtrlMsgEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 2: _t->vuLevelChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->vu_level_changed_receiver((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: { bool _r = _t->startFxAudio((*reinterpret_cast< FxAudioItem*(*)>(_a[1])),(*reinterpret_cast< Executer*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = _t->restartFxAudioInSlot((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = _t->stopAllFxAudio();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->stopFxAudio((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->stopFxAudio((*reinterpret_cast< FxAudioItem*(*)>(_a[1]))); break;
        case 9: { bool _r = _t->fadeoutAllFxAudio((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = _t->fadeoutAllFxAudio();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: _t->fadeoutFxAudio((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->fadeoutFxAudio((*reinterpret_cast< FxAudioItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->fadeoutFxAudio((*reinterpret_cast< FxAudioItem*(*)>(_a[1]))); break;
        case 14: _t->fadeoutFxAudio((*reinterpret_cast< Executer*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->fadeoutFxAudio((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 16: _t->audioCtrlRepeater((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 17: _t->audioCtrlReceiver((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 18: _t->setMasterVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->setVolume((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 20: { int _r = _t->getVolume((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 21: _t->setVolumeFromDmxLevel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioControl::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControl::audioCtrlMsgEmitted)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioControl::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControl::audioThreadCtrlMsgEmitted)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioControl::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControl::vuLevelChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject AudioControl::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_AudioControl.data,
      qt_meta_data_AudioControl,  qt_static_metacall, 0, 0}
};


const QMetaObject *AudioControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioControl.stringdata))
        return static_cast<void*>(const_cast< AudioControl*>(this));
    return QThread::qt_metacast(_clname);
}

int AudioControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void AudioControl::audioCtrlMsgEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AudioControl::audioThreadCtrlMsgEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AudioControl::vuLevelChanged(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
