/********************************************************************************
** Form generated from reading UI file 'sequencestatuswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCESTATUSWIDGET_H
#define UI_SEQUENCESTATUSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
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
            SequenceStatusWidget->setObjectName(QStringLiteral("SequenceStatusWidget"));
        SequenceStatusWidget->resize(400, 433);
        verticalLayout = new QVBoxLayout(SequenceStatusWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        execHashSizeLabel = new QLabel(SequenceStatusWidget);
        execHashSizeLabel->setObjectName(QStringLiteral("execHashSizeLabel"));

        horizontalLayout->addWidget(execHashSizeLabel);

        execHashSizeNum = new QLabel(SequenceStatusWidget);
        execHashSizeNum->setObjectName(QStringLiteral("execHashSizeNum"));

        horizontalLayout->addWidget(execHashSizeNum);

        fxHashSizeLabel = new QLabel(SequenceStatusWidget);
        fxHashSizeLabel->setObjectName(QStringLiteral("fxHashSizeLabel"));

        horizontalLayout->addWidget(fxHashSizeLabel);

        fxHashSizeNum = new QLabel(SequenceStatusWidget);
        fxHashSizeNum->setObjectName(QStringLiteral("fxHashSizeNum"));

        horizontalLayout->addWidget(fxHashSizeNum);


        verticalLayout->addLayout(horizontalLayout);

        seqListWidget = new QListWidget(SequenceStatusWidget);
        seqListWidget->setObjectName(QStringLiteral("seqListWidget"));

        verticalLayout->addWidget(seqListWidget);


        retranslateUi(SequenceStatusWidget);

        QMetaObject::connectSlotsByName(SequenceStatusWidget);
    } // setupUi

    void retranslateUi(QWidget *SequenceStatusWidget)
    {
        SequenceStatusWidget->setWindowTitle(QApplication::translate("SequenceStatusWidget", "Form", 0));
        execHashSizeLabel->setText(QApplication::translate("SequenceStatusWidget", "ExecuterHashSize", 0));
        execHashSizeNum->setText(QApplication::translate("SequenceStatusWidget", "---", 0));
        fxHashSizeLabel->setText(QApplication::translate("SequenceStatusWidget", "FxHashSize", 0));
        fxHashSizeNum->setText(QApplication::translate("SequenceStatusWidget", "---", 0));
    } // retranslateUi

};

namespace Ui {
    class SequenceStatusWidget: public Ui_SequenceStatusWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCESTATUSWIDGET_H
