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
#include <QMessageBox>
#include <QCheckBox>

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
	update_plugin_mapping_f = false;
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
	if (update_plugin_mapping_f) {
		myapp->pluginCentral->updatePluginMappingInformation();
	}
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
	dmxMappingTable->setColumnCount(4);

	QStringList header;
	header << tr("Line") << tr("DMX Universe") << tr("Name") << tr("Response Time");
	dmxMappingTable->setHorizontalHeaderLabels(header);
	dmxMappingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	int row = 0;
	for (int t=0; t<outnames.size(); t++) {
		QTableWidgetItem *item;
		// Get current configuration of Plugin and Output-Line from pluginCentral
		PluginConfig *conf = 0;
		if (cur_plugin_map)
			conf = cur_plugin_map->getCreatePluginLineConfig(plugin->name(),outnames.at(t));

		item = new QTableWidgetItem(tr("Output"));
		if (conf && conf->pIsUsed) {
			item->setIcon(QIcon(":/gfx/icons/ledgreen.png"));
		} else {
			item->setIcon(QIcon(":/gfx/icons/ledred.png"));
		}
		dmxMappingTable->setItem(row,0,item);

		QSpinBox *spin = new QSpinBox();
		spin->setRange(0,MAX_DMX_UNIVERSE);
		spin->setPrefix(tr("universe:"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",outnames.at(t));
		spin->setProperty("tableRow",row);
		if (conf) spin->setValue(conf->pUniverse+1);
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_universe_changed(int)));
		dmxMappingTable->setCellWidget(row,1,spin);

		item = new QTableWidgetItem(outnames.at(t));
		dmxMappingTable->setItem(row,2,item);

		spin = new QSpinBox();
		spin->setRange(0,5000);
		spin->setSuffix(tr("ms"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",outnames.at(t));
		if (conf) spin->setValue(conf->pResponseTime);
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_responsetime_changed(int)));
		dmxMappingTable->setCellWidget(row,3,spin);

		row++;
	}

	for (int t=0; t<innames.size(); t++) {
		QTableWidgetItem *item;
		PluginConfig *conf = 0;
		if (cur_plugin_map)
			conf = myapp->pluginCentral->pluginMapping->getCreatePluginLineConfig(plugin->name(),innames.at(t));

		item = new QTableWidgetItem(tr("Input"));
		if (conf && conf->pIsUsed) {
			item->setIcon(QIcon(":/gfx/icons/ledgreen.png"));
		} else {
			item->setIcon(QIcon(":/gfx/icons/ledred.png"));
		}
		dmxMappingTable->setItem(row,0,item);

		QSpinBox *spin = new QSpinBox();
		spin->setRange(0,MAX_DMX_UNIVERSE);
		spin->setPrefix(tr("universe:"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",innames.at(t));
		spin->setProperty("tableRow",row);
		if (conf) spin->setValue(conf->pUniverse+1);
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_universe_changed(int)));
		dmxMappingTable->setCellWidget(row,1,spin);

		item = new QTableWidgetItem(innames.at(t));
		dmxMappingTable->setItem(row,2,item);

		spin = new QSpinBox();
		spin->setRange(0,5000);
		spin->setSuffix(tr("ms"));
		spin->setFrame(false);
		spin->setProperty("plugin",plugin->name());
		spin->setProperty("line",innames.at(t));
		if (conf) spin->setValue(conf->pResponseTime);
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(if_pluginline_responsetime_changed(int)));
		dmxMappingTable->setCellWidget(row,3,spin);

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
	if (AppCentral::instance()->pluginCentral->pluginMapping->saveToDefaultFile()) {
		QMessageBox::information(this,tr("DMX Configuration")
								 ,tr("DMX Plugin Configuration successfully saved!"));
		LOGTEXT(tr("DMX Plugin Configuration successfully saved!"));
	}
}

void SetupWidget::if_pluginline_universe_changed(int val)
{
	QString plugin_name = sender()->property("plugin").toString();
	QString plugin_line = sender()->property("line").toString();

	if (cur_plugin_map) {
		PluginConfig *lineconf = cur_plugin_map->getCreatePluginLineConfig(plugin_name,plugin_line);
		if (val > 0) {
			lineconf->pUniverse = val-1;
			lineconf->pIsUsed = true;
		} else {
			lineconf->pIsUsed = false;
		}
		update_plugin_mapping_f = true;


		int row = sender()->property("tableRow").toInt();
		QTableWidgetItem * item = dmxMappingTable->item(row,0);
		if (item) {
			if (lineconf->pIsUsed) {
				item->setIcon(QIcon(":/gfx/icons/ledgreen.png"));
			} else {
				item->setIcon(QIcon(":/gfx/icons/ledred.png"));
			}
		}
	}

}

void SetupWidget::if_pluginline_responsetime_changed(int val)
{
	QString plugin_name = sender()->property("plugin").toString();
	QString plugin_line = sender()->property("line").toString();

	if (cur_plugin_map) {
		PluginConfig *lineconf = cur_plugin_map->getCreatePluginLineConfig(plugin_name,plugin_line);
		lineconf->pResponseTime = val;
		update_plugin_mapping_f = true;
	}
}
