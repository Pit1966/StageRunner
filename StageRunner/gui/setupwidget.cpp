//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "setupwidget.h"
#include "config.h"
#include "configrev.h"
#include "system/log.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/ioplugincentral.h"
#include "appcontrol/project.h"
#include "plugins/interfaces/qlcioplugin.h"
#include "appcontrol/usersettings.h"
#include "appcontrol/pluginmapping.h"
#include "appcontrol/fxlistvarset.h"
#include "fx/fxlist.h"
#include "fx/fxsceneitem.h"
#include "appcontrol//audiocontrol.h"
#include "system/variantmapserializer.h"

#include <QtWidgets>
#include <QSettings>

SetupWidget::SetupWidget(AppCentral *app_central, QWidget *parent)
	: QDialog(parent)
	, myapp(app_central)
{
	init();

	setupUi(this);
	initGui();

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("Plugins");

	myapp->pluginCentral->updatePluginMappingInformation();
	m_curPluginMap = myapp->pluginCentral->pluginMapping;

	QList<QLCIOPlugin*>qlcplugins = myapp->pluginCentral->qlcPlugins();
	for (int t=0; t<qlcplugins.size(); t++) {
		QLCIOPlugin *plugin = qlcplugins.at(t);
		QListWidgetItem *item = new QListWidgetItem(plugin->name());
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		if (set.value(plugin->name(), true).toBool() == false) {
			item->setCheckState(Qt::Unchecked);
		} else {
			item->setCheckState(Qt::Checked);
		}
		qlcPluginsList->addItem(item);
	}

	configurePluginButton->setDisabled(true);
	copy_settings_to_gui();
}

void SetupWidget::init()
{
	m_selectedPlugin = 0;
	m_curPluginMap = 0;
	m_updatePluginMapFlag = false;
	m_restartAudioSlotsOnExit = false;
}

void SetupWidget::initGui()
{
	QHBoxLayout *lay1 = new QHBoxLayout;
	QHBoxLayout *lay2 = new QHBoxLayout;

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QCheckBox *check1 = new QCheckBox;
		check1->setObjectName(QString("EnableVolumeDial%1").arg(t));
		check1->setText(tr("Channel %1").arg(t+1));
		lay1->addWidget(check1);

		QCheckBox *check2 = new QCheckBox;
		check2->setObjectName(QString("EnableFFT%1").arg(t));
		check2->setText(tr("Channel %1").arg(t+1));
		lay2->addWidget(check2);
	}
	showVolumeDialGroup->setLayout(lay1);
	enableFFTGroup->setLayout(lay2);
}

