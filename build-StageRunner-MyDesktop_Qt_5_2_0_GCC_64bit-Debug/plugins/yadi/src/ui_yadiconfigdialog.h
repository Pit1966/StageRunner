/********************************************************************************
** Form generated from reading UI file 'yadiconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YADICONFIGDIALOG_H
#define UI_YADICONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_YadiConfigDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *transceiverGroup;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label;
    QComboBox *transMergeModeCombo;
    QLabel *label_4;
    QSpinBox *transMaxOutChannelsSpin;
    QSpinBox *transMaxInChannelsSpin;
    QTextEdit *textEdit;
    QLabel *selectDeviceLabel;
    QPushButton *closeButton;
    QGroupBox *receiverGroup;
    QGridLayout *gridLayout_3;
    QSpinBox *rxMaxInChannelsSpin;
    QLabel *label_2;
    QPushButton *showDmxInButton;
    QComboBox *selectDeviceCombo;
    QPushButton *showDmxOutButton;
    QSpinBox *debugSpin;
    QPushButton *rescanDevButton;
    QLabel *debugLabel;

    void setupUi(QDialog *YadiConfigDialog)
    {
        if (YadiConfigDialog->objectName().isEmpty())
            YadiConfigDialog->setObjectName(QStringLiteral("YadiConfigDialog"));
        YadiConfigDialog->resize(678, 403);
        gridLayout = new QGridLayout(YadiConfigDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        transceiverGroup = new QGroupBox(YadiConfigDialog);
        transceiverGroup->setObjectName(QStringLiteral("transceiverGroup"));
        gridLayout_2 = new QGridLayout(transceiverGroup);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_3 = new QLabel(transceiverGroup);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        label = new QLabel(transceiverGroup);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        transMergeModeCombo = new QComboBox(transceiverGroup);
        transMergeModeCombo->setObjectName(QStringLiteral("transMergeModeCombo"));

        gridLayout_2->addWidget(transMergeModeCombo, 0, 1, 1, 1);

        label_4 = new QLabel(transceiverGroup);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        transMaxOutChannelsSpin = new QSpinBox(transceiverGroup);
        transMaxOutChannelsSpin->setObjectName(QStringLiteral("transMaxOutChannelsSpin"));
        transMaxOutChannelsSpin->setMinimum(4);
        transMaxOutChannelsSpin->setMaximum(512);
        transMaxOutChannelsSpin->setValue(512);

        gridLayout_2->addWidget(transMaxOutChannelsSpin, 1, 1, 1, 1);

        transMaxInChannelsSpin = new QSpinBox(transceiverGroup);
        transMaxInChannelsSpin->setObjectName(QStringLiteral("transMaxInChannelsSpin"));
        transMaxInChannelsSpin->setMinimum(4);
        transMaxInChannelsSpin->setMaximum(512);
        transMaxInChannelsSpin->setValue(512);

        gridLayout_2->addWidget(transMaxInChannelsSpin, 2, 1, 1, 1);


        gridLayout->addWidget(transceiverGroup, 5, 0, 1, 2);

        textEdit = new QTextEdit(YadiConfigDialog);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        gridLayout->addWidget(textEdit, 1, 0, 1, 3);

        selectDeviceLabel = new QLabel(YadiConfigDialog);
        selectDeviceLabel->setObjectName(QStringLiteral("selectDeviceLabel"));

        gridLayout->addWidget(selectDeviceLabel, 2, 0, 1, 1);

        closeButton = new QPushButton(YadiConfigDialog);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        gridLayout->addWidget(closeButton, 6, 2, 1, 1);

        receiverGroup = new QGroupBox(YadiConfigDialog);
        receiverGroup->setObjectName(QStringLiteral("receiverGroup"));
        gridLayout_3 = new QGridLayout(receiverGroup);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        rxMaxInChannelsSpin = new QSpinBox(receiverGroup);
        rxMaxInChannelsSpin->setObjectName(QStringLiteral("rxMaxInChannelsSpin"));
        rxMaxInChannelsSpin->setMinimum(4);
        rxMaxInChannelsSpin->setMaximum(512);
        rxMaxInChannelsSpin->setValue(128);

        gridLayout_3->addWidget(rxMaxInChannelsSpin, 1, 1, 1, 1);

        label_2 = new QLabel(receiverGroup);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 0, 1, 1, 1);


        gridLayout->addWidget(receiverGroup, 5, 2, 1, 1);

        showDmxInButton = new QPushButton(YadiConfigDialog);
        showDmxInButton->setObjectName(QStringLiteral("showDmxInButton"));

        gridLayout->addWidget(showDmxInButton, 3, 1, 1, 1);

        selectDeviceCombo = new QComboBox(YadiConfigDialog);
        selectDeviceCombo->setObjectName(QStringLiteral("selectDeviceCombo"));

        gridLayout->addWidget(selectDeviceCombo, 3, 0, 1, 1);

        showDmxOutButton = new QPushButton(YadiConfigDialog);
        showDmxOutButton->setObjectName(QStringLiteral("showDmxOutButton"));

        gridLayout->addWidget(showDmxOutButton, 4, 1, 1, 1);

        debugSpin = new QSpinBox(YadiConfigDialog);
        debugSpin->setObjectName(QStringLiteral("debugSpin"));

        gridLayout->addWidget(debugSpin, 4, 2, 1, 1);

        rescanDevButton = new QPushButton(YadiConfigDialog);
        rescanDevButton->setObjectName(QStringLiteral("rescanDevButton"));

        gridLayout->addWidget(rescanDevButton, 4, 0, 1, 1);

        debugLabel = new QLabel(YadiConfigDialog);
        debugLabel->setObjectName(QStringLiteral("debugLabel"));

        gridLayout->addWidget(debugLabel, 3, 2, 1, 1);


        retranslateUi(YadiConfigDialog);
        QObject::connect(closeButton, SIGNAL(clicked()), YadiConfigDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(YadiConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *YadiConfigDialog)
    {
        YadiConfigDialog->setWindowTitle(QApplication::translate("YadiConfigDialog", "YADI DMX Configuration", 0));
        transceiverGroup->setTitle(QApplication::translate("YadiConfigDialog", "Transceiver", 0));
        label_3->setText(QApplication::translate("YadiConfigDialog", "Max DMX Output Channels", 0));
        label->setText(QApplication::translate("YadiConfigDialog", "DMX In/Out Merge Mode", 0));
        transMergeModeCombo->clear();
        transMergeModeCombo->insertItems(0, QStringList()
         << QApplication::translate("YadiConfigDialog", "HTP (Highest Value)", 0)
         << QApplication::translate("YadiConfigDialog", "DMX in (Fallback DMX out)", 0)
         << QApplication::translate("YadiConfigDialog", "DMX out (Fallback DMX in)", 0)
         << QApplication::translate("YadiConfigDialog", "force DMX in", 0)
         << QApplication::translate("YadiConfigDialog", "force DMX out", 0)
        );
        label_4->setText(QApplication::translate("YadiConfigDialog", "Max DMX Input Channels", 0));
        selectDeviceLabel->setText(QApplication::translate("YadiConfigDialog", "Select device to configure", 0));
        closeButton->setText(QApplication::translate("YadiConfigDialog", "Close", 0));
        receiverGroup->setTitle(QApplication::translate("YadiConfigDialog", "Receiver", 0));
        label_2->setText(QApplication::translate("YadiConfigDialog", "Max DMX Input Channels", 0));
        showDmxInButton->setText(QApplication::translate("YadiConfigDialog", "Show DMX Input Monitor", 0));
        showDmxOutButton->setText(QApplication::translate("YadiConfigDialog", "Show DMX Output Monitor", 0));
        rescanDevButton->setText(QApplication::translate("YadiConfigDialog", "Rescan Devices", 0));
        debugLabel->setText(QApplication::translate("YadiConfigDialog", "Debug", 0));
    } // retranslateUi

};

namespace Ui {
    class YadiConfigDialog: public Ui_YadiConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YADICONFIGDIALOG_H
