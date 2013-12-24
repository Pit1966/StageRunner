/********************************************************************************
** Form generated from reading UI file 'messagedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEDIALOG_H
#define UI_MESSAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
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
            MessageDialog->setObjectName(QStringLiteral("MessageDialog"));
        MessageDialog->resize(574, 328);
        QIcon icon;
        icon.addFile(QStringLiteral(":/gfx/icons/ledyellow.png"), QSize(), QIcon::Normal, QIcon::Off);
        MessageDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(MessageDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        msgGroup = new QGroupBox(MessageDialog);
        msgGroup->setObjectName(QStringLiteral("msgGroup"));
        verticalLayout_2 = new QVBoxLayout(msgGroup);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        textEdit = new QTextEdit(msgGroup);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout_2->addWidget(textEdit);

        specialFunctionButton = new QPushButton(msgGroup);
        specialFunctionButton->setObjectName(QStringLiteral("specialFunctionButton"));

        verticalLayout_2->addWidget(specialFunctionButton);


        verticalLayout->addWidget(msgGroup);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        doNotShowAgainCheck = new QCheckBox(MessageDialog);
        doNotShowAgainCheck->setObjectName(QStringLiteral("doNotShowAgainCheck"));

        horizontalLayout->addWidget(doNotShowAgainCheck);

        cancelButton = new QPushButton(MessageDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        noButton = new QPushButton(MessageDialog);
        noButton->setObjectName(QStringLiteral("noButton"));

        horizontalLayout->addWidget(noButton);

        yesButton = new QPushButton(MessageDialog);
        yesButton->setObjectName(QStringLiteral("yesButton"));

        horizontalLayout->addWidget(yesButton);

        continueButton = new QPushButton(MessageDialog);
        continueButton->setObjectName(QStringLiteral("continueButton"));

        horizontalLayout->addWidget(continueButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MessageDialog);

        QMetaObject::connectSlotsByName(MessageDialog);
    } // setupUi

    void retranslateUi(QDialog *MessageDialog)
    {
        MessageDialog->setWindowTitle(QApplication::translate("MessageDialog", "System Message", 0));
        msgGroup->setTitle(QString());
        specialFunctionButton->setText(QApplication::translate("MessageDialog", "Special Function", 0));
        doNotShowAgainCheck->setText(QApplication::translate("MessageDialog", "Do not show again", 0));
        cancelButton->setText(QApplication::translate("MessageDialog", "Cancel", 0));
        noButton->setText(QApplication::translate("MessageDialog", "No", 0));
        yesButton->setText(QApplication::translate("MessageDialog", "Yes", 0));
        continueButton->setText(QApplication::translate("MessageDialog", "Continue", 0));
    } // retranslateUi

};

namespace Ui {
    class MessageDialog: public Ui_MessageDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEDIALOG_H
