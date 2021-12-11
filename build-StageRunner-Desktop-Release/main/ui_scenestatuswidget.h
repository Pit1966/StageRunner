/********************************************************************************
** Form generated from reading UI file 'scenestatuswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENESTATUSWIDGET_H
#define UI_SCENESTATUSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SceneStatusWidget
{
public:
    QVBoxLayout *verticalLayout;
    QCheckBox *showInactiveCheck;
    QListWidget *sceneListWidget;

    void setupUi(QWidget *SceneStatusWidget)
    {
        if (SceneStatusWidget->objectName().isEmpty())
            SceneStatusWidget->setObjectName(QString::fromUtf8("SceneStatusWidget"));
        SceneStatusWidget->resize(409, 339);
        verticalLayout = new QVBoxLayout(SceneStatusWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        showInactiveCheck = new QCheckBox(SceneStatusWidget);
        showInactiveCheck->setObjectName(QString::fromUtf8("showInactiveCheck"));

        verticalLayout->addWidget(showInactiveCheck);

        sceneListWidget = new QListWidget(SceneStatusWidget);
        sceneListWidget->setObjectName(QString::fromUtf8("sceneListWidget"));

        verticalLayout->addWidget(sceneListWidget);


        retranslateUi(SceneStatusWidget);

        QMetaObject::connectSlotsByName(SceneStatusWidget);
    } // setupUi

    void retranslateUi(QWidget *SceneStatusWidget)
    {
        SceneStatusWidget->setWindowTitle(QCoreApplication::translate("SceneStatusWidget", "Scene Status", nullptr));
        showInactiveCheck->setText(QCoreApplication::translate("SceneStatusWidget", "Show inactive used scenes", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneStatusWidget: public Ui_SceneStatusWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENESTATUSWIDGET_H
