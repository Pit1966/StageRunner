/********************************************************************************
** Form generated from reading UI file 'universeeditorwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UNIVERSEEDITORWIDGET_H
#define UI_UNIVERSEEDITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gui/deviceinfowidget.h"

QT_BEGIN_NAMESPACE

class Ui_UniverseEditorWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *middleLayout;
    QVBoxLayout *middleLeftLayout;
    QHBoxLayout *chooseFixtureLayout;
    QPushButton *selectDeviceButton;
    DeviceInfoWidget *deviceInfoWidget;
    QTableWidget *universeTable;
    QHBoxLayout *horizontalLayout;
    QPushButton *addDeviceButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *removeDeviceButton;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_saveLayout;
    QPushButton *pushButton_loadLayout;
    QPushButton *pushButton_createTemplate;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_close;

    void setupUi(QWidget *UniverseEditorWidget)
    {
        if (UniverseEditorWidget->objectName().isEmpty())
            UniverseEditorWidget->setObjectName(QString::fromUtf8("UniverseEditorWidget"));
        UniverseEditorWidget->resize(1355, 768);
        verticalLayout = new QVBoxLayout(UniverseEditorWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        middleLayout = new QHBoxLayout();
        middleLayout->setObjectName(QString::fromUtf8("middleLayout"));
        middleLayout->setContentsMargins(-1, 0, -1, -1);
        middleLeftLayout = new QVBoxLayout();
        middleLeftLayout->setObjectName(QString::fromUtf8("middleLeftLayout"));
        middleLeftLayout->setContentsMargins(-1, -1, 0, -1);
        chooseFixtureLayout = new QHBoxLayout();
        chooseFixtureLayout->setObjectName(QString::fromUtf8("chooseFixtureLayout"));
        selectDeviceButton = new QPushButton(UniverseEditorWidget);
        selectDeviceButton->setObjectName(QString::fromUtf8("selectDeviceButton"));

        chooseFixtureLayout->addWidget(selectDeviceButton);


        middleLeftLayout->addLayout(chooseFixtureLayout);

        deviceInfoWidget = new DeviceInfoWidget(UniverseEditorWidget);
        deviceInfoWidget->setObjectName(QString::fromUtf8("deviceInfoWidget"));
        deviceInfoWidget->setMinimumSize(QSize(100, 0));

        middleLeftLayout->addWidget(deviceInfoWidget);


        middleLayout->addLayout(middleLeftLayout);

        universeTable = new QTableWidget(UniverseEditorWidget);
        if (universeTable->columnCount() < 4)
            universeTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        universeTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        universeTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        universeTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        universeTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        universeTable->setObjectName(QString::fromUtf8("universeTable"));
        universeTable->horizontalHeader()->setCascadingSectionResizes(true);
        universeTable->verticalHeader()->setVisible(false);

        middleLayout->addWidget(universeTable);


        verticalLayout->addLayout(middleLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        addDeviceButton = new QPushButton(UniverseEditorWidget);
        addDeviceButton->setObjectName(QString::fromUtf8("addDeviceButton"));

        horizontalLayout->addWidget(addDeviceButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        removeDeviceButton = new QPushButton(UniverseEditorWidget);
        removeDeviceButton->setObjectName(QString::fromUtf8("removeDeviceButton"));

        horizontalLayout->addWidget(removeDeviceButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_saveLayout = new QPushButton(UniverseEditorWidget);
        pushButton_saveLayout->setObjectName(QString::fromUtf8("pushButton_saveLayout"));

        horizontalLayout_2->addWidget(pushButton_saveLayout);

        pushButton_loadLayout = new QPushButton(UniverseEditorWidget);
        pushButton_loadLayout->setObjectName(QString::fromUtf8("pushButton_loadLayout"));

        horizontalLayout_2->addWidget(pushButton_loadLayout);

        pushButton_createTemplate = new QPushButton(UniverseEditorWidget);
        pushButton_createTemplate->setObjectName(QString::fromUtf8("pushButton_createTemplate"));

        horizontalLayout_2->addWidget(pushButton_createTemplate);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pushButton_close = new QPushButton(UniverseEditorWidget);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));

        horizontalLayout_2->addWidget(pushButton_close);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(UniverseEditorWidget);

        QMetaObject::connectSlotsByName(UniverseEditorWidget);
    } // setupUi

    void retranslateUi(QWidget *UniverseEditorWidget)
    {
        UniverseEditorWidget->setWindowTitle(QCoreApplication::translate("UniverseEditorWidget", "Form", nullptr));
        selectDeviceButton->setText(QCoreApplication::translate("UniverseEditorWidget", "Select Device", nullptr));
        QTableWidgetItem *___qtablewidgetitem = universeTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("UniverseEditorWidget", "Device Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = universeTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("UniverseEditorWidget", "DMX", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = universeTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("UniverseEditorWidget", "Channel Mode", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = universeTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("UniverseEditorWidget", "Description", nullptr));
        addDeviceButton->setText(QCoreApplication::translate("UniverseEditorWidget", "Add Device", nullptr));
        removeDeviceButton->setText(QCoreApplication::translate("UniverseEditorWidget", "Remove Device", nullptr));
        pushButton_saveLayout->setText(QCoreApplication::translate("UniverseEditorWidget", "Save Layout", nullptr));
        pushButton_loadLayout->setText(QCoreApplication::translate("UniverseEditorWidget", "Load Layout", nullptr));
        pushButton_createTemplate->setText(QCoreApplication::translate("UniverseEditorWidget", "Create Template Scene", nullptr));
        pushButton_close->setText(QCoreApplication::translate("UniverseEditorWidget", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UniverseEditorWidget: public Ui_UniverseEditorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UNIVERSEEDITORWIDGET_H
