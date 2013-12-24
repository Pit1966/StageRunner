/****************************************************************************
** Meta object code from reading C++ file 'setupwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StageRunner/gui/setupwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'setupwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SetupWidget_t {
    QByteArrayData data[18];
    char stringdata[381];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SetupWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SetupWidget_t qt_meta_stringdata_SetupWidget = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 23),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 5),
QT_MOC_LITERAL(4, 43, 20),
QT_MOC_LITERAL(5, 64, 19),
QT_MOC_LITERAL(6, 84, 23),
QT_MOC_LITERAL(7, 108, 31),
QT_MOC_LITERAL(8, 140, 16),
QT_MOC_LITERAL(9, 157, 4),
QT_MOC_LITERAL(10, 162, 32),
QT_MOC_LITERAL(11, 195, 30),
QT_MOC_LITERAL(12, 226, 30),
QT_MOC_LITERAL(13, 257, 3),
QT_MOC_LITERAL(14, 261, 34),
QT_MOC_LITERAL(15, 296, 36),
QT_MOC_LITERAL(16, 333, 4),
QT_MOC_LITERAL(17, 338, 41)
    },
    "SetupWidget\0applicationStyleChanged\0"
    "\0style\0dialKnobStyleChanged\0"
    "on_okButton_clicked\0on_cancelButton_clicked\0"
    "on_qlcPluginsList_itemActivated\0"
    "QListWidgetItem*\0item\0"
    "on_configurePluginButton_clicked\0"
    "on_saveDmxConfigButton_clicked\0"
    "if_pluginline_universe_changed\0val\0"
    "if_pluginline_responsetime_changed\0"
    "on_appStyleCombo_currentIndexChanged\0"
    "arg1\0on_dialKnobStyleCombo_currentIndexChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SetupWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06,
       4,    1,   72,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   75,    2, 0x08,
       6,    0,   76,    2, 0x08,
       7,    1,   77,    2, 0x08,
      10,    0,   80,    2, 0x08,
      11,    0,   81,    2, 0x08,
      12,    1,   82,    2, 0x08,
      14,    1,   85,    2, 0x08,
      15,    1,   88,    2, 0x08,
      17,    1,   91,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,

       0        // eod
};

void SetupWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SetupWidget *_t = static_cast<SetupWidget *>(_o);
        switch (_id) {
        case 0: _t->applicationStyleChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->dialKnobStyleChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->on_okButton_clicked(); break;
        case 3: _t->on_cancelButton_clicked(); break;
        case 4: _t->on_qlcPluginsList_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->on_configurePluginButton_clicked(); break;
        case 6: _t->on_saveDmxConfigButton_clicked(); break;
        case 7: _t->if_pluginline_universe_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->if_pluginline_responsetime_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_appStyleCombo_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_dialKnobStyleCombo_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SetupWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SetupWidget::applicationStyleChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (SetupWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SetupWidget::dialKnobStyleChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject SetupWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SetupWidget.data,
      qt_meta_data_SetupWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *SetupWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SetupWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetupWidget.stringdata))
        return static_cast<void*>(const_cast< SetupWidget*>(this));
    if (!strcmp(_clname, "Ui::SetupWidget"))
        return static_cast< Ui::SetupWidget*>(const_cast< SetupWidget*>(this));
    return QDialog::qt_metacast(_clname);
}

int SetupWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void SetupWidget::applicationStyleChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SetupWidget::dialKnobStyleChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
