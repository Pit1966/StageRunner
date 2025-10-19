#include "dmxtypeselectorwidget.h"
#include "ui_dmxtypeselectorwidget.h"

#include "dmx/dmxhelp.h"

#include <QLabel>
#include <QPushButton>

DmxTypeSelectorWidget::DmxTypeSelectorWidget(DmxChannelType type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DmxTypeSelectorWidget),
	m_type(type)
{
	ui->setupUi(this);
	guiInitTypes();

	connect(ui->firstButton, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

DmxTypeSelectorWidget::~DmxTypeSelectorWidget()
{
	delete ui;
}

void DmxTypeSelectorWidget::guiInitTypes()
{
	int row = 1;
	for (int i=1; i<DmxChannelType::DMX_TYPES; i++) {
		QString typeLong = DMXHelp::dmxTypeToString(DmxChannelType(i));
		QString typeShort = DMXHelp::dmxTypeToShortString(DmxChannelType(i));
		if (typeShort.startsWith('#'))
			continue;

		QLabel *lab = new QLabel(typeLong);
		QPushButton *but = new QPushButton(typeShort);
		but->setProperty("type", i);
		if (i == m_type) {
			lab->setStyleSheet("font-weight: bold");
			but->setStyleSheet("font-weight: bold");
		}
		connect(but, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
		ui->buttonLayout->addWidget(lab, row, 0);
		ui->buttonLayout->addWidget(but, row, 1);

		row++;
	}
}

void DmxTypeSelectorWidget::onButtonClicked()
{
	QPushButton *but = qobject_cast<QPushButton*>(sender());
	if (but) {
		m_type = DmxChannelType(but->property("type").toInt());
	}

	accept();
}
