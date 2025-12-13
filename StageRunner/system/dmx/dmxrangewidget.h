#ifndef DMXRANGEWIDGET_H
#define DMXRANGEWIDGET_H

#include <QDialog>

namespace Ui {
class DmxRangeWidget;
}

class DmxRangeWidget : public QDialog
{
	Q_OBJECT
private:
	Ui::DmxRangeWidget *ui;

public:
	explicit DmxRangeWidget(int first = 1, int last = 1, int universe = 1, QWidget *parent = nullptr);
	~DmxRangeWidget();

	int first() const;
	int last() const;
	int universe() const;

private slots:
	void on_firstDmxChanSpin_valueChanged(int arg1);
	void on_lastDmxChanSpin_valueChanged(int arg1);
	void on_channelsSpin_valueChanged(int arg1);
	void on_lastDmxChanSpin_editingFinished();
};

#endif // DMXRANGEWIDGET_H
