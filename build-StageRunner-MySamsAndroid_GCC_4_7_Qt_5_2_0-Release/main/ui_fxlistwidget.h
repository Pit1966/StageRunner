/********************************************************************************
** Form generated from reading UI file 'fxlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXLISTWIDGET_H
#define UI_FXLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gui/ptablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_FxListWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QCheckBox *loopCheck;
    QCheckBox *autoProceedCheck;
    PTableWidget *fxTable;

    void setupUi(QWidget *FxListWidget)
    {
        if (FxListWidget->objectName().isEmpty())
            FxListWidget->setObjectName(QStringLiteral("FxListWidget"));
        FxListWidget->resize(514, 641);
        verticalLayout = new QVBoxLayout(FxListWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        label = new QLabel(FxListWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        loopCheck = new QCheckBox(FxListWidget);
        loopCheck->setObjectName(QStringLiteral("loopCheck"));

        horizontalLayout->addWidget(loopCheck);

        autoProceedCheck = new QCheckBox(FxListWidget);
        autoProceedCheck->setObjectName(QStringLiteral("autoProceedCheck"));

        horizontalLayout->addWidget(autoProceedCheck);


        verticalLayout->addLayout(horizontalLayout);

        fxTable = new PTableWidget(FxListWidget);
        fxTable->setObjectName(QStringLiteral("fxTable"));

        verticalLayout->addWidget(fxTable);


        retranslateUi(FxListWidget);

        QMetaObject::connectSlotsByName(FxListWidget);
    } // setupUi

    void retranslateUi(QWidget *FxListWidget)
    {
        FxListWidget->setWindowTitle(QApplication::translate("FxListWidget", "FxList", 0));
        label->setText(QApplication::translate("FxListWidget", "Fx List", 0));
        loopCheck->setText(QApplication::translate("FxListWidget", "Loop", 0));
        autoProceedCheck->setText(QApplication::translate("FxListWidget", "Auto proceed Sequence", 0));
    } // retranslateUi

};

namespace Ui {
    class FxListWidget: public Ui_FxListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXLISTWIDGET_H
