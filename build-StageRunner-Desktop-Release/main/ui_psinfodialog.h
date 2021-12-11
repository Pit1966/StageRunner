/********************************************************************************
** Form generated from reading UI file 'psinfodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PSINFODIALOG_H
#define UI_PSINFODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PsInfoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QPlainTextEdit *infoEdit;
    QPushButton *closeButton;

    void setupUi(QDialog *PsInfoDialog)
    {
        if (PsInfoDialog->objectName().isEmpty())
            PsInfoDialog->setObjectName(QString::fromUtf8("PsInfoDialog"));
        PsInfoDialog->resize(888, 396);
        verticalLayout = new QVBoxLayout(PsInfoDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(PsInfoDialog);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        verticalLayout->addWidget(titleLabel);

        infoEdit = new QPlainTextEdit(PsInfoDialog);
        infoEdit->setObjectName(QString::fromUtf8("infoEdit"));
        infoEdit->setUndoRedoEnabled(false);
        infoEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        infoEdit->setReadOnly(true);

        verticalLayout->addWidget(infoEdit);

        closeButton = new QPushButton(PsInfoDialog);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        verticalLayout->addWidget(closeButton);


        retranslateUi(PsInfoDialog);

        QMetaObject::connectSlotsByName(PsInfoDialog);
    } // setupUi

    void retranslateUi(QDialog *PsInfoDialog)
    {
        PsInfoDialog->setWindowTitle(QCoreApplication::translate("PsInfoDialog", "Dialog", nullptr));
        titleLabel->setText(QCoreApplication::translate("PsInfoDialog", "Logging information", nullptr));
        closeButton->setText(QCoreApplication::translate("PsInfoDialog", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PsInfoDialog: public Ui_PsInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PSINFODIALOG_H
