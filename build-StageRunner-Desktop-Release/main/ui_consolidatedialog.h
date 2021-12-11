/********************************************************************************
** Form generated from reading UI file 'consolidatedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLIDATEDIALOG_H
#define UI_CONSOLIDATEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConsolidateDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *infoLabel;
    QFrame *line_2;
    QGroupBox *exportParaGroup;
    QGridLayout *gridLayout;
    QLabel *targetProjectNameLab;
    QLineEdit *targetProjectNameEdit;
    QLabel *targetPathLabel;
    QLineEdit *targetPathEdit;
    QPushButton *targetDirButton;
    QCheckBox *keepFxIDsCheck;
    QProgressBar *progressBar;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *ConsolidateDialog)
    {
        if (ConsolidateDialog->objectName().isEmpty())
            ConsolidateDialog->setObjectName(QString::fromUtf8("ConsolidateDialog"));
        ConsolidateDialog->resize(705, 254);
        verticalLayout = new QVBoxLayout(ConsolidateDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        infoLabel = new QLabel(ConsolidateDialog);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));

        verticalLayout->addWidget(infoLabel);

        line_2 = new QFrame(ConsolidateDialog);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        exportParaGroup = new QGroupBox(ConsolidateDialog);
        exportParaGroup->setObjectName(QString::fromUtf8("exportParaGroup"));
        gridLayout = new QGridLayout(exportParaGroup);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        targetProjectNameLab = new QLabel(exportParaGroup);
        targetProjectNameLab->setObjectName(QString::fromUtf8("targetProjectNameLab"));

        gridLayout->addWidget(targetProjectNameLab, 0, 0, 1, 1);

        targetProjectNameEdit = new QLineEdit(exportParaGroup);
        targetProjectNameEdit->setObjectName(QString::fromUtf8("targetProjectNameEdit"));

        gridLayout->addWidget(targetProjectNameEdit, 0, 1, 1, 1);

        targetPathLabel = new QLabel(exportParaGroup);
        targetPathLabel->setObjectName(QString::fromUtf8("targetPathLabel"));

        gridLayout->addWidget(targetPathLabel, 1, 0, 1, 1);

        targetPathEdit = new QLineEdit(exportParaGroup);
        targetPathEdit->setObjectName(QString::fromUtf8("targetPathEdit"));

        gridLayout->addWidget(targetPathEdit, 1, 1, 1, 1);

        targetDirButton = new QPushButton(exportParaGroup);
        targetDirButton->setObjectName(QString::fromUtf8("targetDirButton"));

        gridLayout->addWidget(targetDirButton, 1, 2, 1, 1);

        keepFxIDsCheck = new QCheckBox(exportParaGroup);
        keepFxIDsCheck->setObjectName(QString::fromUtf8("keepFxIDsCheck"));
        keepFxIDsCheck->setChecked(true);

        gridLayout->addWidget(keepFxIDsCheck, 2, 1, 1, 1);


        verticalLayout->addWidget(exportParaGroup);

        progressBar = new QProgressBar(ConsolidateDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(ConsolidateDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cancelButton = new QPushButton(ConsolidateDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(ConsolidateDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        horizontalLayout->addWidget(okButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ConsolidateDialog);

        QMetaObject::connectSlotsByName(ConsolidateDialog);
    } // setupUi

    void retranslateUi(QDialog *ConsolidateDialog)
    {
        ConsolidateDialog->setWindowTitle(QCoreApplication::translate("ConsolidateDialog", "Dialog", nullptr));
        infoLabel->setText(QCoreApplication::translate("ConsolidateDialog", "<html><head/><body><p><span style=\" font-weight:600;\">Consolidate project:</span></p></body></html>", nullptr));
        exportParaGroup->setTitle(QCoreApplication::translate("ConsolidateDialog", "Export parameters", nullptr));
        targetProjectNameLab->setText(QCoreApplication::translate("ConsolidateDialog", "Target project name", nullptr));
        targetPathLabel->setText(QCoreApplication::translate("ConsolidateDialog", "Target path", nullptr));
        targetDirButton->setText(QCoreApplication::translate("ConsolidateDialog", "...", nullptr));
        keepFxIDsCheck->setText(QCoreApplication::translate("ConsolidateDialog", "Keep FX IDs", nullptr));
        cancelButton->setText(QCoreApplication::translate("ConsolidateDialog", "Cancel", nullptr));
        okButton->setText(QCoreApplication::translate("ConsolidateDialog", "Consolidate", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConsolidateDialog: public Ui_ConsolidateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLIDATEDIALOG_H
