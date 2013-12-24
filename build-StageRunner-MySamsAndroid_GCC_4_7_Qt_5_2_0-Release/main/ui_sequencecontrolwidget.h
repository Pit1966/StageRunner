/********************************************************************************
** Form generated from reading UI file 'sequencecontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCECONTROLWIDGET_H
#define UI_SEQUENCECONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "thirdparty/widget/qsynthknob.h"

QT_BEGIN_NAMESPACE

class Ui_SequenceControlWidget
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *ctrlButtonLayout;
    qsynthKnob *masterVolDial;
    QPushButton *moveToPrevButton;
    QPushButton *ctrlStopButton;
    QPushButton *ctrlPlayButton;
    QPushButton *moveToNextButton;
    QGroupBox *nextSequenceGroup;
    QVBoxLayout *verticalLayout;
    QLabel *nextInSeqLabel;

    void setupUi(QGroupBox *SequenceControlWidget)
    {
        if (SequenceControlWidget->objectName().isEmpty())
            SequenceControlWidget->setObjectName(QStringLiteral("SequenceControlWidget"));
        SequenceControlWidget->resize(489, 235);
        mainLayout = new QVBoxLayout(SequenceControlWidget);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        ctrlButtonLayout = new QHBoxLayout();
        ctrlButtonLayout->setObjectName(QStringLiteral("ctrlButtonLayout"));
        masterVolDial = new qsynthKnob(SequenceControlWidget);
        masterVolDial->setObjectName(QStringLiteral("masterVolDial"));
        masterVolDial->setNotchesVisible(true);

        ctrlButtonLayout->addWidget(masterVolDial);

        moveToPrevButton = new QPushButton(SequenceControlWidget);
        moveToPrevButton->setObjectName(QStringLiteral("moveToPrevButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/gfx/icons/audio_back.png"), QSize(), QIcon::Normal, QIcon::Off);
        moveToPrevButton->setIcon(icon);
        moveToPrevButton->setIconSize(QSize(48, 48));
        moveToPrevButton->setFlat(true);

        ctrlButtonLayout->addWidget(moveToPrevButton);

        ctrlStopButton = new QPushButton(SequenceControlWidget);
        ctrlStopButton->setObjectName(QStringLiteral("ctrlStopButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/gfx/icons/audio_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ctrlStopButton->setIcon(icon1);
        ctrlStopButton->setIconSize(QSize(48, 48));
        ctrlStopButton->setFlat(true);

        ctrlButtonLayout->addWidget(ctrlStopButton);

        ctrlPlayButton = new QPushButton(SequenceControlWidget);
        ctrlPlayButton->setObjectName(QStringLiteral("ctrlPlayButton"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/gfx/icons/audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        ctrlPlayButton->setIcon(icon2);
        ctrlPlayButton->setIconSize(QSize(64, 64));
        ctrlPlayButton->setFlat(true);

        ctrlButtonLayout->addWidget(ctrlPlayButton);

        moveToNextButton = new QPushButton(SequenceControlWidget);
        moveToNextButton->setObjectName(QStringLiteral("moveToNextButton"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/gfx/icons/audio_skip.png"), QSize(), QIcon::Normal, QIcon::Off);
        moveToNextButton->setIcon(icon3);
        moveToNextButton->setIconSize(QSize(48, 48));
        moveToNextButton->setFlat(true);

        ctrlButtonLayout->addWidget(moveToNextButton);


        mainLayout->addLayout(ctrlButtonLayout);

        nextSequenceGroup = new QGroupBox(SequenceControlWidget);
        nextSequenceGroup->setObjectName(QStringLiteral("nextSequenceGroup"));
        verticalLayout = new QVBoxLayout(nextSequenceGroup);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        nextInSeqLabel = new QLabel(nextSequenceGroup);
        nextInSeqLabel->setObjectName(QStringLiteral("nextInSeqLabel"));
        QPalette palette;
        QBrush brush(QColor(25, 255, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(144, 141, 139, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        nextInSeqLabel->setPalette(palette);
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        nextInSeqLabel->setFont(font);

        verticalLayout->addWidget(nextInSeqLabel);


        mainLayout->addWidget(nextSequenceGroup);


        retranslateUi(SequenceControlWidget);

        QMetaObject::connectSlotsByName(SequenceControlWidget);
    } // setupUi

    void retranslateUi(QGroupBox *SequenceControlWidget)
    {
        SequenceControlWidget->setWindowTitle(QApplication::translate("SequenceControlWidget", "Form", 0));
        moveToPrevButton->setText(QString());
        ctrlStopButton->setText(QString());
        ctrlPlayButton->setText(QString());
        moveToNextButton->setText(QString());
        nextSequenceGroup->setTitle(QApplication::translate("SequenceControlWidget", "Next in sequence", 0));
        nextInSeqLabel->setText(QApplication::translate("SequenceControlWidget", "---", 0));
    } // retranslateUi

};

namespace Ui {
    class SequenceControlWidget: public Ui_SequenceControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCECONTROLWIDGET_H