void SetupWidget::copy_settings_to_gui()
{
	UserSettings *set = myapp->userSettings;

	audioBufferSizeEdit->setText(QString::number(set->pAudioBufferSize));
	defaultAudioFadeOutMsSpin->setValue(set->pDefaultAudioFadeoutTimeMs);

	// Styles
	int idx;
	appStyleCombo->addItem("LightDesk");
	appStyleCombo->addItem("Default");
	appStyleCombo->addItems(QStyleFactory::keys());
	idx = appStyleCombo->findText(set->pApplicationGuiStyle);
	if (idx >= 0) appStyleCombo->setCurrentIndex(idx);
	guiIsDarkCheck->setChecked(bool(set->pIsDarkGuiTheme));

	dialKnobStyleCombo->addItem("Default");
	dialKnobStyleCombo->addItem("QSynth Dial Classic");
	dialKnobStyleCombo->addItem("QSynth Dial Peppino");
	idx = dialKnobStyleCombo->findText(set->pDialKnobStyle);
	if (idx >= 0)
		dialKnobStyleCombo->setCurrentIndex(idx);

	noInterfaceFeedbackCheck->setChecked(set->pNoInterfaceDmxFeedback);
	prohibitAudioDoubleStartCheck->setChecked(set->pProhibitAudioDoubleStart);
	useLogVolumeDefaultCheck->setChecked(set->pLogarithmicVolDials);
	useLogVolumeProjectCheck->setChecked(myapp->project->pLogarithmicVol);
	audioInTaskCheck->setChecked(set->pIsAudioInThread);
	smallAudioBufWorkAroundCheck->setChecked(set->pIsSmallAudioBufferFix);
	reactivateAudioTimeSpin->setValue(set->pAudioAllowReactivateTime);

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QSpinBox *spin_univ = this->findChild<QSpinBox*>(QString("mapAudioToUniv%1").arg(t+1));
		QSpinBox *spin_chan = this->findChild<QSpinBox*>(QString("mapAudioToDmx%1").arg(t+1));
		if (spin_univ) {
			spin_univ->setValue(set->mapAudioToDmxUniv[t]);
		}
		if (spin_chan) {
			spin_chan->setValue(set->mapAudioToDmxChan[t]);
		}
	}

	playlistChannelSpin->setValue(qint32(set->pAudioPlayListChannel)+1);

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QCheckBox *check_volumedial = findChild<QCheckBox*>(QString("EnableVolumeDial%1").arg(t));
		check_volumedial->setChecked(set->pVolumeDialMask & (1<<t));
		QCheckBox *check_fftenable = findChild<QCheckBox*>(QString("EnableFFT%1").arg(t));
		check_fftenable->setChecked(set->pFFTAudioMask & (1<<t));
	}


	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QComboBox *combo = findChild<QComboBox*>(QString("slotDeviceCombo%1").arg(t+1));
		if (!combo) continue;
		combo->clear();
		combo->addItem("system default");

		foreach (QString devname, myapp->unitAudio->audioDeviceNames()) {
			combo->addItem(devname);
			// set current configuration
			if (devname == set->pSlotAudioDevice[t])
				combo->setCurrentIndex(combo->count()-1);
		}
	}

	// Video Tab
	videoXSizeEdit->setText(QString::number(set->pVideoOutXSize));
	videoYSizeEdit->setText(QString::number(set->pVideoOutYSize));
	videoForceSecondCheck->setChecked(set->pVideoForceSecondDesktop);
}

void SetupWidget::copy_gui_to_settings()
{
	UserSettings *set = myapp->userSettings;
	set->pAudioBufferSize = audioBufferSizeEdit->text().toInt();
	set->pDefaultAudioFadeoutTimeMs = defaultAudioFadeOutMsSpin->value();

	set->pApplicationGuiStyle = appStyleCombo->currentText();
	set->pDialKnobStyle = dialKnobStyleCombo->currentText();
	set->pIsDarkGuiTheme = guiIsDarkCheck->isChecked();

	set->pNoInterfaceDmxFeedback = noInterfaceFeedbackCheck->isChecked();
	set->pProhibitAudioDoubleStart = prohibitAudioDoubleStartCheck->isChecked();
	set->pLogarithmicVolDials = useLogVolumeDefaultCheck->isChecked();
	set->pIsAudioInThread = audioInTaskCheck->isChecked();
	set->pIsSmallAudioBufferFix = smallAudioBufWorkAroundCheck->isChecked();
	myapp->project->pLogarithmicVol = useLogVolumeProjectCheck->isChecked();
	set->pAudioAllowReactivateTime = reactivateAudioTimeSpin->value();

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QSpinBox *spin_univ = this->findChild<QSpinBox*>(QString("mapAudioToUniv%1").arg(t+1));
		QSpinBox *spin_chan = this->findChild<QSpinBox*>(QString("mapAudioToDmx%1").arg(t+1));
		if (spin_univ) {
			set->mapAudioToDmxUniv[t] = spin_univ->value();
		}
		if (spin_chan) {
			set->mapAudioToDmxChan[t] = spin_chan->value();
		}
	}
	set->pAudioPlayListChannel = playlistChannelSpin->value()-1;

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QCheckBox *check_volumedial = findChild<QCheckBox*>(QString("EnableVolumeDial%1").arg(t));
		quint32 mask1 = set->pVolumeDialMask;
		if (check_volumedial->isChecked()) {
			mask1 |= 1<<t;
		} else {
			mask1 &= ~(1<<t);
		}
		set->pVolumeDialMask = mask1;

		QCheckBox *check_fftenable = findChild<QCheckBox*>(QString("EnableFFT%1").arg(t));
		quint32 mask2 = set->pFFTAudioMask;
		if (check_fftenable->isChecked()) {
			mask2 |= 1<<t;
		} else {
			mask2 &= ~(1<<t);
		}
		set->pFFTAudioMask = mask2;
	}

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		QComboBox *combo = findChild<QComboBox*>(QString("slotDeviceCombo%1").arg(t+1));
		if (!combo) continue;
		if (combo->currentIndex() > 0) {
			set->pSlotAudioDevice[t] = combo->currentText();
		} else {
			set->pSlotAudioDevice[t] = QString();
		}

	}

	// Video Tab
	set->pVideoOutXSize = videoXSizeEdit->text().toInt();
	set->pVideoOutYSize = videoYSizeEdit->text().toInt();
	set->pVideoForceSecondDesktop = videoForceSecondCheck->isChecked();

	// Set options in order to avoid necessary restart of application
	AppCentral::ref().unitAudio->setSmallAudioBufFix(set->pIsSmallAudioBufferFix);
}

