#ifndef DEVICEINFOWIDGET_H
#define DEVICEINFOWIDGET_H

#include "ui_deviceinfowidget.h"

class SR_Fixture;

class DeviceInfoWidget : public QWidget, private Ui::DeviceInfoWidget
{
	Q_OBJECT
private:
	SR_Fixture *m_fixture = nullptr;
	int m_modeNum = 0;
	bool m_isValid = false;

public:
	explicit DeviceInfoWidget(QWidget *parent = nullptr);
	~DeviceInfoWidget();

	bool setDeviceDefinition(SR_Fixture *fix, int mode = 0);
	bool setDeviceDefinition(const QString &path, int mode = 0);
	int modeNumber() const {return m_modeNum;}
	bool isValid() const {return m_isValid;}
	SR_Fixture * device();

private slots:
	void on_modeCombo_activated(int index);
};

#endif // DEVICEINFOWIDGET_H
