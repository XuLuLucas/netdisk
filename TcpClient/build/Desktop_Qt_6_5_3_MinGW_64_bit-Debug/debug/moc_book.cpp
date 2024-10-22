/****************************************************************************
** Meta object code from reading C++ file 'book.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../book.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'book.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSbookENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSbookENDCLASS = QtMocHelpers::stringData(
    "book",
    "CreateDir",
    "",
    "delItem",
    "flushList",
    "renameDir",
    "enterDir",
    "QListWidgetItem*",
    "exitDir",
    "upLoadFile",
    "downLoadFile"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSbookENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[5];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[10];
    char stringdata5[10];
    char stringdata6[9];
    char stringdata7[17];
    char stringdata8[8];
    char stringdata9[11];
    char stringdata10[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSbookENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSbookENDCLASS_t qt_meta_stringdata_CLASSbookENDCLASS = {
    {
        QT_MOC_LITERAL(0, 4),  // "book"
        QT_MOC_LITERAL(5, 9),  // "CreateDir"
        QT_MOC_LITERAL(15, 0),  // ""
        QT_MOC_LITERAL(16, 7),  // "delItem"
        QT_MOC_LITERAL(24, 9),  // "flushList"
        QT_MOC_LITERAL(34, 9),  // "renameDir"
        QT_MOC_LITERAL(44, 8),  // "enterDir"
        QT_MOC_LITERAL(53, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(70, 7),  // "exitDir"
        QT_MOC_LITERAL(78, 10),  // "upLoadFile"
        QT_MOC_LITERAL(89, 12)   // "downLoadFile"
    },
    "book",
    "CreateDir",
    "",
    "delItem",
    "flushList",
    "renameDir",
    "enterDir",
    "QListWidgetItem*",
    "exitDir",
    "upLoadFile",
    "downLoadFile"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSbookENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x0a,    1 /* Public */,
       3,    0,   63,    2, 0x0a,    2 /* Public */,
       4,    0,   64,    2, 0x0a,    3 /* Public */,
       5,    0,   65,    2, 0x0a,    4 /* Public */,
       6,    1,   66,    2, 0x0a,    5 /* Public */,
       8,    0,   69,    2, 0x0a,    7 /* Public */,
       9,    0,   70,    2, 0x0a,    8 /* Public */,
      10,    0,   71,    2, 0x0a,    9 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject book::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSbookENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSbookENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSbookENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<book, std::true_type>,
        // method 'CreateDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'delItem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'flushList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'renameDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enterDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'exitDir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'upLoadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'downLoadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void book::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<book *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->CreateDir(); break;
        case 1: _t->delItem(); break;
        case 2: _t->flushList(); break;
        case 3: _t->renameDir(); break;
        case 4: _t->enterDir((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 5: _t->exitDir(); break;
        case 6: _t->upLoadFile(); break;
        case 7: _t->downLoadFile(); break;
        default: ;
        }
    }
}

const QMetaObject *book::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *book::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSbookENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int book::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
