/********************************************************************************
** Form generated from reading UI file 'fxlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXLISTWIDGET_H
#define UI_FXLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gui/ptablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_FxListWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *titleLabel;
    QCheckBox *loopCheck;
    QCheckBox *randomCheckBox;
    QCheckBox *autoProceedCheck;
    QPushButton *editButton;
    QPushButton *closeButton;
    PTableWidget *fxTable;

    void setupUi(QWidget *FxListWidget)
    {
        if (FxListWidget->objectName().isEmpty())
            FxListWidget->setObjectName(QString::fromUtf8("FxListWidget"));
        FxListWidget->resize(514, 641);
        verticalLayout = new QVBoxLayout(FxListWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        titleLabel = new QLabel(FxListWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);

        horizontalLayout->addWidget(titleLabel);

        loopCheck = new QCheckBox(FxListWidget);
        loopCheck->setObjectName(QString::fromUtf8("loopCheck"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(loopCheck->sizePolicy().hasHeightForWidth());
        loopCheck->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(loopCheck);

        randomCheckBox = new QCheckBox(FxListWidget);
        randomCheckBox->setObjectName(QString::fromUtf8("randomCheckBox"));

        horizontalLayout->addWidget(randomCheckBox);

        autoProceedCheck = new QCheckBox(FxListWidget);
        autoProceedCheck->setObjectName(QString::fromUtf8("autoProceedCheck"));

        horizontalLayout->addWidget(autoProceedCheck);

        editButton = new QPushButton(FxListWidget);
        editButton->setObjectName(QString::fromUtf8("editButton"));
        editButton->setCheckable(true);

        horizontalLayout->addWidget(editButton);

        closeButton = new QPushButton(FxListWidget);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);

        fxTable = new PTableWidget(FxListWidget);
        fxTable->setObjectName(QString::fromUtf8("fxTable"));

        verticalLayout->addWidget(fxTable);


        retranslateUi(FxListWidget);

        QMetaObject::connectSlotsByName(FxListWidget);
    } // setupUi

    void retranslateUi(QWidget *FxListWidget)
    {
        FxListWidget->setWindowTitle(QCoreApplication::translate("FxListWidget", "FxList", nullptr));
        titleLabel->setText(QCoreApplication::translate("FxListWidget", "Fx List", nullptr));
        loopCheck->setText(QCoreApplication::translate("FxListWidget", "Loop", nullptr));
        randomCheckBox->setText(QCoreApplication::translate("FxListWidget", "Random", nullptr));
        autoProceedCheck->setText(QCoreApplication::translate("FxListWidget", "Auto proceed", nullptr));
        editButton->setText(QCoreApplication::translate("FxListWidget", "Edit", nullptr));
        closeButton->setText(QCoreApplication::translate("FxListWidget", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FxListWidget: public Ui_FxListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXLISTWIDGET_H
