/********************************************************************************
** Form generated from reading UI file 'audiocontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOCONTROLWIDGET_H
#define UI_AUDIOCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AudioControlWidget
{
public:
    QVBoxLayout *topLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *mainLayout;
    QLabel *statusLabel;

    void setupUi(QGroupBox *AudioControlWidget)
    {
        if (AudioControlWidget->objectName().isEmpty())
            AudioControlWidget->setObjectName(QStringLiteral("AudioControlWidget"));
        AudioControlWidget->resize(400, 300);
        topLayout = new QVBoxLayout(AudioControlWidget);
        topLayout->setSpacing(2);
        topLayout->setObjectName(QStringLiteral("topLayout"));
        topLayout->setContentsMargins(0, 4, 0, 0);
        scrollArea = new QScrollArea(AudioControlWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 394, 272));
        mainLayout = new QVBoxLayout(scrollAreaWidgetContents);
        mainLayout->setSpacing(2);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(2, 2, 2, 2);
        statusLabel = new QLabel(scrollAreaWidgetContents);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(statusLabel);

        scrollArea->setWidget(scrollAreaWidgetContents);

        topLayout->addWidget(scrollArea);


        retranslateUi(AudioControlWidget);

        QMetaObject::connectSlotsByName(AudioControlWidget);
    } // setupUi

    void retranslateUi(QGroupBox *AudioControlWidget)
    {
        AudioControlWidget->setWindowTitle(QApplication::translate("AudioControlWidget", "GroupBox", 0));
        AudioControlWidget->setTitle(QApplication::translate("AudioControlWidget", "Audio Control", 0));
        statusLabel->setText(QApplication::translate("AudioControlWidget", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class AudioControlWidget: public Ui_AudioControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOCONTROLWIDGET_H
