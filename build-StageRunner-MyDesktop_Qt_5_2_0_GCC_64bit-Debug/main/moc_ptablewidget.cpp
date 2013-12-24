/****************************************************************************
** Meta object code from reading C++ file 'ptablewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/ptablewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ptablewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PTableWidget_t {
    QByteArrayData data[16];
    char stringdata[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PTableWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PTableWidget_t qt_meta_stringdata_PTableWidget = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 17),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 4),
QT_MOC_LITERAL(4, 37, 3),
QT_MOC_LITERAL(5, 41, 14),
QT_MOC_LITERAL(6, 56, 6),
QT_MOC_LITERAL(7, 63, 7),
QT_MOC_LITERAL(8, 71, 13),
QT_MOC_LITERAL(9, 85, 13),
QT_MOC_LITERAL(10, 99, 12),
QT_MOC_LITERAL(11, 112, 6),
QT_MOC_LITERAL(12, 119, 7),
QT_MOC_LITERAL(13, 127, 9),
QT_MOC_LITERAL(14, 137, 20),
QT_MOC_LITERAL(15, 158, 3)
    },
    "PTableWidget\0dropEventReceived\0\0text\0"
    "row\0rowMovedFromTo\0srcrow\0destrow\0"
    "rowClonedFrom\0PTableWidget*\0originPTable\0"
    "srcRow\0destRow\0removeSrc\0if_scrolled_vertical\0"
    "val\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PTableWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06,
       5,    2,   39,    2, 0x06,
       8,    4,   44,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      14,    1,   53,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    7,
    QMetaType::Void, 0x80000000 | 9, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   10,   11,   12,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   15,

       0        // eod
};

void PTableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PTableWidget *_t = static_cast<PTableWidget *>(_o);
        switch (_id) {
        case 0: _t->dropEventReceived((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->rowMovedFromTo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->rowClonedFrom((*reinterpret_cast< PTableWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 3: _t->if_scrolled_vertical((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PTableWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PTableWidget::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PTableWidget::dropEventReceived)) {
                *result = 0;
            }
        }
        {
            typedef void (PTableWidget::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PTableWidget::rowMovedFromTo)) {
                *result = 1;
            }
        }
        {
            typedef void (PTableWidget::*_t)(PTableWidget * , int , int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PTableWidget::rowClonedFrom)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject PTableWidget::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_PTableWidget.data,
      qt_meta_data_PTableWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *PTableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PTableWidget.stringdata))
        return static_cast<void*>(const_cast< PTableWidget*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int PTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PTableWidget::dropEventReceived(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PTableWidget::rowMovedFromTo(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PTableWidget::rowClonedFrom(PTableWidget * _t1, int _t2, int _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