void SetupWidget::on_okButton_clicked()
{
	copy_gui_to_settings();
	if (m_updatePluginMapFlag) {
		myapp->pluginCentral->updatePluginMappingInformation();
	}
	emit setupChanged(myapp->userSettings);

	AppCentral::ref().unitAudio->setAudioInThreadEnabled(myapp->userSettings->pIsAudioInThread);
	if (m_restartAudioSlotsOnExit)
		AppCentral::ref().unitAudio->reCreateMediaPlayerInstances();

	accept();
}

void SetupWidget::on_cancelButton_clicked()
{
	reject();
}

void SetupWidget::on_qlcPluginsList_itemActivated(QListWidgetItem *item)
{
	m_selectedPlugin = myapp->pluginCentral->getQLCPluginByName(item->text());

	update_gui_plugin_info();
}

void SetupWidget::update_gui_plugin_info()
{
	if (m_selectedPlugin) {
		if (m_selectedPlugin->property("isDisabled").toBool()) {
			qlcPluginEdit->setHtml("Plugin is disabled");
			return;
		}

		QString info;
		QStringList outputs = m_selectedPlugin->outputs();
        if (outputs.size())
            info += tr("<font color=darkOrange>Available outputs</font>");
		for (int t=0; t<outputs.size(); t++) {
			info += m_selectedPlugin->outputInfo(t);
		}

		QStringList inputs = m_selectedPlugin->inputs();
        if (inputs.size())
            info += tr("<br><font color=darkOrange>Available inputs</font>");
        for (int t=0; t<inputs.size(); t++) {
			info += m_selectedPlugin->inputInfo(t);
		}

		update_dmx_mapping_table(m_selectedPlugin);
		qlcPluginEdit->setHtml(info);
		configurePluginButton->setEnabled(true);
	} else {
		qlcPluginEdit->clear();
		configurePluginButton->setEnabled(false);
	}
}

void SetupWidget::update_dmx_mapping_table(QLCIOPlugin *plugin)
{
	dmxMappingTable->clearContents();
	if (!plugin) return;

	QStringList outnames = IOPluginCentral::outputsOf(plugin);
	QStringList innames = IOPluginCentral::inputsOf(plugin);
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
		if (m_curPluginMap)
			conf = m_curPluginMap->getCreatePluginLineConfig(plugin->name(),outnames.at(t));

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
		if (conf)
			spin->setValue(int(conf->pUniverse));
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onPluginLineUniverseChanged(int)));
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
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onPluginLineResponseTimeChanged(int)));
		dmxMappingTable->setCellWidget(row,3,spin);

		row++;
	}

	for (int t=0; t<innames.size(); t++) {
		QTableWidgetItem *item;
		PluginConfig *conf = 0;
		if (m_curPluginMap)
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
		if (conf)
			spin->setValue(int(conf->pUniverse));
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onPluginLineUniverseChanged(int)));
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
		connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onPluginLineResponseTimeChanged(int)));
		dmxMappingTable->setCellWidget(row,3,spin);

		row++;
	}
}


