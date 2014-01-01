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
    QPushButton *closeButton;
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
        titleLabel = new QLabel(FxListWidget);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);

        horizontalLayout->addWidget(titleLabel);

        loopCheck = new QCheckBox(FxListWidget);
        loopCheck->setObjectName(QStringLiteral("loopCheck"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(loopCheck->sizePolicy().hasHeightForWidth());
        loopCheck->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(loopCheck);

        randomCheckBox = new QCheckBox(FxListWidget);
        randomCheckBox->setObjectName(QStringLiteral("randomCheckBox"));

        horizontalLayout->addWidget(randomCheckBox);

        autoProceedCheck = new QCheckBox(FxListWidget);
        autoProceedCheck->setObjectName(QStringLiteral("autoProceedCheck"));

        horizontalLayout->addWidget(autoProceedCheck);

        closeButton = new QPushButton(FxListWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);

        fxTable = new PTableWidget(FxListWidget);
        fxTable->setObjectName(QStringLiteral("fxTable"));

        verticalLayout->addWidget(fxTable);


        retranslateUi(FxListWidget);
        QObject::connect(closeButton, SIGNAL(clicked()), FxListWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(FxListWidget);
    } // setupUi

    void retranslateUi(QWidget *FxListWidget)
    {
        FxListWidget->setWindowTitle(QApplication::translate("FxListWidget", "FxList", 0));
        titleLabel->setText(QApplication::translate("FxListWidget", "Fx List", 0));
        loopCheck->setText(QApplication::translate("FxListWidget", "Loop", 0));
        randomCheckBox->setText(QApplication::translate("FxListWidget", "Random", 0));
        autoProceedCheck->setText(QApplication::translate("FxListWidget", "Auto proceed", 0));
        closeButton->setText(QApplication::translate("FxListWidget", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class FxListWidget: public Ui_FxListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXLISTWIDGET_H
