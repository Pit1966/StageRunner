/********************************************************************************
** Form generated from reading UI file 'fxplaylistwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXPLAYLISTWIDGET_H
#define UI_FXPLAYLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "customwidget/pslistwidget.h"

QT_BEGIN_NAMESPACE

class Ui_FxPlayListWidget
{
public:
    QVBoxLayout *verticalLayout;
    PsListWidget *playListWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *closeButton;

    void setupUi(QWidget *FxPlayListWidget)
    {
        if (FxPlayListWidget->objectName().isEmpty())
            FxPlayListWidget->setObjectName(QString::fromUtf8("FxPlayListWidget"));
        FxPlayListWidget->resize(477, 649);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/audio_speaker_grey.png"), QSize(), QIcon::Normal, QIcon::Off);
        FxPlayListWidget->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(FxPlayListWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        playListWidget = new PsListWidget(FxPlayListWidget);
        playListWidget->setObjectName(QString::fromUtf8("playListWidget"));
        playListWidget->setSortingEnabled(false);

        verticalLayout->addWidget(playListWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        closeButton = new QPushButton(FxPlayListWidget);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(FxPlayListWidget);

        QMetaObject::connectSlotsByName(FxPlayListWidget);
    } // setupUi

    void retranslateUi(QWidget *FxPlayListWidget)
    {
        FxPlayListWidget->setWindowTitle(QCoreApplication::translate("FxPlayListWidget", "Audio Play List Editor", nullptr));
        closeButton->setText(QCoreApplication::translate("FxPlayListWidget", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FxPlayListWidget: public Ui_FxPlayListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXPLAYLISTWIDGET_H
