/********************************************************************************
** Form generated from reading UI file 'lightcontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIGHTCONTROLWIDGET_H
#define UI_LIGHTCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
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
            LightControlWidget->setObjectName(QString::fromUtf8("LightControlWidget"));
        LightControlWidget->resize(400, 84);
        gridLayout = new QGridLayout(LightControlWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        fadeToBlackButton = new QPushButton(LightControlWidget);
        fadeToBlackButton->setObjectName(QString::fromUtf8("fadeToBlackButton"));
        fadeToBlackButton->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(fadeToBlackButton, 0, 1, 1, 1);

        blackButton = new QPushButton(LightControlWidget);
        blackButton->setObjectName(QString::fromUtf8("blackButton"));
        blackButton->setMinimumSize(QSize(0, 40));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/backspace.png"), QSize(), QIcon::Normal, QIcon::Off);
        blackButton->setIcon(icon);

        gridLayout->addWidget(blackButton, 0, 0, 1, 1);

        fadeSecondsSpin = new QSpinBox(LightControlWidget);
        fadeSecondsSpin->setObjectName(QString::fromUtf8("fadeSecondsSpin"));
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
        LightControlWidget->setWindowTitle(QCoreApplication::translate("LightControlWidget", "Light Control", nullptr));
        fadeToBlackButton->setText(QCoreApplication::translate("LightControlWidget", "Fade to BLACK", nullptr));
        blackButton->setText(QCoreApplication::translate("LightControlWidget", "BLACK", nullptr));
        fadeSecondsSpin->setSuffix(QCoreApplication::translate("LightControlWidget", " sec", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LightControlWidget: public Ui_LightControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIGHTCONTROLWIDGET_H
