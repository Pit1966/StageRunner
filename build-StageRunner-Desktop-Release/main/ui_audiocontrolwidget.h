/********************************************************************************
** Form generated from reading UI file 'audiocontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOCONTROLWIDGET_H
#define UI_AUDIOCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
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
            AudioControlWidget->setObjectName(QString::fromUtf8("AudioControlWidget"));
        AudioControlWidget->resize(400, 300);
        topLayout = new QVBoxLayout(AudioControlWidget);
        topLayout->setSpacing(2);
        topLayout->setObjectName(QString::fromUtf8("topLayout"));
        topLayout->setContentsMargins(0, 4, 0, 0);
        scrollArea = new QScrollArea(AudioControlWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 394, 272));
        mainLayout = new QVBoxLayout(scrollAreaWidgetContents);
        mainLayout->setSpacing(2);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(2, 2, 2, 2);
        statusLabel = new QLabel(scrollAreaWidgetContents);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(statusLabel);

        scrollArea->setWidget(scrollAreaWidgetContents);

        topLayout->addWidget(scrollArea);


        retranslateUi(AudioControlWidget);

        QMetaObject::connectSlotsByName(AudioControlWidget);
    } // setupUi

    void retranslateUi(QGroupBox *AudioControlWidget)
    {
        AudioControlWidget->setWindowTitle(QCoreApplication::translate("AudioControlWidget", "GroupBox", nullptr));
        AudioControlWidget->setTitle(QCoreApplication::translate("AudioControlWidget", "Audio Control", nullptr));
        statusLabel->setText(QCoreApplication::translate("AudioControlWidget", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AudioControlWidget: public Ui_AudioControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOCONTROLWIDGET_H
