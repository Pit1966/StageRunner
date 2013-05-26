#include "setupwidget.h"
#include "config.h"
#include "appcentral.h"
#include "ioplugincentral.h"
#include "plugins/interfaces/qlcioplugin.h"
#include "usersettings.h"
#include "pluginmapping.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QSpinBox>

SetupWidget::SetupWidget(AppCentral *app_central, QWidget *parent)
	: QDialog(parent)
	, myapp(app_central)
{
	init();

	setupUi(this);

	myapp->pluginCentral->updatePluginMappingInformation();
	cur_plugin_map = myapp->pluginCentral->pluginMapping;

	QList<QLCIOPlugin*>qlcplugins = myapp->pluginCentral->qlcPlugins();
	for (int t=0; t<qlcplugins.size(); t++) {
		QLCIOPlugin *plugin = qlcplugins.at(t);
		qlcPluginsList->addItem(plugin->name());
	}

	configurePluginButton->setDisabled(true);
	copy_settings_to_gui();
}

void SetupWidget::init()
{
	cur_selected_qlc_plugin = 0;
	cur_plugin_map = 0;
}

void SetupWidget::copy_settings_to_gui()
{
	UserSettings *set = myapp->userSettings;

	audioBufferSizeEdit->setText(QString::number(set->pAudioBufferSize));
}

void SetupWidget::copy_gui_to_settings()
{
	UserSettings *set = myapp->userSettings;
	set->pAudioBufferSize = audioBufferSizeEdit->text().toInt();
}

void SetupWidget::on_okButton_clicked()
{
	copy_gui_to_settings();
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

		update_dmx_mapping_table(cur_selected_qlc_plugin);

	}  else {
		configurePluginButton->setEnabled(false);
	}

	qlcPluginEdit->setHtml(info);
}

void SetupWidget::update_dmx_mapping_table(QLCIOPlugin *plugin)
{
	dmxMappingTable->clearContents();
	if (!plugin) return;

	QStringList outnames = plugin->outputs();
	QStringList innames = plugin->inputs();
	dmxMappingTable->setRowCount(outnames.size() + innames.size());
	dmxMappingTable->setColumnCount(3);

	QStringList header;
	header << tr("Line") << tr("DMX Universe") << tr("Name");
	dmxMappingTable->setHorizontalHeaderLabels(header);
	dmxMappingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	int row = 0;
	for (int t=0; t<outnames.size(); t++) {
		QTableWidgetItem *item;

		item = new QTableWidgetItem(tr("DMX Output"));
		dmxMappingTable->setItem(row,0,item);

		QSpinBox *spin = new QSpinBox();
		spin->setRange(1,MAX_DMX_UNIVERSE);
		spin->setPrefix(tr("universe:"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",outnames.at(t));
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_universe_changed(int)));
		dmxMappingTable->setCellWidget(row,1,spin);

		item = new QTableWidgetItem(outnames.at(t));
		dmxMappingTable->setItem(row,2,item);

		row++;
	}

	for (int t=0; t<innames.size(); t++) {
		QTableWidgetItem *item;

		item = new QTableWidgetItem(tr("DMX Input"));
		dmxMappingTable->setItem(row,0,item);

		QSpinBox *spin = new QSpinBox();
		spin->setRange(1,MAX_DMX_UNIVERSE);
		spin->setPrefix(tr("universe:"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",innames.at(t));
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_universe_changed(int)));
		dmxMappingTable->setCellWidget(row,1,spin);

		item = new QTableWidgetItem(innames.at(t));
		dmxMappingTable->setItem(row,2,item);

		row++;
	}
}


void SetupWidget::on_configurePluginButton_clicked()
{
	if (cur_selected_qlc_plugin) {
		cur_selected_qlc_plugin->configure();
	}
}

void SetupWidget::on_saveDmxConfigButton_clicked()
{
	AppCentral::instance()->pluginCentral->pluginMapping->saveToDefaultFile();
}

void SetupWidget::if_pluginline_universe_changed(int val)
{
	QString plugin_name = sender()->property("plugin").toString();
	QString plugin_line = sender()->property("line").toString();

	if (cur_plugin_map) {
		PluginConfig *lineconf = cur_plugin_map->getCreatePluginLineConfig(plugin_name,plugin_line);
		lineconf->pUniverse = val;
	}
}