void SetupWidget::on_configurePluginButton_clicked()
{
	if (m_selectedPlugin) {
		m_selectedPlugin->configure();
		update_gui_plugin_info();

		QString plugin_name = m_selectedPlugin->name();
		for (int line=0; line<2; line++) {
			for (int uni=0; uni<MAX_DMX_UNIVERSE; uni++) {
				QString line_name = IOPluginCentral::outputOf(line, m_selectedPlugin);
				PluginConfig *lineconf = m_curPluginMap->getCreatePluginLineConfig(plugin_name, line_name);
				if (!lineconf)
					continue;
				QVariantMap map = m_selectedPlugin->getParameters(uni, line, QLCIOPlugin::Output);
				if (lineconf->pUniverse == uni+1) {
					QString ser = VariantMapSerializer::toString(map);
					lineconf->pParameters = ser;
				}
			}
		}
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

void SetupWidget::onPluginLineUniverseChanged(int val)
{
	QString plugin_name = sender()->property("plugin").toString();
	QString plugin_line = sender()->property("line").toString();

	if (m_curPluginMap) {
		PluginConfig *lineconf = m_curPluginMap->getCreatePluginLineConfig(plugin_name,plugin_line);
		if (val > 0) {
			lineconf->pUniverse = val;
			lineconf->pIsUsed = true;
		} else {
			lineconf->pIsUsed = false;
		}
		m_updatePluginMapFlag = true;


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

void SetupWidget::onPluginLineResponseTimeChanged(int val)
{
	QString plugin_name = sender()->property("plugin").toString();
	QString plugin_line = sender()->property("line").toString();

	if (m_curPluginMap) {
		PluginConfig *lineconf = m_curPluginMap->getCreatePluginLineConfig(plugin_name,plugin_line);
		lineconf->pResponseTime = val;
		m_updatePluginMapFlag = true;
	}
}

void SetupWidget::on_appStyleCombo_currentIndexChanged(int idx)
{
	if (idx >= 0)
		emit applicationStyleChanged(appStyleCombo->currentText());
}

void SetupWidget::on_dialKnobStyleCombo_currentIndexChanged(int idx)
{
	if (idx >= 0)
		emit dialKnobStyleChanged(dialKnobStyleCombo->currentText());
}

void SetupWidget::on_addDmxUniverseToTemplateButton_clicked()
{
	int channels = QInputDialog::getInt(this
										, tr("System question!")
										, tr("How many channels should be used?")
										, 64, 24, 512);


	FxItem *fxc = 0;

	FxList *fxlist = AppCentral::ref().templateFxList->fxList();
	fxlist->addFxScene(channels,&fxc);
	if (fxc) {
		fxc->setName(tr("Universe%1_%2Ch").arg(1).arg(channels));
		fxc->generateNewID(11000);
	}
	fxlist->emitListChangedSignal();
}

void SetupWidget::on_restartAudioVideoSlots_clicked()
{
	m_restartAudioSlotsOnExit = true;
	restartAudioVideoSlots->setText("Audio will be restarted on setup exit");
}

void SetupWidget::on_updateLinesButton_clicked()
{
	myapp->reOpenPlugins();
}

void SetupWidget::on_qlcPluginsList_itemChanged(QListWidgetItem *item)
{
	// DEBUGTEXT("item changed %s\n", item->text().toLocal8Bit().constData());

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("Plugins");
	if (item->checkState() == Qt::Unchecked) {
		set.setValue(item->text(), false);
	} else {
		set.setValue(item->text(), true);
	}
}

