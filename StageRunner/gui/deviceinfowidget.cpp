#include "deviceinfowidget.h"
#include "system/dmx/fixture.h"

DeviceInfoWidget::DeviceInfoWidget(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
}

DeviceInfoWidget::~DeviceInfoWidget()
{
	delete m_fixture;
}

bool DeviceInfoWidget::setDeviceDefinition(SR_Fixture *fix, int mode)
{
	if (m_fixture != fix) {
		delete m_fixture;
		m_fixture = fix;
	}

	// device name
	deviceNameLabel->setText(fix->modelName());
	deviceNameLabel->setStyleSheet("color: #00ff00");

	// modes
	modeCombo->clear();
	for (int t=0; t<fix->modeCount(); t++) {
		SR_Mode *srmode = fix->mode(t);
		modeCombo->addItem(srmode->name());
	}

	channelsList->clear();
	if (mode >= fix->modeCount())
		return false;

	modeCombo->setCurrentIndex(mode);
	QStringList chtexts = fix->getChannelAndPresetTexts(mode);

	for (auto s : chtexts) {
		channelsList->addItem(s);
	}

	m_modeNum = mode;
	m_isValid = true;
	return true;
}

bool DeviceInfoWidget::setDeviceDefinition(const QString &path, int mode)
{
	SR_Fixture *fix = new SR_Fixture();

	if (!fix->loadQLCFixture(path)) {
		delete fix;
		return false;
	}

	return setDeviceDefinition(fix, mode);
}

SR_Fixture *DeviceInfoWidget::device()
{
	return m_fixture;
}

void DeviceInfoWidget::on_modeCombo_activated(int index)
{
	setDeviceDefinition(m_fixture, index);
}
