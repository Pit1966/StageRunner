/****************************************************************************
** Meta object code from reading C++ file 'audioslotwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/audioslotwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audioslotwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AudioSlotWidget_t {
    QByteArrayData data[21];
    char stringdata[285];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AudioSlotWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AudioSlotWidget_t qt_meta_stringdata_AudioSlotWidget = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 11),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 7),
QT_MOC_LITERAL(4, 37, 11),
QT_MOC_LITERAL(5, 49, 13),
QT_MOC_LITERAL(6, 63, 3),
QT_MOC_LITERAL(7, 67, 19),
QT_MOC_LITERAL(8, 87, 12),
QT_MOC_LITERAL(9, 100, 3),
QT_MOC_LITERAL(10, 104, 25),
QT_MOC_LITERAL(11, 130, 25),
QT_MOC_LITERAL(12, 156, 29),
QT_MOC_LITERAL(13, 186, 8),
QT_MOC_LITERAL(14, 195, 31),
QT_MOC_LITERAL(15, 227, 12),
QT_MOC_LITERAL(16, 240, 5),
QT_MOC_LITERAL(17, 246, 15),
QT_MOC_LITERAL(18, 262, 10),
QT_MOC_LITERAL(19, 273, 4),
QT_MOC_LITERAL(20, 278, 5)
    },
    "AudioSlotWidget\0playClicked\0\0slotNum\0"
    "stopClicked\0volumeChanged\0vol\0"
    "audioCtrlCmdEmitted\0AudioCtrlMsg\0msg\0"
    "on_slotPlayButton_clicked\0"
    "on_slotStopButton_clicked\0"
    "on_slotVolumeDial_sliderMoved\0position\0"
    "on_slotVolumeDial_doubleClicked\0"
    "setPlayState\0state\0updateGuiStatus\0"
    "setVuLevel\0left\0right\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioSlotWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06,
       4,    1,   72,    2, 0x06,
       5,    2,   75,    2, 0x06,
       7,    1,   80,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      10,    0,   83,    2, 0x08,
      11,    0,   84,    2, 0x08,
      12,    1,   85,    2, 0x08,
      14,    0,   88,    2, 0x08,
      15,    1,   89,    2, 0x0a,
      17,    1,   92,    2, 0x0a,
      18,    2,   95,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   20,

       0        // eod
};

void AudioSlotWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioSlotWidget *_t = static_cast<AudioSlotWidget *>(_o);
        switch (_id) {
        case 0: _t->playClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->stopClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->volumeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->audioCtrlCmdEmitted((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 4: _t->on_slotPlayButton_clicked(); break;
        case 5: _t->on_slotStopButton_clicked(); break;
        case 6: _t->on_slotVolumeDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_slotVolumeDial_doubleClicked(); break;
        case 8: _t->setPlayState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->updateGuiStatus((*reinterpret_cast< AudioCtrlMsg(*)>(_a[1]))); break;
        case 10: _t->setVuLevel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioSlotWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlotWidget::playClicked)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioSlotWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlotWidget::stopClicked)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioSlotWidget::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlotWidget::volumeChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (AudioSlotWidget::*_t)(AudioCtrlMsg );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioSlotWidget::audioCtrlCmdEmitted)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject AudioSlotWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_AudioSlotWidget.data,
      qt_meta_data_AudioSlotWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *AudioSlotWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioSlotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioSlotWidget.stringdata))
        return static_cast<void*>(const_cast< AudioSlotWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int AudioSlotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void AudioSlotWidget::playClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AudioSlotWidget::stopClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AudioSlotWidget::volumeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AudioSlotWidget::audioCtrlCmdEmitted(AudioCtrlMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
