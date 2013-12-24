/********************************************************************************
** Form generated from reading UI file 'scenestatuswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENESTATUSWIDGET_H
#define UI_SCENESTATUSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SceneStatusWidget
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *sceneListWidget;

    void setupUi(QWidget *SceneStatusWidget)
    {
        if (SceneStatusWidget->objectName().isEmpty())
            SceneStatusWidget->setObjectName(QStringLiteral("SceneStatusWidget"));
        SceneStatusWidget->resize(409, 339);
        verticalLayout = new QVBoxLayout(SceneStatusWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        sceneListWidget = new QListWidget(SceneStatusWidget);
        sceneListWidget->setObjectName(QStringLiteral("sceneListWidget"));

        verticalLayout->addWidget(sceneListWidget);


        retranslateUi(SceneStatusWidget);

        QMetaObject::connectSlotsByName(SceneStatusWidget);
    } // setupUi

    void retranslateUi(QWidget *SceneStatusWidget)
    {
        SceneStatusWidget->setWindowTitle(QApplication::translate("SceneStatusWidget", "Scene Status", 0));
    } // retranslateUi

};

namespace Ui {
    class SceneStatusWidget: public Ui_SceneStatusWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENESTATUSWIDGET_H
