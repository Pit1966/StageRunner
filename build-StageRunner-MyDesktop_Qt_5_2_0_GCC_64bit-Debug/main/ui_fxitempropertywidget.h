/********************************************************************************
** Form generated from reading UI file 'fxitempropertywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXITEMPROPERTYWIDGET_H
#define UI_FXITEMPROPERTYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "customwidget/pslineedit.h"
#include "thirdparty/widget/qsynthknob.h"

QT_BEGIN_NAMESPACE

class Ui_FxItemPropertyWidget
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *fxGroup;
    QGridLayout *gridLayout;
    PsLineEdit *keyEdit;
    QLabel *nameLabel;
    QLabel *keyLabel;
    QLabel *idLabel;
    PsLineEdit *idEdit;
    QPushButton *keyClearButton;
    PsLineEdit *nameEdit;
    QGroupBox *audioGroup;
    QGridLayout *gridLayout_2;
    PsLineEdit *audioFilePathEdit;
    qsynthKnob *initialVolDial;
    QLabel *audioFilePathLabel;
    QLabel *label;
    QLabel *audioLoopsLabel;
    QSpinBox *audioLoopsSpin;
    QGroupBox *sceneGroup;
    QGridLayout *gridLayout_3;
    PsLineEdit *fadeInTimeEdit;
    PsLineEdit *faderCountEdit;
    QLabel *faderCountLabel;
    PsLineEdit *fadeOutTimeEdit;
    QLabel *fadeInTimeLabel;
    QLabel *fadeOutTimeLabel;
    QPushButton *editOnceButton;
    QPushButton *closeButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *FxItemPropertyWidget)
    {
        if (FxItemPropertyWidget->objectName().isEmpty())
            FxItemPropertyWidget->setObjectName(QStringLiteral("FxItemPropertyWidget"));
        FxItemPropertyWidget->resize(204, 791);
        QFont font;
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        FxItemPropertyWidget->setFont(font);
        FxItemPropertyWidget->setAutoFillBackground(false);
        verticalLayout = new QVBoxLayout(FxItemPropertyWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        fxGroup = new QGroupBox(FxItemPropertyWidget);
        fxGroup->setObjectName(QStringLiteral("fxGroup"));
        gridLayout = new QGridLayout(fxGroup);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        keyEdit = new PsLineEdit(fxGroup);
        keyEdit->setObjectName(QStringLiteral("keyEdit"));
        keyEdit->setMinimumSize(QSize(60, 0));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        keyEdit->setFont(font1);
        keyEdit->setMaxLength(10);
        keyEdit->setFrame(true);

        gridLayout->addWidget(keyEdit, 2, 1, 1, 1);

        nameLabel = new QLabel(fxGroup);
        nameLabel->setObjectName(QStringLiteral("nameLabel"));

        gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

        keyLabel = new QLabel(fxGroup);
        keyLabel->setObjectName(QStringLiteral("keyLabel"));

        gridLayout->addWidget(keyLabel, 2, 0, 1, 1);

        idLabel = new QLabel(fxGroup);
        idLabel->setObjectName(QStringLiteral("idLabel"));

        gridLayout->addWidget(idLabel, 3, 0, 1, 1);

        idEdit = new PsLineEdit(fxGroup);
        idEdit->setObjectName(QStringLiteral("idEdit"));
        idEdit->setMinimumSize(QSize(60, 0));
        idEdit->setMaximumSize(QSize(50, 16777215));
        idEdit->setReadOnly(true);

        gridLayout->addWidget(idEdit, 3, 1, 1, 1);

        keyClearButton = new QPushButton(fxGroup);
        keyClearButton->setObjectName(QStringLiteral("keyClearButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(keyClearButton->sizePolicy().hasHeightForWidth());
        keyClearButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(keyClearButton, 2, 2, 1, 1);

        nameEdit = new PsLineEdit(fxGroup);
        nameEdit->setObjectName(QStringLiteral("nameEdit"));

        gridLayout->addWidget(nameEdit, 1, 0, 1, 3);


        verticalLayout->addWidget(fxGroup);

        audioGroup = new QGroupBox(FxItemPropertyWidget);
        audioGroup->setObjectName(QStringLiteral("audioGroup"));
        gridLayout_2 = new QGridLayout(audioGroup);
        gridLayout_2->setContentsMargins(2, 2, 2, 2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        audioFilePathEdit = new PsLineEdit(audioGroup);
        audioFilePathEdit->setObjectName(QStringLiteral("audioFilePathEdit"));

        gridLayout_2->addWidget(audioFilePathEdit, 0, 1, 1, 1);

        initialVolDial = new qsynthKnob(audioGroup);
        initialVolDial->setObjectName(QStringLiteral("initialVolDial"));
        initialVolDial->setMaximumSize(QSize(80, 80));
        initialVolDial->setMaximum(100);
        initialVolDial->setNotchTarget(10);
        initialVolDial->setNotchesVisible(true);

        gridLayout_2->addWidget(initialVolDial, 1, 1, 1, 1);

        audioFilePathLabel = new QLabel(audioGroup);
        audioFilePathLabel->setObjectName(QStringLiteral("audioFilePathLabel"));

        gridLayout_2->addWidget(audioFilePathLabel, 0, 0, 1, 1);

        label = new QLabel(audioGroup);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        audioLoopsLabel = new QLabel(audioGroup);
        audioLoopsLabel->setObjectName(QStringLiteral("audioLoopsLabel"));

        gridLayout_2->addWidget(audioLoopsLabel, 2, 0, 1, 1);

        audioLoopsSpin = new QSpinBox(audioGroup);
        audioLoopsSpin->setObjectName(QStringLiteral("audioLoopsSpin"));
        audioLoopsSpin->setMinimum(-1);
        audioLoopsSpin->setMaximum(999);

        gridLayout_2->addWidget(audioLoopsSpin, 2, 1, 1, 1);


        verticalLayout->addWidget(audioGroup);

        sceneGroup = new QGroupBox(FxItemPropertyWidget);
        sceneGroup->setObjectName(QStringLiteral("sceneGroup"));
        gridLayout_3 = new QGridLayout(sceneGroup);
        gridLayout_3->setContentsMargins(2, 2, 2, 2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        fadeInTimeEdit = new PsLineEdit(sceneGroup);
        fadeInTimeEdit->setObjectName(QStringLiteral("fadeInTimeEdit"));

        gridLayout_3->addWidget(fadeInTimeEdit, 1, 1, 1, 1);

        faderCountEdit = new PsLineEdit(sceneGroup);
        faderCountEdit->setObjectName(QStringLiteral("faderCountEdit"));
        faderCountEdit->setFrame(true);

        gridLayout_3->addWidget(faderCountEdit, 0, 1, 1, 1);

        faderCountLabel = new QLabel(sceneGroup);
        faderCountLabel->setObjectName(QStringLiteral("faderCountLabel"));
        faderCountLabel->setWordWrap(true);

        gridLayout_3->addWidget(faderCountLabel, 0, 0, 1, 1);

        fadeOutTimeEdit = new PsLineEdit(sceneGroup);
        fadeOutTimeEdit->setObjectName(QStringLiteral("fadeOutTimeEdit"));

        gridLayout_3->addWidget(fadeOutTimeEdit, 2, 1, 1, 1);

        fadeInTimeLabel = new QLabel(sceneGroup);
        fadeInTimeLabel->setObjectName(QStringLiteral("fadeInTimeLabel"));

        gridLayout_3->addWidget(fadeInTimeLabel, 1, 0, 1, 1);

        fadeOutTimeLabel = new QLabel(sceneGroup);
        fadeOutTimeLabel->setObjectName(QStringLiteral("fadeOutTimeLabel"));

        gridLayout_3->addWidget(fadeOutTimeLabel, 2, 0, 1, 1);


        verticalLayout->addWidget(sceneGroup);

        editOnceButton = new QPushButton(FxItemPropertyWidget);
        editOnceButton->setObjectName(QStringLiteral("editOnceButton"));

        verticalLayout->addWidget(editOnceButton);

        closeButton = new QPushButton(FxItemPropertyWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        verticalLayout->addWidget(closeButton);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(FxItemPropertyWidget);
        QObject::connect(closeButton, SIGNAL(clicked()), FxItemPropertyWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(FxItemPropertyWidget);
    } // setupUi

    void retranslateUi(QWidget *FxItemPropertyWidget)
    {
        FxItemPropertyWidget->setWindowTitle(QApplication::translate("FxItemPropertyWidget", "Fx Editor", 0));
        fxGroup->setTitle(QApplication::translate("FxItemPropertyWidget", "Fx", 0));
        nameLabel->setText(QApplication::translate("FxItemPropertyWidget", "Name:", 0));
        keyLabel->setText(QApplication::translate("FxItemPropertyWidget", "Key:", 0));
        idLabel->setText(QApplication::translate("FxItemPropertyWidget", "Id:", 0));
        keyClearButton->setText(QApplication::translate("FxItemPropertyWidget", "Clear", 0));
        audioGroup->setTitle(QApplication::translate("FxItemPropertyWidget", "Audio", 0));
        audioFilePathLabel->setText(QApplication::translate("FxItemPropertyWidget", "File path:", 0));
        label->setText(QApplication::translate("FxItemPropertyWidget", "Initial\n"
"Volume", 0));
        audioLoopsLabel->setText(QApplication::translate("FxItemPropertyWidget", "Loops", 0));
        sceneGroup->setTitle(QApplication::translate("FxItemPropertyWidget", "Scene", 0));
        faderCountLabel->setText(QApplication::translate("FxItemPropertyWidget", "Fader\n"
"count", 0));
        fadeInTimeLabel->setText(QApplication::translate("FxItemPropertyWidget", "Fade In Time:", 0));
        fadeOutTimeLabel->setText(QApplication::translate("FxItemPropertyWidget", "Fade Out Time:", 0));
        editOnceButton->setText(QApplication::translate("FxItemPropertyWidget", "Edit once", 0));
        closeButton->setText(QApplication::translate("FxItemPropertyWidget", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class FxItemPropertyWidget: public Ui_FxItemPropertyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXITEMPROPERTYWIDGET_H
