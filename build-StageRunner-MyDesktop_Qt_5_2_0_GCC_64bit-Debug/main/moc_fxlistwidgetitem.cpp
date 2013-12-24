/****************************************************************************
** Meta object code from reading C++ file 'fxlistwidgetitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/fxlistwidgetitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fxlistwidgetitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxListWidgetItem_t {
    QByteArrayData data[23];
    char stringdata[340];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FxListWidgetItem_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FxListWidgetItem_t qt_meta_stringdata_FxListWidgetItem = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 11),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 17),
QT_MOC_LITERAL(4, 48, 17),
QT_MOC_LITERAL(5, 66, 14),
QT_MOC_LITERAL(6, 81, 6),
QT_MOC_LITERAL(7, 88, 4),
QT_MOC_LITERAL(8, 93, 11),
QT_MOC_LITERAL(9, 105, 5),
QT_MOC_LITERAL(10, 111, 16),
QT_MOC_LITERAL(11, 128, 11),
QT_MOC_LITERAL(12, 140, 9),
QT_MOC_LITERAL(13, 150, 21),
QT_MOC_LITERAL(14, 172, 9),
QT_MOC_LITERAL(15, 182, 9),
QT_MOC_LITERAL(16, 192, 22),
QT_MOC_LITERAL(17, 215, 22),
QT_MOC_LITERAL(18, 238, 20),
QT_MOC_LITERAL(19, 259, 26),
QT_MOC_LITERAL(20, 286, 19),
QT_MOC_LITERAL(21, 306, 4),
QT_MOC_LITERAL(22, 311, 27)
    },
    "FxListWidgetItem\0itemClicked\0\0"
    "FxListWidgetItem*\0itemDoubleClicked\0"
    "itemTextEdited\0draged\0item\0setEditable\0"
    "state\0setNeverEditable\0setSelected\0"
    "setMarked\0setActivationProgress\0"
    "perMilleA\0perMilleB\0setActivationProgressB\0"
    "setActivationProgressA\0if_edit_item_clicked\0"
    "if_edit_item_doubleclicked\0"
    "if_edit_item_edited\0text\0"
    "if_label_item_doubleclicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxListWidgetItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06,
       4,    1,   92,    2, 0x06,
       5,    2,   95,    2, 0x06,
       6,    1,  100,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       8,    1,  103,    2, 0x0a,
      10,    1,  106,    2, 0x0a,
      11,    1,  109,    2, 0x0a,
      12,    1,  112,    2, 0x0a,
      13,    2,  115,    2, 0x0a,
      16,    1,  120,    2, 0x0a,
      17,    1,  123,    2, 0x0a,
      18,    0,  126,    2, 0x08,
      19,    0,  127,    2, 0x08,
      20,    1,  128,    2, 0x08,
      22,    0,  131,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    2,    2,
    QMetaType::Void, 0x80000000 | 3,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   14,   15,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void,

       0        // eod
};

void FxListWidgetItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxListWidgetItem *_t = static_cast<FxListWidgetItem *>(_o);
        switch (_id) {
        case 0: _t->itemClicked((*reinterpret_cast< FxListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->itemDoubleClicked((*reinterpret_cast< FxListWidgetItem*(*)>(_a[1]))); break;
        case 2: _t->itemTextEdited((*reinterpret_cast< FxListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->draged((*reinterpret_cast< FxListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->setEditable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->setNeverEditable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->setSelected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->setMarked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setActivationProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->setActivationProgressB((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setActivationProgressA((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->if_edit_item_clicked(); break;
        case 12: _t->if_edit_item_doubleclicked(); break;
        case 13: _t->if_edit_item_edited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->if_label_item_doubleclicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FxListWidgetItem* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FxListWidgetItem* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FxListWidgetItem* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FxListWidgetItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FxListWidgetItem::*_t)(FxListWidgetItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxListWidgetItem::itemClicked)) {
                *result = 0;
            }
        }
        {
            typedef void (FxListWidgetItem::*_t)(FxListWidgetItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxListWidgetItem::itemDoubleClicked)) {
                *result = 1;
            }
        }
        {
            typedef void (FxListWidgetItem::*_t)(FxListWidgetItem * , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxListWidgetItem::itemTextEdited)) {
                *result = 2;
            }
        }
        {
            typedef void (FxListWidgetItem::*_t)(FxListWidgetItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxListWidgetItem::draged)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject FxListWidgetItem::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FxListWidgetItem.data,
      qt_meta_data_FxListWidgetItem,  qt_static_metacall, 0, 0}
};


const QMetaObject *FxListWidgetItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxListWidgetItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FxListWidgetItem.stringdata))
        return static_cast<void*>(const_cast< FxListWidgetItem*>(this));
    if (!strcmp(_clname, "Ui::FxListWidgetItem"))
        return static_cast< Ui::FxListWidgetItem*>(const_cast< FxListWidgetItem*>(this));
    return QWidget::qt_metacast(_clname);
}

int FxListWidgetItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void FxListWidgetItem::itemClicked(FxListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FxListWidgetItem::itemDoubleClicked(FxListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FxListWidgetItem::itemTextEdited(FxListWidgetItem * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FxListWidgetItem::draged(FxListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
