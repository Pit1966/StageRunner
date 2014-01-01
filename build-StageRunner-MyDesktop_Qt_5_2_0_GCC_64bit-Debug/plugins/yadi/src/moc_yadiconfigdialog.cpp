/****************************************************************************
** Meta object code from reading C++ file 'yadiconfigdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../StageRunner/plugins/yadi/src/yadiconfigdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'yadiconfigdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_YadiConfigDialog_t {
    QByteArrayData data[14];
    char stringdata[352];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_YadiConfigDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_YadiConfigDialog_t qt_meta_stringdata_YadiConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 26),
QT_MOC_LITERAL(2, 44, 0),
QT_MOC_LITERAL(3, 45, 24),
QT_MOC_LITERAL(4, 70, 40),
QT_MOC_LITERAL(5, 111, 5),
QT_MOC_LITERAL(6, 117, 32),
QT_MOC_LITERAL(7, 150, 39),
QT_MOC_LITERAL(8, 190, 4),
QT_MOC_LITERAL(9, 195, 38),
QT_MOC_LITERAL(10, 234, 35),
QT_MOC_LITERAL(11, 270, 26),
QT_MOC_LITERAL(12, 297, 27),
QT_MOC_LITERAL(13, 325, 25)
    },
    "YadiConfigDialog\0on_rescanDevButton_clicked\0"
    "\0show_current_device_list\0"
    "on_selectDeviceCombo_currentIndexChanged\0"
    "index\0on_transMergeModeCombo_activated\0"
    "on_transMaxOutChannelsSpin_valueChanged\0"
    "arg1\0on_transMaxInChannelsSpin_valueChanged\0"
    "on_rxMaxInChannelsSpin_valueChanged\0"
    "on_showDmxInButton_clicked\0"
    "on_showDmxOutButton_clicked\0"
    "on_debugSpin_valueChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_YadiConfigDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08,
       3,    0,   65,    2, 0x08,
       4,    1,   66,    2, 0x08,
       6,    1,   69,    2, 0x08,
       7,    1,   72,    2, 0x08,
       9,    1,   75,    2, 0x08,
      10,    1,   78,    2, 0x08,
      11,    0,   81,    2, 0x08,
      12,    0,   82,    2, 0x08,
      13,    1,   83,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

void YadiConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        YadiConfigDialog *_t = static_cast<YadiConfigDialog *>(_o);
        switch (_id) {
        case 0: _t->on_rescanDevButton_clicked(); break;
        case 1: _t->show_current_device_list(); break;
        case 2: _t->on_selectDeviceCombo_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_transMergeModeCombo_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_transMaxOutChannelsSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_transMaxInChannelsSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_rxMaxInChannelsSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_showDmxInButton_clicked(); break;
        case 8: _t->on_showDmxOutButton_clicked(); break;
        case 9: _t->on_debugSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject YadiConfigDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_YadiConfigDialog.data,
      qt_meta_data_YadiConfigDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *YadiConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *YadiConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_YadiConfigDialog.stringdata))
        return static_cast<void*>(const_cast< YadiConfigDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int YadiConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
