//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef FXITEMPROPERTYWIDGET_H
#define FXITEMPROPERTYWIDGET_H

#include "ui_fxitempropertywidget.h"

class FxItem;
class FxAudioItem;
class FxSceneItem;
class FxSeqItem;
class FxClipItem;

class FxItemPropertyWidget : public QWidget, public Ui::FxItemPropertyWidget
{
	Q_OBJECT
private:
	FxItem *cur_fx;						///< A pointer to the actually set FxItem (of NULL if none)
	FxAudioItem *cur_fxa;				///< A convenience pointer to FxItem if it is an FxAudioItem (Type FX_AUDIO)
	FxSceneItem *cur_fxs;				///< A convenience pointer to FxItem if it is an FxSceneItem (Type FX_SCENE)
	FxSeqItem *cur_fxseq;
	FxClipItem *cur_fxclip;				///< A convenience pointer to FxItem if it is an FxClipItem (Type FX_CLIP)

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
	void on_videoInitialVolDial_sliderMoved(int position);
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
	void on_audioSlotSpin_valueChanged(int arg1);
	void on_audioStartAtEdit_textEdited(const QString &arg1);
	void on_audioStopAtEdit_textEdited(const QString &arg1);
	void on_audioOnStartCombo_activated(int index);
	void on_audioOnStopCombo_activated(int index);
	void on_audioOnStartEdit_textEdited(const QString &arg1);
	void on_audioOnStopEdit_textEdited(const QString &arg1);
	void finish_edit();

	void on_hookedToUniverseSpin_valueChanged(int arg1);
	void on_hookedToChannelSpin_valueChanged(int arg1);
	void on_seqStopOtherCheck_clicked(bool checked);
	void on_seqBlackOtherCheck_clicked(bool checked);
	void on_setToCurrentVolButton_clicked();

	void on_videoLoopsSpin_valueChanged(int arg1);
	void on_videoBlackAtEndCheck_clicked(bool checked);
	void on_videoFilePathEdit_doubleClicked();

	void on_findAudioFileButton_clicked();


signals:
	void modified(FxItem *fx);
};

#endif // FXITEMPROPERTYWIDGET_H
