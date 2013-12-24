/********************************************************************************
** Form generated from reading UI file 'fxlistwidgetitem.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXLISTWIDGETITEM_H
#define UI_FXLISTWIDGETITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include "customwidget/pslabel.h"
#include "customwidget/pslineedit.h"

QT_BEGIN_NAMESPACE

class Ui_FxListWidgetItem
{
public:
    QHBoxLayout *horizontalLayout;
    PsLabel *itemLabel;
    PsLineEdit *itemEdit;

    void setupUi(QWidget *FxListWidgetItem)
    {
        if (FxListWidgetItem->objectName().isEmpty())
            FxListWidgetItem->setObjectName(QStringLiteral("FxListWidgetItem"));
        FxListWidgetItem->resize(182, 25);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FxListWidgetItem->sizePolicy().hasHeightForWidth());
        FxListWidgetItem->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(FxListWidgetItem);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        itemLabel = new PsLabel(FxListWidgetItem);
        itemLabel->setObjectName(QStringLiteral("itemLabel"));

        horizontalLayout->addWidget(itemLabel);

        itemEdit = new PsLineEdit(FxListWidgetItem);
        itemEdit->setObjectName(QStringLiteral("itemEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(itemEdit->sizePolicy().hasHeightForWidth());
        itemEdit->setSizePolicy(sizePolicy1);
        itemEdit->setFrame(false);

        horizontalLayout->addWidget(itemEdit);


        retranslateUi(FxListWidgetItem);

        QMetaObject::connectSlotsByName(FxListWidgetItem);
    } // setupUi

    void retranslateUi(QWidget *FxListWidgetItem)
    {
        FxListWidgetItem->setWindowTitle(QApplication::translate("FxListWidgetItem", "Form", 0));
        itemLabel->setText(QApplication::translate("FxListWidgetItem", "Dummy", 0));
    } // retranslateUi

};

namespace Ui {
    class FxListWidgetItem: public Ui_FxListWidgetItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXLISTWIDGETITEM_H
