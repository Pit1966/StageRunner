#include "setupwidget.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/ioplugincentral.h"
#include "plugins/interfaces/qlcioplugin.h"
#include "appcontrol/usersettings.h"

#include <QListWidget>
#include <QListWidgetItem>

SetupWidget::SetupWidget(AppCentral *app_central, QWidget *parent)
	: myapp(app_central)
	, QDialog(parent)
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
	cur_selected_qlc_plugin = myapp->pluginCentral->getQLCPluginByName(item->text());

	if (cur_selected_qlc_plugin) {
		qlcPluginEdit->setHtml(cur_selected_qlc_plugin->outputInfo(0));
		configurePluginButton->setEnabled(true);
	} else {
		configurePluginButton->setEnabled(false);
	}
}

void SetupWidget::on_configurePluginButton_clicked()
{
	if (cur_selected_qlc_plugin) {
		cur_selected_qlc_plugin->configure();
	}
}
