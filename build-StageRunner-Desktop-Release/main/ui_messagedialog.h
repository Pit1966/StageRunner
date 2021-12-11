/********************************************************************************
** Form generated from reading UI file 'messagedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEDIALOG_H
#define UI_MESSAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MessageDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *msgGroup;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEdit;
    QPushButton *specialFunctionButton;
    QHBoxLayout *horizontalLayout;
    QCheckBox *doNotShowAgainCheck;
    QPushButton *cancelButton;
    QPushButton *noButton;
    QPushButton *yesButton;
    QPushButton *continueButton;

    void setupUi(QDialog *MessageDialog)
    {
        if (MessageDialog->objectName().isEmpty())
            MessageDialog->setObjectName(QString::fromUtf8("MessageDialog"));
        MessageDialog->resize(574, 328);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/ledyellow.png"), QSize(), QIcon::Normal, QIcon::Off);
        MessageDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(MessageDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        msgGroup = new QGroupBox(MessageDialog);
        msgGroup->setObjectName(QString::fromUtf8("msgGroup"));
        verticalLayout_2 = new QVBoxLayout(msgGroup);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        textEdit = new QTextEdit(msgGroup);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout_2->addWidget(textEdit);

        specialFunctionButton = new QPushButton(msgGroup);
        specialFunctionButton->setObjectName(QString::fromUtf8("specialFunctionButton"));

        verticalLayout_2->addWidget(specialFunctionButton);


        verticalLayout->addWidget(msgGroup);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        doNotShowAgainCheck = new QCheckBox(MessageDialog);
        doNotShowAgainCheck->setObjectName(QString::fromUtf8("doNotShowAgainCheck"));

        horizontalLayout->addWidget(doNotShowAgainCheck);

        cancelButton = new QPushButton(MessageDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        noButton = new QPushButton(MessageDialog);
        noButton->setObjectName(QString::fromUtf8("noButton"));

        horizontalLayout->addWidget(noButton);

        yesButton = new QPushButton(MessageDialog);
        yesButton->setObjectName(QString::fromUtf8("yesButton"));

        horizontalLayout->addWidget(yesButton);

        continueButton = new QPushButton(MessageDialog);
        continueButton->setObjectName(QString::fromUtf8("continueButton"));

        horizontalLayout->addWidget(continueButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MessageDialog);

        QMetaObject::connectSlotsByName(MessageDialog);
    } // setupUi

    void retranslateUi(QDialog *MessageDialog)
    {
        MessageDialog->setWindowTitle(QCoreApplication::translate("MessageDialog", "System Message", nullptr));
        msgGroup->setTitle(QString());
        specialFunctionButton->setText(QCoreApplication::translate("MessageDialog", "Special Function", nullptr));
        doNotShowAgainCheck->setText(QCoreApplication::translate("MessageDialog", "Do not show again", nullptr));
        cancelButton->setText(QCoreApplication::translate("MessageDialog", "Cancel", nullptr));
        noButton->setText(QCoreApplication::translate("MessageDialog", "No", nullptr));
        yesButton->setText(QCoreApplication::translate("MessageDialog", "Yes", nullptr));
        continueButton->setText(QCoreApplication::translate("MessageDialog", "Continue", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MessageDialog: public Ui_MessageDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEDIALOG_H
