/****************************************************************************
** Meta object code from reading C++ file 'audiocontrolwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/audiocontrolwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiocontrolwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AudioControlWidget_t {
    QByteArrayData data[16];
    char stringdata[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AudioControlWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AudioControlWidget_t qt_meta_stringdata_AudioControlWidget = {
    {
QT_MOC_LITERAL(0, 0, 18),
QT_MOC_LITERAL(1, 19, 11),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 7),
QT_MOC_LITERAL(4, 40, 11),
QT_MOC_LITERAL(5, 52, 13),
QT_MOC_LITERAL(6, 66, 6),
QT_MOC_LITERAL(7, 73, 19),
QT_MOC_LITERAL(8, 93, 12),
QT_MOC_LITERAL(9, 106, 3),
QT_MOC_LITERAL(10, 110, 17),
QT_MOC_LITERAL(11, 128, 17),
QT_MOC_LITERAL(12, 146, 15),
QT_MOC_LITERAL(13, 162, 7),
QT_MOC_LITERAL(14, 170, 4),
QT_MOC_LITERAL(15, 175, 5)
    },
    "AudioControlWidget\0playClicked\0\0slotNum\0"
    "stopClicked\0volumeChanged\0volume\0"
    "audioCtrlCmdEmitted\0AudioCtrlMsg\0msg\0"
    "audioCtrlReceiver\0audioCtrlRepeater\0"
    "setVuMeterLevel\0channel\0left\0right\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioControlWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06,
       4,    1,   52,    2, 0x06,
       5,    2,   55,    2, 0x06,
       7,    1,   60,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    1,   63,    2, 0x0a,
      11,    1,   66,    2, 0x0a,
      12,    3,   69,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   13,   14,   15,

       0        // eod
};

void AudioControlWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioControlWidget *_t = static_cast<AudioControlWidget *>(_o);
        switch (_id) {
        case 0: _t->playClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->stopClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->volumeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->audioCtrlCmdEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 4: _t->audioCtrlReceiver((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 5: _t->audioCtrlRepeater((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 6: _t->setVuMeterLevel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioControlWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControlWidget::playClicked)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioControlWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControlWidget::stopClicked)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioControlWidget::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControlWidget::volumeChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (AudioControlWidget::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioControlWidget::audioCtrlCmdEmitted)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject AudioControlWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_AudioControlWidget.data,
      qt_meta_data_AudioControlWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *AudioControlWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioControlWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioControlWidget.stringdata))
        return static_cast<void*>(const_cast< AudioControlWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int AudioControlWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void AudioControlWidget::playClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AudioControlWidget::stopClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AudioControlWidget::volumeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AudioControlWidget::audioCtrlCmdEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
