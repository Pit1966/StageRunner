/****************************************************************************
** Meta object code from reading C++ file 'sequencecontrolwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/sequencecontrolwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sequencecontrolwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SequenceControlWidget_t {
    QByteArrayData data[13];
    char stringdata[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SequenceControlWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SequenceControlWidget_t qt_meta_stringdata_SequenceControlWidget = {
    {
QT_MOC_LITERAL(0, 0, 21),
QT_MOC_LITERAL(1, 22, 14),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 7),
QT_MOC_LITERAL(4, 46, 7),
QT_MOC_LITERAL(5, 54, 9),
QT_MOC_LITERAL(6, 64, 2),
QT_MOC_LITERAL(7, 67, 25),
QT_MOC_LITERAL(8, 93, 25),
QT_MOC_LITERAL(9, 119, 28),
QT_MOC_LITERAL(10, 148, 8),
QT_MOC_LITERAL(11, 157, 27),
QT_MOC_LITERAL(12, 185, 27)
    },
    "SequenceControlWidget\0fxCmdActivated\0"
    "\0FxItem*\0CtrlCmd\0setNextFx\0fx\0"
    "on_ctrlPlayButton_clicked\0"
    "on_ctrlStopButton_clicked\0"
    "on_masterVolDial_sliderMoved\0position\0"
    "on_moveToNextButton_clicked\0"
    "on_moveToPrevButton_clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SequenceControlWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    1,   54,    2, 0x0a,
       7,    0,   57,    2, 0x08,
       8,    0,   58,    2, 0x08,
       9,    1,   59,    2, 0x08,
      11,    0,   62,    2, 0x08,
      12,    0,   63,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SequenceControlWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SequenceControlWidget *_t = static_cast<SequenceControlWidget *>(_o);
        switch (_id) {
        case 0: _t->fxCmdActivated((*reinterpret_cast< FxItem*(*)>(_a[1])),(*reinterpret_cast< CtrlCmd(*)>(_a[2]))); break;
        case 1: _t->setNextFx((*reinterpret_cast< FxItem*(*)>(_a[1]))); break;
        case 2: _t->on_ctrlPlayButton_clicked(); break;
        case 3: _t->on_ctrlStopButton_clicked(); break;
        case 4: _t->on_masterVolDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_moveToNextButton_clicked(); break;
        case 6: _t->on_moveToPrevButton_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SequenceControlWidget::*_t)(FxItem * , CtrlCmd );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SequenceControlWidget::fxCmdActivated)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject SequenceControlWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_SequenceControlWidget.data,
      qt_meta_data_SequenceControlWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *SequenceControlWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SequenceControlWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SequenceControlWidget.stringdata))
        return static_cast<void*>(const_cast< SequenceControlWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int SequenceControlWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SequenceControlWidget::fxCmdActivated(FxItem * _t1, CtrlCmd _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
