#ifndef FXITEMPROPERTYWIDGET_H
#define FXITEMPROPERTYWIDGET_H

#include "ui_fxitempropertywidget.h"

class FxItem;
class FxAudioItem;
class FxSceneItem;
class FxSeqItem;

class FxItemPropertyWidget : public QWidget, public Ui::FxItemPropertyWidget
{
	Q_OBJECT
private:
	FxItem *cur_fx;						///< A pointer to the actually set FxItem (of NULL if none)
	FxAudioItem *cur_fxa;				///< A convenience pointer to FxItem if it is an FxAudioItem (Type FX_AUDIO)
	FxSceneItem *cur_fxs;				///< A cnnvenience pointer to FxItem if it is an FxSceneItem (Type FX_SCENE)
	FxSeqItem *cur_fxseq;

	bool once_edit_f;

public:
	FxItemPropertyWidget(QWidget *parent = 0);
	bool isOnceEdit() {return once_edit_f;}

	static FxItemPropertyWidget *openFxPropertyEditor(FxItem *fx);

public slots:
	bool setFxItem(FxItem *fx);
	void setEditable(bool state, bool once = false);

private slots:
	void on_initialVolDial_sliderMoved(int position);
	void on_nameEdit_textEdited(const QString &arg1);
	void on_keyEdit_textEdited(const QString &arg1);
	void on_faderCountEdit_textEdited(const QString &arg1);
	void on_audioFilePathEdit_doubleClicked();
	void on_preDelayEdit_textEdited(const QString &arg1);
	void on_fadeInTimeEdit_textEdited(const QString &arg1);
	void on_holdTimeEdit_textEdited(const QString &arg1);
	void on_fadeOutTimeEdit_textEdited(const QString &arg1);
	void on_postDelayEdit_textEdited(const QString &arg1);
	void on_keyClearButton_clicked();
	void on_editOnceButton_clicked();
	void on_audioLoopsSpin_valueChanged(int arg1);
	void on_audioStartAtEdit_textEdited(const QString &arg1);
	void on_audioStopAtEdit_textEdited(const QString &arg1);
	void finish_edit();

	void on_hookedToUniverseSpin_valueChanged(int arg1);

	void on_hookedToChannelSpin_valueChanged(int arg1);

	void on_seqStopOtherCheck_clicked(bool checked);

	void on_seqBlackOtherCheck_clicked(bool checked);

	void on_audioOnStartCombo_activated(int index);

	void on_setToCurrentVolButton_clicked();

signals:
	void modified(FxItem *fx);
};

#endif // FXITEMPROPERTYWIDGET_H
