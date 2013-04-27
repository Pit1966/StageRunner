#ifndef FXITEMPROPERTYWIDGET_H
#define FXITEMPROPERTYWIDGET_H

#include "ui_fxitempropertywidget.h"

class FxItem;
class FxAudioItem;

class FxItemPropertyWidget : public QWidget, public Ui::FxItemPropertyWidget
{
	Q_OBJECT
private:
	FxItem *cur_fx;						///< A pointer to the actually set FxItem (of NULL if none)
	FxAudioItem *cur_fxa;				///< A convenience pointer to FxItem if it is an FxAudioItem (Type FX_AUDIO)

public:
	FxItemPropertyWidget(QWidget *parent = 0);


public slots:
	bool setFxItem(FxItem *fx);

private slots:
	void on_initialVolDial_sliderMoved(int position);
	void on_nameEdit_textEdited(const QString &arg1);
	void on_keyEdit_textEdited(const QString &arg1);

signals:
	void modified();
};

#endif // FXITEMPROPERTYWIDGET_H
