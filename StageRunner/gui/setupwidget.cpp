#include "setupwidget.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/ioplugincentral.h"
#include "plugins/interfaces/qlcioplugin.h"
#include "appcontrol/usersettings.h"

#include <QListWidget>
#include <QListWidgetItem>

SetupWidget::SetupWidget(AppCentral *app_central, QWidget *parent)
	: QDialog(parent)
	, myapp(app_central)
{
	init();

	setupUi(this);

	QList<QLCIOPlugin*>qlcplugins = myapp->pluginCentral->qlcPlugins();
	for (int t=0; t<qlcplugins.size(); t++) {
		QLCIOPlugin *plugin = qlcplugins.at(t);
		qlcPluginsList->addItem(plugin->name());
	}

	configurePluginButton->setDisabled(true);
}

void SetupWidget::init()
{
	cur_selected_qlc_plugin = 0;

}

void SetupWidget::on_okButton_clicked()
{
	accept();

}

void SetupWidget::on_cancelButton_clicked()
{
	reject();
}

void SetupWidget::on_qlcPluginsList_itemActivated(QListWidgetItem *item)
{
	QString info;

	cur_selected_qlc_plugin = myapp->pluginCentral->getQLCPluginByName(item->text());
	if (cur_selected_qlc_plugin) {
		configurePluginButton->setEnabled(true);

		info += tr("<br><font color=blue>Available outputs</font><br>");
		QStringList outputs = cur_selected_qlc_plugin->outputs();
		for (int t=0; t<outputs.size(); t++) {
			info += cur_selected_qlc_plugin->outputInfo(t);
		}

		info += tr("<br><font color=blue>Available inputs</font><br>");
		QStringList inputs = cur_selected_qlc_plugin->inputs();
		for (int t=0; t<inputs.size(); t++) {
			info += cur_selected_qlc_plugin->inputInfo(t);
		}

	}  else {
		configurePluginButton->setEnabled(false);
	}

	qlcPluginEdit->setHtml(info);
}

void SetupWidget::on_configurePluginButton_clicked()
{
	if (cur_selected_qlc_plugin) {
		cur_selected_qlc_plugin->configure();
	}
}
