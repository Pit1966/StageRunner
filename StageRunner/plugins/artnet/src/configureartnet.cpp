/*
  Q Light Controller Plus
  configureartnet.cpp

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

	  http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QSpacerItem>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>

#include "configureartnet.h"
#include "artnetplugin.h"

#define KNodesColumnIP          0
#define KNodesColumnShortName   1
#define KNodesColumnLongName    2

#define KMapColumnInterface     0
#define KMapColumnUniverse      1
#define KMapColumnIPAddress     2
#define KMapColumnArtNetUni     3
#define KMapColumnTransmitMode  4
#define KMapColumnAddIP			5
#define KMapColumnAddOutput		5

#define PROP_UNIVERSE (Qt::UserRole + 0)
#define PROP_LINE (Qt::UserRole + 1)
#define PROP_TYPE (Qt::UserRole + 2)
#define PROP_OUT_NUM (Qt::UserRole + 3)

// ArtNet universe is a 15bit value
#define ARTNET_UNIVERSE_MAX 0x7fff

/*****************************************************************************
 * Initialization
 *****************************************************************************/

ConfigureArtNet::ConfigureArtNet(ArtNetPlugin* plugin, QWidget* parent)
	: QDialog(parent)
{
	Q_ASSERT(plugin != NULL);
	m_plugin = plugin;

	/* Setup UI controls */
	setupUi(this);

	fillNodesTree();
	fillMappingTree();
}


void ConfigureArtNet::fillNodesTree()
{
	ArtNetController *prevController = NULL;

	QList<ArtNetIO> IOmap = m_plugin->getIOMapping();

	for (int i = 0; i < IOmap.length(); i++)
	{
		ArtNetController *controller = IOmap.at(i).controller;

		if (controller != NULL && controller != prevController)
		{
			QTreeWidgetItem* pitem = new QTreeWidgetItem(m_nodesTree);
			pitem->setText(KNodesColumnIP, tr("%1 nodes").arg(controller->getNetworkIP()));
			QHash<QHostAddress, ArtNetNodeInfo> nodesList = controller->getNodesList();
			QHashIterator<QHostAddress, ArtNetNodeInfo> it(nodesList);
			while (it.hasNext())
			{
				it.next();
				QTreeWidgetItem* nitem = new QTreeWidgetItem(pitem);
				ArtNetNodeInfo nInfo = it.value();
				QString ip = it.key().toString();
				if (ip.startsWith("::ffff:"))
					ip.remove("::ffff:");
				nitem->setText(KNodesColumnIP,ip);
				nitem->setText(KNodesColumnShortName, nInfo.shortName);
				nitem->setText(KNodesColumnLongName, nInfo.longName);
			}
			prevController = controller;
		}
	}

	m_nodesTree->header()->resizeSections(QHeaderView::ResizeToContents);
}

void ConfigureArtNet::fillMappingTree()
{
	QTreeWidgetItem* inputItem = NULL;
	QTreeWidgetItem* outputItem = NULL;

	QList<ArtNetIO> IOmap = m_plugin->getIOMapping();
	foreach(ArtNetIO io, IOmap)
	{
		if (io.controller == NULL)
			continue;

		ArtNetController *controller = io.controller;
		if (controller == NULL)
			continue;

		qDebug() << "[ArtNet] controller IP" << controller->getNetworkIP() << "type:" << controller->type();
		if ((controller->type() & ArtNetController::Input) && inputItem == NULL)
		{
			inputItem = new QTreeWidgetItem(m_uniMapTree);
			inputItem->setText(KMapColumnInterface, tr("Inputs"));
			inputItem->setExpanded(true);
		}
		if ((controller->type() & ArtNetController::Output) && outputItem == NULL)
		{
			outputItem = new QTreeWidgetItem(m_uniMapTree);
			outputItem->setText(KMapColumnInterface, tr("Outputs"));
			outputItem->setExpanded(true);

			// QPushButton *addbut = new QPushButton("Add output");
			// connect(addbut, SIGNAL(clicked()), this, SLOT(onAddOutputButtonClicked()));
			// m_uniMapTree->setItemWidget(outputItem, KMapColumnAddOutput, addbut);
		}
		foreach(quint32 universe, controller->universesList())
		{
			UniverseInfo *info = controller->getUniverseInfo(universe);

			if (info->type & ArtNetController::Input)
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(inputItem);
				item->setData(KMapColumnInterface, PROP_UNIVERSE, universe);
				item->setData(KMapColumnInterface, PROP_LINE, controller->line());
				item->setData(KMapColumnInterface, PROP_TYPE, ArtNetController::Input);

				item->setText(KMapColumnInterface, io.address.broadcast().toString());
				item->setText(KMapColumnUniverse, QString::number(universe + 1));
				item->setTextAlignment(KMapColumnUniverse, Qt::AlignHCenter | Qt::AlignVCenter);

				QSpinBox *spin = new QSpinBox(this);
				spin->setRange(0, ARTNET_UNIVERSE_MAX);
				spin->setValue(info->inputUniverse);
				m_uniMapTree->setItemWidget(item, KMapColumnArtNetUni, spin);
			}
			if (info->type & ArtNetController::Output)
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(outputItem);
				item->setData(KMapColumnInterface, PROP_UNIVERSE, universe);
				item->setData(KMapColumnInterface, PROP_LINE, controller->line());
				item->setData(KMapColumnInterface, PROP_TYPE, ArtNetController::Output);
				item->setData(KMapColumnInterface, PROP_OUT_NUM, 0);

				item->setText(KMapColumnInterface, controller->getNetworkIP());
				item->setText(KMapColumnUniverse, QString::number(universe + 1));
				item->setTextAlignment(KMapColumnUniverse, Qt::AlignHCenter | Qt::AlignVCenter);

				QHostAddress outputAddress = info->outputAddresses.size() ? info->outputAddresses.at(0) : QHostAddress::LocalHost;
				if (outputAddress == QHostAddress::LocalHost)
				{
					// localhost (127.0.0.1) do not need broadcast or anything else
					item->setText(KMapColumnIPAddress, outputAddress.toString());
				}
				else
				{
					QWidget *IPwidget = new QLineEdit(outputAddress.toString());
					m_uniMapTree->setItemWidget(item, KMapColumnIPAddress, IPwidget);
				}

				QSpinBox *spin = new QSpinBox(this);
				spin->setRange(0, ARTNET_UNIVERSE_MAX);
				spin->setValue(info->outputUniverse);
				m_uniMapTree->setItemWidget(item, KMapColumnArtNetUni, spin);

				QComboBox *combo = new QComboBox(this);
				combo->addItem(tr("Full"));
				combo->addItem(tr("Partial"));
				if (info->outputTransmissionMode == ArtNetController::Partial)
					combo->setCurrentIndex(1);
				m_uniMapTree->setItemWidget(item, KMapColumnTransmitMode, combo);

				//item->setText(KMapColumnAddOutput, QString("Add target IP"));
				QPushButton *addbut = new QPushButton("Add target IP");
				QVariant var;
				var.setValue(info);
				addbut->setProperty("universe_info", var);
				connect(addbut, SIGNAL(clicked()), this, SLOT(onAddTargetIPButtonClicked()));
				m_uniMapTree->setItemWidget(item, KMapColumnAddIP, addbut);
				addbut->setStyle(nullptr);

				// add additional lines, if there are more target host addresses
				for (int i=1; i<info->outputAddresses.size(); i++) {
					QTreeWidgetItem *subitem = new QTreeWidgetItem(outputItem);
					subitem->setData(KMapColumnInterface, PROP_UNIVERSE, universe);
					subitem->setData(KMapColumnInterface, PROP_LINE, controller->line());
					subitem->setData(KMapColumnInterface, PROP_TYPE, ArtNetController::Output);
					subitem->setData(KMapColumnInterface, PROP_OUT_NUM, i);

					QHostAddress addAddress = info->outputAddresses.at(i);
					QWidget *edit = new QLineEdit(addAddress.toString());
					m_uniMapTree->setItemWidget(subitem, KMapColumnIPAddress, edit);

					QPushButton *rembut = new QPushButton("Remove output");
					QVariant var;
					var.setValue(info);
					rembut->setProperty("universe_info", var);
					rembut->setProperty("output_num", i);
					connect(rembut, SIGNAL(clicked()), this, SLOT(onRemoveTargetIPButtonClicked()));
					m_uniMapTree->setItemWidget(subitem, KMapColumnAddIP, rembut);
				}

			}
		}
	}

	m_uniMapTree->header()->resizeSections(QHeaderView::ResizeToContents);
}

void ConfigureArtNet::showIPAlert(QString ip)
{
	QMessageBox::critical(this, tr("Invalid IP"), tr("%1 is not a valid IP.\nPlease fix it before confirming.").arg(ip));
}

void ConfigureArtNet::onAddTargetIPButtonClicked()
{
	QPushButton *addbut = qobject_cast<QPushButton *>(sender());
	if (!addbut) return;

	QVariant var = addbut->property("universe_info");
	if (!var.isValid()) return;

	UniverseInfo *info = var.value<UniverseInfo*>();

	qDebug() << "info addresses" << info->outputAddresses;

	QString str = QInputDialog::getText(this, tr("Universe config"), tr("Enter artnet node IP4 address"));
	if (str.isEmpty()) return;

	QHostAddress addr = QHostAddress(str);
	info->outputAddresses.append(addr);
	m_uniMapTree->clear();
	fillMappingTree();
}

void ConfigureArtNet::onRemoveTargetIPButtonClicked()
{
	QPushButton *rembut = qobject_cast<QPushButton *>(sender());
	if (!rembut) return;

	QVariant var = rembut->property("universe_info");
	if (!var.isValid()) return;

	UniverseInfo *info = var.value<UniverseInfo*>();

	int outNum = rembut->property("output_num").toInt();
	if (outNum > 0) {
		info->outputAddresses.removeAt(outNum);
		m_uniMapTree->clear();
		fillMappingTree();
	}
}

void ConfigureArtNet::onAddOutputButtonClicked()
{


}

ConfigureArtNet::~ConfigureArtNet()
{
}

/*****************************************************************************
 * Dialog actions
 *****************************************************************************/

void ConfigureArtNet::accept()
{
	for(int i = 0; i < m_uniMapTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *topItem = m_uniMapTree->topLevelItem(i);
		for(int c = 0; c < topItem->childCount(); c++)
		{
			QTreeWidgetItem *item = topItem->child(c);
			if (item->data(KMapColumnInterface, PROP_UNIVERSE).isValid() == false)
				continue;

			quint32 universe = item->data(KMapColumnInterface, PROP_UNIVERSE).toUInt();
			quint32 line = item->data(KMapColumnInterface, PROP_LINE).toUInt();
			int outNum = item->data(KMapColumnInterface, PROP_OUT_NUM).toInt();

			ArtNetController::Type type = ArtNetController::Type(item->data(KMapColumnInterface, PROP_TYPE).toInt());
			QLCIOPlugin::Capability cap = QLCIOPlugin::Input;
			if (type == ArtNetController::Output)
				cap = QLCIOPlugin::Output;


			QHostAddress hostAddress;
			QLineEdit *ipEdit = qobject_cast<QLineEdit*>(m_uniMapTree->itemWidget(item, KMapColumnIPAddress));
			if (ipEdit != nullptr) {
				Q_ASSERT(cap == QLCIOPlugin::Output);
				hostAddress = QHostAddress(ipEdit->text());
				if (hostAddress.isNull() && ipEdit->text().size() > 0) {
					showIPAlert(ipEdit->text());
					return;
				}

				//qDebug() << "IPchanged = " << IPChanged;
			}

			if (outNum <= 0) {
				m_plugin->setParameter(universe, line, cap, ARTNET_OUTPUTIP, hostAddress.toString());
			}
			else {	// outNum > 0
				// this is additional configuration for plugin in last line. But, only available for output!
				if (cap == QLCIOPlugin::Output) {
					m_plugin->setParameter(universe, line, cap, ARTNET_OUTPUT_ADD_IP + QString::number(outNum), hostAddress.toString());
				}
				continue;
			}


			QSpinBox *spin = qobject_cast<QSpinBox*>(m_uniMapTree->itemWidget(item, KMapColumnArtNetUni));
			Q_ASSERT(spin != NULL);

			m_plugin->setParameter(universe, line, cap, (cap == QLCIOPlugin::Output ? ARTNET_OUTPUTUNI : ARTNET_INPUTUNI), spin->value());

			QComboBox *combo = qobject_cast<QComboBox*>(m_uniMapTree->itemWidget(item, KMapColumnTransmitMode));
			if (combo != NULL)
			{
				ArtNetController::TransmissionMode transmissionMode;
				if (combo->currentIndex() == 0)
					transmissionMode = ArtNetController::Full;
				else
					transmissionMode = ArtNetController::Partial;

				m_plugin->setParameter(universe, line, cap, ARTNET_TRANSMITMODE,
									   ArtNetController::transmissionModeToString(transmissionMode));
			}
		}
	}

	QDialog::accept();
}

int ConfigureArtNet::exec()
{
	return QDialog::exec();
}
