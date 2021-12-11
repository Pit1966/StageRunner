/********************************************************************************
** Form generated from reading UI file 'fxlistwidgetitem.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXLISTWIDGETITEM_H
#define UI_FXLISTWIDGETITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
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
    PsLabel *itemExtra;

    void setupUi(QWidget *FxListWidgetItem)
    {
        if (FxListWidgetItem->objectName().isEmpty())
            FxListWidgetItem->setObjectName(QString::fromUtf8("FxListWidgetItem"));
        FxListWidgetItem->resize(182, 25);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FxListWidgetItem->sizePolicy().hasHeightForWidth());
        FxListWidgetItem->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(FxListWidgetItem);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        itemLabel = new PsLabel(FxListWidgetItem);
        itemLabel->setObjectName(QString::fromUtf8("itemLabel"));

        horizontalLayout->addWidget(itemLabel);

        itemEdit = new PsLineEdit(FxListWidgetItem);
        itemEdit->setObjectName(QString::fromUtf8("itemEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(itemEdit->sizePolicy().hasHeightForWidth());
        itemEdit->setSizePolicy(sizePolicy1);
        itemEdit->setFrame(false);

        horizontalLayout->addWidget(itemEdit);

        itemExtra = new PsLabel(FxListWidgetItem);
        itemExtra->setObjectName(QString::fromUtf8("itemExtra"));

        horizontalLayout->addWidget(itemExtra);


        retranslateUi(FxListWidgetItem);

        QMetaObject::connectSlotsByName(FxListWidgetItem);
    } // setupUi

    void retranslateUi(QWidget *FxListWidgetItem)
    {
        FxListWidgetItem->setWindowTitle(QCoreApplication::translate("FxListWidgetItem", "Form", nullptr));
        itemLabel->setText(QCoreApplication::translate("FxListWidgetItem", "Dummy", nullptr));
        itemExtra->setText(QCoreApplication::translate("FxListWidgetItem", "Dummy", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FxListWidgetItem: public Ui_FxListWidgetItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXLISTWIDGETITEM_H
