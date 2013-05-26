/*
  Q Light Controller
  enttecdmxusbconfig.cpp

  Copyright (C) Heikki Junnila

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  Version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details. The license is
  in the file "COPYING".

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <QPushButton>
#include <QHeaderView>
#include <QTreeWidget>
#include <QSettings>
#include <QComboBox>
#include <QLayout>
#include <QDebug>
#include <QTimer>

#include "enttecdmxusbconfig.h"
#include "enttecdmxusbwidget.h"
#include "enttecdmxusb.h"

#define SETTINGS_GEOMETRY "enttecdmxusbconfig/geometry"

#define COL_NAME   0
#define COL_SERIAL 1
#define COL_TYPE   2
#define PROP_SERIAL "serial"

EnttecDMXUSBConfig::EnttecDMXUSBConfig(EnttecDMXUSB* plugin, QWidget* parent)
	: QDialog(parent)
	, m_plugin(plugin)
	, m_tree(new QTreeWidget(this))
	, m_refreshButton(new QPushButton(tr("Refresh"), this))
	, m_closeButton(new QPushButton(tr("Close"), this))
	, m_ignoreItemChanged(false)
{
	Q_ASSERT(plugin != NULL);

	setWindowTitle(plugin->name());

	QStringList header;
	header << tr("Name") << tr("Serial") << QString("Mode");
	m_tree->setHeaderLabels(header);
	m_tree->setSelectionMode(QAbstractItemView::NoSelection);
#ifdef IS_QT5
	m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
	m_tree->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

	QVBoxLayout* vbox = new QVBoxLayout(this);
	vbox->addWidget(m_tree);

	QHBoxLayout* hbox = new QHBoxLayout;
	hbox->addWidget(m_refreshButton);
	hbox->addStretch();
	hbox->addWidget(m_closeButton);
	vbox->addLayout(hbox);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(slotRefresh()));
	connect(m_closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	QSettings settings;
	QVariant var = settings.value(SETTINGS_GEOMETRY);
	if (var.isValid() == true)
		restoreGeometry(var.toByteArray());

	slotRefresh();
}

EnttecDMXUSBConfig::~EnttecDMXUSBConfig()
{
	QSettings settings;
	settings.setValue(SETTINGS_GEOMETRY, saveGeometry());
}

void EnttecDMXUSBConfig::slotTypeComboActivated(int index)
{
	QComboBox* combo = qobject_cast<QComboBox*> (QObject::sender());
	Q_ASSERT(combo != NULL);

	QVariant var = combo->property(PROP_SERIAL);
	if (var.isValid() == true)
	{
		EnttecDMXUSBWidget::Type type = (EnttecDMXUSBWidget::Type)
											combo->itemData(index).toInt();
		QMap <QString,QVariant> typeMap(QLCFTDI::typeMap());
		typeMap[var.toString()] = type;
		QLCFTDI::storeTypeMap(typeMap);
	}

	QTimer::singleShot(0, this, SLOT(slotRefresh()));
}

void EnttecDMXUSBConfig::slotRefresh()
{
	m_plugin->rescanWidgets();

	m_ignoreItemChanged = true;

	m_tree->clear();
	QListIterator <EnttecDMXUSBWidget*> it(m_plugin->widgets());
	while (it.hasNext() == true)
	{
		EnttecDMXUSBWidget* widget = it.next();
		QTreeWidgetItem* item = new QTreeWidgetItem(m_tree);
		item->setText(COL_NAME, widget->name());
		item->setText(COL_SERIAL, widget->serial());
		m_tree->setItemWidget(item, COL_TYPE, createTypeCombo(widget));
	}

	m_ignoreItemChanged = false;
}

QComboBox* EnttecDMXUSBConfig::createTypeCombo(EnttecDMXUSBWidget* widget)
{
	Q_ASSERT(widget != NULL);
	QComboBox* combo = new QComboBox;
	combo->setProperty(PROP_SERIAL, widget->serial());
	combo->addItem(QString("Pro TX"), EnttecDMXUSBWidget::ProTX);
	combo->addItem(QString("Open TX"), EnttecDMXUSBWidget::OpenTX);
	combo->addItem(QString("Pro RX"), EnttecDMXUSBWidget::ProRX);
	int index = combo->findData(widget->type());
	combo->setCurrentIndex(index);

	connect(combo, SIGNAL(activated(int)), this, SLOT(slotTypeComboActivated(int)));

	return combo;
}
