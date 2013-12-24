/********************************************************************************
** Form generated from reading UI file 'lightcontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIGHTCONTROLWIDGET_H
#define UI_LIGHTCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_LightControlWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *fadeToBlackButton;
    QPushButton *blackButton;
    QSpinBox *fadeSecondsSpin;

    void setupUi(QGroupBox *LightControlWidget)
    {
        if (LightControlWidget->objectName().isEmpty())
            LightControlWidget->setObjectName(QStringLiteral("LightControlWidget"));
        LightControlWidget->resize(400, 300);
        gridLayout = new QGridLayout(LightControlWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        fadeToBlackButton = new QPushButton(LightControlWidget);
        fadeToBlackButton->setObjectName(QStringLiteral("fadeToBlackButton"));
        fadeToBlackButton->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(fadeToBlackButton, 0, 1, 1, 1);

        blackButton = new QPushButton(LightControlWidget);
        blackButton->setObjectName(QStringLiteral("blackButton"));
        blackButton->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(blackButton, 0, 0, 1, 1);

        fadeSecondsSpin = new QSpinBox(LightControlWidget);
        fadeSecondsSpin->setObjectName(QStringLiteral("fadeSecondsSpin"));
        fadeSecondsSpin->setMinimumSize(QSize(0, 40));
        fadeSecondsSpin->setMaximumSize(QSize(60, 16777215));
        fadeSecondsSpin->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        fadeSecondsSpin->setMinimum(1);
        fadeSecondsSpin->setMaximum(10);
        fadeSecondsSpin->setValue(3);

        gridLayout->addWidget(fadeSecondsSpin, 0, 2, 1, 1);


        retranslateUi(LightControlWidget);

        QMetaObject::connectSlotsByName(LightControlWidget);
    } // setupUi

    void retranslateUi(QGroupBox *LightControlWidget)
    {
        LightControlWidget->setWindowTitle(QApplication::translate("LightControlWidget", "Light Control", 0));
        fadeToBlackButton->setText(QApplication::translate("LightControlWidget", "Fade to BLACK", 0));
        blackButton->setText(QApplication::translate("LightControlWidget", "BLACK", 0));
        fadeSecondsSpin->setSuffix(QApplication::translate("LightControlWidget", " sec", 0));
    } // retranslateUi

};

namespace Ui {
    class LightControlWidget: public Ui_LightControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIGHTCONTROLWIDGET_H
