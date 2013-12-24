/****************************************************************************
** Meta object code from reading C++ file 'fxitempropertywidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/fxitempropertywidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fxitempropertywidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxItemPropertyWidget_t {
    QByteArrayData data[22];
    char stringdata[383];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FxItemPropertyWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FxItemPropertyWidget_t qt_meta_stringdata_FxItemPropertyWidget = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 8),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 9),
QT_MOC_LITERAL(4, 41, 7),
QT_MOC_LITERAL(5, 49, 2),
QT_MOC_LITERAL(6, 52, 11),
QT_MOC_LITERAL(7, 64, 5),
QT_MOC_LITERAL(8, 70, 4),
QT_MOC_LITERAL(9, 75, 29),
QT_MOC_LITERAL(10, 105, 8),
QT_MOC_LITERAL(11, 114, 22),
QT_MOC_LITERAL(12, 137, 4),
QT_MOC_LITERAL(13, 142, 21),
QT_MOC_LITERAL(14, 164, 28),
QT_MOC_LITERAL(15, 193, 34),
QT_MOC_LITERAL(16, 228, 28),
QT_MOC_LITERAL(17, 257, 29),
QT_MOC_LITERAL(18, 287, 25),
QT_MOC_LITERAL(19, 313, 25),
QT_MOC_LITERAL(20, 339, 11),
QT_MOC_LITERAL(21, 351, 30)
    },
    "FxItemPropertyWidget\0modified\0\0setFxItem\0"
    "FxItem*\0fx\0setEditable\0state\0once\0"
    "on_initialVolDial_sliderMoved\0position\0"
    "on_nameEdit_textEdited\0arg1\0"
    "on_keyEdit_textEdited\0"
    "on_faderCountEdit_textEdited\0"
    "on_audioFilePathEdit_doubleClicked\0"
    "on_fadeInTimeEdit_textEdited\0"
    "on_fadeOutTimeEdit_textEdited\0"
    "on_keyClearButton_clicked\0"
    "on_editOnceButton_clicked\0finish_edit\0"
    "on_audioLoopsSpin_valueChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxItemPropertyWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       3,    1,   90,    2, 0x0a,
       6,    2,   93,    2, 0x0a,
       6,    1,   98,    2, 0x2a,
       9,    1,  101,    2, 0x08,
      11,    1,  104,    2, 0x08,
      13,    1,  107,    2, 0x08,
      14,    1,  110,    2, 0x08,
      15,    0,  113,    2, 0x08,
      16,    1,  114,    2, 0x08,
      17,    1,  117,    2, 0x08,
      18,    0,  120,    2, 0x08,
      19,    0,  121,    2, 0x08,
      20,    0,  122,    2, 0x08,
      21,    1,  123,    2, 0x08,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Bool, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    7,    8,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,

       0        // eod
};

void FxItemPropertyWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxItemPropertyWidget *_t = static_cast<FxItemPropertyWidget *>(_o);
        switch (_id) {
        case 0: _t->modified(); break;
        case 1: { bool _r = _t->setFxItem((*reinterpret_cast< FxItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: _t->setEditable((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->setEditable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_initialVolDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_nameEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->on_keyEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->on_faderCountEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->on_audioFilePathEdit_doubleClicked(); break;
        case 9: _t->on_fadeInTimeEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_fadeOutTimeEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->on_keyClearButton_clicked(); break;
        case 12: _t->on_editOnceButton_clicked(); break;
        case 13: _t->finish_edit(); break;
        case 14: _t->on_audioLoopsSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FxItemPropertyWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxItemPropertyWidget::modified)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject FxItemPropertyWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FxItemPropertyWidget.data,
      qt_meta_data_FxItemPropertyWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *FxItemPropertyWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxItemPropertyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FxItemPropertyWidget.stringdata))
        return static_cast<void*>(const_cast< FxItemPropertyWidget*>(this));
    if (!strcmp(_clname, "Ui::FxItemPropertyWidget"))
        return static_cast< Ui::FxItemPropertyWidget*>(const_cast< FxItemPropertyWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int FxItemPropertyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void FxItemPropertyWidget::modified()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
