/********************************************************************************
** Form generated from reading UI file 'deviceinfowidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICEINFOWIDGET_H
#define UI_DEVICEINFOWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeviceInfoWidget
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *deviceLabel;
    QLabel *deviceNameLabel;
    QLabel *modeLabel;
    QComboBox *modeCombo;
    QGroupBox *channelsGroup;
    QVBoxLayout *verticalLayout_2;
    QListWidget *channelsList;

    void setupUi(QWidget *DeviceInfoWidget)
    {
        if (DeviceInfoWidget->objectName().isEmpty())
            DeviceInfoWidget->setObjectName(QString::fromUtf8("DeviceInfoWidget"));
        DeviceInfoWidget->resize(422, 392);
        verticalLayout = new QVBoxLayout(DeviceInfoWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        deviceLabel = new QLabel(DeviceInfoWidget);
        deviceLabel->setObjectName(QString::fromUtf8("deviceLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        deviceLabel->setFont(font);

        formLayout->setWidget(0, QFormLayout::LabelRole, deviceLabel);

        deviceNameLabel = new QLabel(DeviceInfoWidget);
        deviceNameLabel->setObjectName(QString::fromUtf8("deviceNameLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, deviceNameLabel);

        modeLabel = new QLabel(DeviceInfoWidget);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));
        modeLabel->setFont(font);

        formLayout->setWidget(1, QFormLayout::LabelRole, modeLabel);

        modeCombo = new QComboBox(DeviceInfoWidget);
        modeCombo->setObjectName(QString::fromUtf8("modeCombo"));

        formLayout->setWidget(1, QFormLayout::FieldRole, modeCombo);


        verticalLayout->addLayout(formLayout);

        channelsGroup = new QGroupBox(DeviceInfoWidget);
        channelsGroup->setObjectName(QString::fromUtf8("channelsGroup"));
        verticalLayout_2 = new QVBoxLayout(channelsGroup);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        channelsList = new QListWidget(channelsGroup);
        channelsList->setObjectName(QString::fromUtf8("channelsList"));

        verticalLayout_2->addWidget(channelsList);


        verticalLayout->addWidget(channelsGroup);


        retranslateUi(DeviceInfoWidget);

        QMetaObject::connectSlotsByName(DeviceInfoWidget);
    } // setupUi

    void retranslateUi(QWidget *DeviceInfoWidget)
    {
        DeviceInfoWidget->setWindowTitle(QCoreApplication::translate("DeviceInfoWidget", "Form", nullptr));
        deviceLabel->setText(QCoreApplication::translate("DeviceInfoWidget", "DEVICE", nullptr));
        deviceNameLabel->setText(QCoreApplication::translate("DeviceInfoWidget", "Device", nullptr));
        modeLabel->setText(QCoreApplication::translate("DeviceInfoWidget", "Mode", nullptr));
        channelsGroup->setTitle(QCoreApplication::translate("DeviceInfoWidget", "Channels", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeviceInfoWidget: public Ui_DeviceInfoWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICEINFOWIDGET_H
