/****************************************************************************
** Meta object code from reading C++ file 'sequencestatuswidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/sequencestatuswidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sequencestatuswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SequenceStatusWidget_t {
    QByteArrayData data[7];
    char stringdata[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SequenceStatusWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SequenceStatusWidget_t qt_meta_stringdata_SequenceStatusWidget = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 17),
QT_MOC_LITERAL(2, 39, 0),
QT_MOC_LITERAL(3, 40, 9),
QT_MOC_LITERAL(4, 50, 4),
QT_MOC_LITERAL(5, 55, 14),
QT_MOC_LITERAL(6, 70, 15)
    },
    "SequenceStatusWidget\0propagateExecuter\0"
    "\0Executer*\0exec\0insertExecuter\0"
    "scratchExecuter\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SequenceStatusWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a,
       5,    1,   32,    2, 0x0a,
       6,    1,   35,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void SequenceStatusWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SequenceStatusWidget *_t = static_cast<SequenceStatusWidget *>(_o);
        switch (_id) {
        case 0: _t->propagateExecuter((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 1: _t->insertExecuter((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        case 2: _t->scratchExecuter((*reinterpret_cast< Executer*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SequenceStatusWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SequenceStatusWidget.data,
      qt_meta_data_SequenceStatusWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *SequenceStatusWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SequenceStatusWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SequenceStatusWidget.stringdata))
        return static_cast<void*>(const_cast< SequenceStatusWidget*>(this));
    if (!strcmp(_clname, "Ui::SequenceStatusWidget"))
        return static_cast< Ui::SequenceStatusWidget*>(const_cast< SequenceStatusWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SequenceStatusWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
    return _id;
}
QT_END_MOC_NAMESPACE
