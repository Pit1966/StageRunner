/********************************************************************************
** Form generated from reading UI file 'sequencestatuswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCESTATUSWIDGET_H
#define UI_SEQUENCESTATUSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SequenceStatusWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *execHashSizeLabel;
    QLabel *execHashSizeNum;
    QLabel *fxHashSizeLabel;
    QLabel *fxHashSizeNum;
    QListWidget *seqListWidget;

    void setupUi(QWidget *SequenceStatusWidget)
    {
        if (SequenceStatusWidget->objectName().isEmpty())
            SequenceStatusWidget->setObjectName(QString::fromUtf8("SequenceStatusWidget"));
        SequenceStatusWidget->resize(400, 433);
        verticalLayout = new QVBoxLayout(SequenceStatusWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        execHashSizeLabel = new QLabel(SequenceStatusWidget);
        execHashSizeLabel->setObjectName(QString::fromUtf8("execHashSizeLabel"));

        horizontalLayout->addWidget(execHashSizeLabel);

        execHashSizeNum = new QLabel(SequenceStatusWidget);
        execHashSizeNum->setObjectName(QString::fromUtf8("execHashSizeNum"));

        horizontalLayout->addWidget(execHashSizeNum);

        fxHashSizeLabel = new QLabel(SequenceStatusWidget);
        fxHashSizeLabel->setObjectName(QString::fromUtf8("fxHashSizeLabel"));

        horizontalLayout->addWidget(fxHashSizeLabel);

        fxHashSizeNum = new QLabel(SequenceStatusWidget);
        fxHashSizeNum->setObjectName(QString::fromUtf8("fxHashSizeNum"));

        horizontalLayout->addWidget(fxHashSizeNum);


        verticalLayout->addLayout(horizontalLayout);

        seqListWidget = new QListWidget(SequenceStatusWidget);
        seqListWidget->setObjectName(QString::fromUtf8("seqListWidget"));

        verticalLayout->addWidget(seqListWidget);


        retranslateUi(SequenceStatusWidget);

        QMetaObject::connectSlotsByName(SequenceStatusWidget);
    } // setupUi

    void retranslateUi(QWidget *SequenceStatusWidget)
    {
        SequenceStatusWidget->setWindowTitle(QCoreApplication::translate("SequenceStatusWidget", "Form", nullptr));
        execHashSizeLabel->setText(QCoreApplication::translate("SequenceStatusWidget", "ExecuterHashSize", nullptr));
        execHashSizeNum->setText(QCoreApplication::translate("SequenceStatusWidget", "---", nullptr));
        fxHashSizeLabel->setText(QCoreApplication::translate("SequenceStatusWidget", "FxHashSize", nullptr));
        fxHashSizeNum->setText(QCoreApplication::translate("SequenceStatusWidget", "---", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SequenceStatusWidget: public Ui_SequenceStatusWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCESTATUSWIDGET_H
