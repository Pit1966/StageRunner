/********************************************************************************
** Form generated from reading UI file 'fxscriptwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXSCRIPTWIDGET_H
#define UI_FXSCRIPTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "widgets/scripteditwidget.h"

QT_BEGIN_NAMESPACE

class Ui_FxScriptWidget
{
public:
    QVBoxLayout *verticalLayout;
    ScriptEditWidget *scriptEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QPushButton *applyCloseButton;

    void setupUi(QWidget *FxScriptWidget)
    {
        if (FxScriptWidget->objectName().isEmpty())
            FxScriptWidget->setObjectName(QString::fromUtf8("FxScriptWidget"));
        FxScriptWidget->resize(400, 372);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/script.png"), QSize(), QIcon::Normal, QIcon::Off);
        FxScriptWidget->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(FxScriptWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scriptEdit = new ScriptEditWidget(FxScriptWidget);
        scriptEdit->setObjectName(QString::fromUtf8("scriptEdit"));

        verticalLayout->addWidget(scriptEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cancelButton = new QPushButton(FxScriptWidget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        applyButton = new QPushButton(FxScriptWidget);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));

        horizontalLayout->addWidget(applyButton);

        applyCloseButton = new QPushButton(FxScriptWidget);
        applyCloseButton->setObjectName(QString::fromUtf8("applyCloseButton"));

        horizontalLayout->addWidget(applyCloseButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(FxScriptWidget);

        QMetaObject::connectSlotsByName(FxScriptWidget);
    } // setupUi

    void retranslateUi(QWidget *FxScriptWidget)
    {
        FxScriptWidget->setWindowTitle(QCoreApplication::translate("FxScriptWidget", "ScriptEditor", nullptr));
        cancelButton->setText(QCoreApplication::translate("FxScriptWidget", "Cancel", nullptr));
        applyButton->setText(QCoreApplication::translate("FxScriptWidget", "Apply", nullptr));
        applyCloseButton->setText(QCoreApplication::translate("FxScriptWidget", "Apply & Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FxScriptWidget: public Ui_FxScriptWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXSCRIPTWIDGET_H
