#include "dmxtypeselectorwidget.h"
#include "ui_dmxtypeselectorwidget.h"

#include "system/dmx/dmxhelp.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/colorsettings.h"

#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

DmxTypeSelectorWidget::DmxTypeSelectorWidget(DmxChannelType type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DmxTypeSelectorWidget),
	m_type(type),
	m_iType(type)
{
	ui->setupUi(this);
	guiInitTypes();

	connect(ui->firstButton, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

DmxTypeSelectorWidget::~DmxTypeSelectorWidget()
{
	delete ui;
}

void DmxTypeSelectorWidget::setScaler(int num, int denom)
{
	ui->numeratorEdit->setText(QString::number(num));
	ui->denominatorEdit->setText(QString::number(denom));
	m_iNum = num;
	m_iDen = denom;
}

bool DmxTypeSelectorWidget::getScaler(int &num, int &denom)
{
	bool modified = m_type != m_iType;

	int n = ui->numeratorEdit->text().toInt();
	int d = ui->denominatorEdit->text().toInt();
	if (n != 0 && d != 0) {
		num = n;
		denom = d;
		if (n != m_iNum || d != m_iDen)
			modified = true;
	}

	return modified;
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
			but->setStyleSheet("font-weight: bold; color: blue");
			lab->setStyleSheet(QString("font-weight: bold; color: %1").arg(AppCentral::ref().colorSettings->pKeyColumn));
			but->setFocus();
		}
		connect(but, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
		ui->buttonLayout->addWidget(lab, row, 0);
		ui->buttonLayout->addWidget(but, row, 1);

		row++;
	}
}

void DmxTypeSelectorWidget::keyPressEvent(QKeyEvent *ev)
{
	if (ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter) {
		ev->accept();
		m_type = m_iType;
		accept();
	} else {
		QDialog::keyPressEvent(ev);
	}
}

void DmxTypeSelectorWidget::onButtonClicked()
{
	QPushButton *but = qobject_cast<QPushButton*>(sender());
	if (but) {
		m_buttonClicked = true;
		m_type = DmxChannelType(but->property("type").toInt());
	}

	accept();
}
