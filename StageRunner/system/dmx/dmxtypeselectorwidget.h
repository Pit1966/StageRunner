#ifndef DMXTYPESELECTORWIDGET_H
#define DMXTYPESELECTORWIDGET_H

#include <QDialog>

#include "system/dmxtypes.h"

namespace Ui {
class DmxTypeSelectorWidget;
}

class DmxTypeSelectorWidget : public QDialog
{
	Q_OBJECT
private:
	Ui::DmxTypeSelectorWidget *ui;
	DmxChannelType m_type;

public:
	explicit DmxTypeSelectorWidget(DmxChannelType type = DmxChannelType::DMX_GENERIC, QWidget *parent = nullptr);
	~DmxTypeSelectorWidget();

	DmxChannelType selectedType() const {return m_type;}
	void setScaler(int num, int denom);
	bool getScaler(int &num, int &denom);

private:
	void guiInitTypes();

private slots:
	void onButtonClicked();

};

#endif // DMXTYPESELECTORWIDGET_H
