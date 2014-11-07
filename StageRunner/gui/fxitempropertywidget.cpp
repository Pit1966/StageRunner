#include "fxitempropertywidget.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxseqitem.h"
#include "usersettings.h"
#include "appcentral.h"
#include "qtstatictools.h"

#include <QFileDialog>
#include <QFileInfo>

FxItemPropertyWidget::FxItemPropertyWidget(QWidget *parent) :
	QWidget(parent)
{
	cur_fx = 0;
	cur_fxa = 0;
	cur_fxs = 0;

	once_edit_f = false;

	setupUi(this);
	setWindowTitle("Fx Editor");
	setObjectName("DockFxEditor");

	audioGroup->setVisible(false);
	sceneGroup->setVisible(false);

	keyEdit->setSingleKeyEditEnabled(true);

	connect(nameEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(keyEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(faderCountEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(audioFilePathEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(fadeInTimeEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(fadeOutTimeEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(preDelayEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(postDelayEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(holdTimeEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(audioStartAtEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));
	connect(audioStopAtEdit,SIGNAL(editingFinished()),this,SLOT(finish_edit()));

}

FxItemPropertyWidget *FxItemPropertyWidget::openFxPropertyEditor(FxItem *fx)
{
	if (!FxItem::exists(fx)) return 0;

	FxItemPropertyWidget *itemEditor = new FxItemPropertyWidget();
	itemEditor->setFxItem(fx);
	itemEditor->setAttribute(Qt::WA_DeleteOnClose);
	itemEditor->editOnceButton->hide();
	itemEditor->show();

	itemEditor->setEditable(true);
	return itemEditor;
}

bool FxItemPropertyWidget::setFxItem(FxItem *fx)
{
	if (!FxItem::exists(fx)) {
		cur_fx = 0;
		audioGroup->setVisible(false);
		sceneGroup->setVisible(false);
		sequenceGroup->setVisible(false);
		return false;
	}
	cur_fx = fx;
	cur_fxa = 0;
	cur_fxs = 0;
	cur_fxseq = 0;

	nameEdit->setText(fx->name());
	idEdit->setText(QString::number(fx->id()));
	keyEdit->setText(QtStaticTools::keyToString(fx->keyCode()));

	preDelayEdit->setText(QtStaticTools::msToTimeString(fx->preDelay()));
	fadeInTimeEdit->setText(QtStaticTools::msToTimeString(fx->fadeInTime()));
	holdTimeEdit->setText(QtStaticTools::msToTimeString(fx->holdTime()));
	fadeOutTimeEdit->setText(QtStaticTools::msToTimeString(fx->fadeOutTime()));
	postDelayEdit->setText(QtStaticTools::msToTimeString(fx->postDelay()));

	if (fx->fxType() == FX_AUDIO) {
		cur_fxa = static_cast<FxAudioItem*>(fx);
		initialVolDial->setValue(cur_fxa->initialVolume);
		audioFilePathEdit->setText(cur_fxa->filePath());
		audioLoopsSpin->setValue(cur_fxa->loopTimes);
		audioStartAtEdit->setText(QtStaticTools::msToTimeString(cur_fxa->initialSeekPos));
		audioStopAtEdit->setText(QtStaticTools::msToTimeString(cur_fxa->stopAtSeekPos));
		hookedToChannelSpin->setValue(fx->hookedChannel()+1);
		hookedToUniverseSpin->setValue(fx->hookedUniverse()+1);

		audioGroup->setVisible(true);
		hookedToGroup->setVisible(true);
	}
	else if (fx->fxType() == FX_AUDIO_PLAYLIST) {
		cur_fxa = static_cast<FxAudioItem*>(fx);
		initialVolDial->setValue(cur_fxa->initialVolume);
		audioFilePathEdit->clear();
		audioLoopsSpin->setValue(cur_fxa->loopTimes);

		audioGroup->setVisible(true);
		hookedToGroup->setVisible(false);
	}
	else {
		audioGroup->setVisible(false);
		hookedToGroup->setVisible(false);
	}

	if (fx->fxType() == FX_SCENE) {
		cur_fxs = static_cast<FxSceneItem*>(fx);
		faderCountEdit->setText(QString::number(cur_fxs->tubeCount()));
		hookedToChannelSpin->setValue(fx->hookedChannel()+1);
		hookedToUniverseSpin->setValue(fx->hookedUniverse()+1);

		sceneGroup->setVisible(true);
		sequenceGroup->setVisible(false);
		hookedToGroup->setVisible(true);
	}
	else if (fx->fxType() == FX_SEQUENCE) {
		cur_fxseq = static_cast<FxSeqItem*>(fx);
		sceneGroup->setVisible(false);
		sequenceGroup->setVisible(true);

		hookedToChannelSpin->setValue(fx->hookedChannel()+1);
		hookedToUniverseSpin->setValue(fx->hookedUniverse()+1);
		hookedToGroup->setVisible(true);

		seqBlackOtherCheck->setChecked(cur_fxseq->blackOtherSeqOnStart);
		seqStopOtherCheck->setChecked(cur_fxseq->stopOtherSeqOnStart);
	}
	else {
		sceneGroup->setVisible(false);
		sequenceGroup->setVisible(false);
	}


	return true;
}

void FxItemPropertyWidget::setEditable(bool state, bool once)
{
	QList<PsLineEdit*>childs = findChildren<PsLineEdit*>();
	foreach(PsLineEdit* edit, childs) {
		edit->setEditable(state);
		edit->setWarnColor(true);
	}
	once_edit_f = once;
	if (!state) {
		editOnceButton->setText(tr("Edit once"));

	}
}


void FxItemPropertyWidget::on_initialVolDial_sliderMoved(int position)
{
	if (FxItem::exists(cur_fxa)) {
		cur_fxa->initialVolume = position;
		cur_fxa->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_nameEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fx)) {
		cur_fx->setName(arg1);
		cur_fx->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_keyEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fx)) {
		QString norm = arg1.trimmed().toUpper();
		if (norm != arg1) keyEdit->setText(norm);

		if (norm.size()) {
			cur_fx->setKeyCode(QtStaticTools::stringToKey(norm));
		} else {
			cur_fx->setKeyCode(0);
		}
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_faderCountEdit_textEdited(const QString &arg1)
{
	if (FxItem::exists(cur_fxs)) {
		bool ok;
		int num = arg1.toInt(&ok);
		if (ok && num != cur_fxs->tubeCount()) {
			cur_fxs->setTubeCount(num);
		}
	}
}

void FxItemPropertyWidget::on_audioFilePathEdit_doubleClicked()
{
	QString path = QFileDialog::getOpenFileName(this
												,tr("Choose Audio File")
												,AppCentral::instance()->userSettings->pLastAudioFxImportPath);
	if (path.size()) {
		AppCentral::instance()->userSettings->pLastAudioFxImportPath = path;

		if (FxItem::exists(cur_fxa)) {
			cur_fxa->setFilePath(path);
			cur_fxa->setModified(true);
			audioFilePathEdit->setText(cur_fxa->filePath());
			emit modified();
		}
	}
}

void FxItemPropertyWidget::on_preDelayEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fx)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fx->preDelay() != time_ms) {
		cur_fx->setPreDelay(time_ms);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_fadeInTimeEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fx)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fx->fadeInTime() != time_ms) {
		cur_fx->setFadeInTime(time_ms);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_holdTimeEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fx)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fx->holdTime() != time_ms) {
		cur_fx->setHoldTime(time_ms);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_fadeOutTimeEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fx)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fx->fadeOutTime() != time_ms) {
		cur_fx->setFadeOutTime(time_ms);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_postDelayEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fx)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fx->postDelay() != time_ms) {
		cur_fx->setPostDelay(time_ms);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_keyClearButton_clicked()
{
	keyEdit->clear();

	if (!FxItem::exists(cur_fx)) return;

	if (cur_fx->keyCode()) {
		cur_fx->setKeyCode(0);
		cur_fx->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_editOnceButton_clicked()
{
	if (AppCentral::instance()->isEditMode()) return;

	if (!once_edit_f) {
		editOnceButton->setText(tr("Cancel Edit Mode"));
	}

	setEditable(!once_edit_f,!once_edit_f);
}

void FxItemPropertyWidget::finish_edit()
{
	setEditable(false,false);
}

void FxItemPropertyWidget::on_audioLoopsSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fxa)) return;

	if (cur_fxa->loopTimes != arg1) {
		cur_fxa->loopTimes = arg1;
		cur_fxa->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_audioStartAtEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fxa)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxa->initialSeekPos != time_ms) {
		cur_fxa->initialSeekPos = time_ms;
		cur_fxa->setModified(true);
		emit modified();
	}
}


void FxItemPropertyWidget::on_audioStopAtEdit_textEdited(const QString &arg1)
{
	if (!FxItem::exists(cur_fxa)) return;

	int time_ms = QtStaticTools::timeStringToMS(arg1);
	if (cur_fxa->stopAtSeekPos != time_ms) {
		cur_fxa->stopAtSeekPos = time_ms;
		cur_fxa->setModified(true);
		emit modified();
	}

}

void FxItemPropertyWidget::on_hookedToUniverseSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fx)) return;
	arg1--;

	if (cur_fx->hookedUniverse() != arg1) {
		cur_fx->hookToUniverse(arg1);
		cur_fxa->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_hookedToChannelSpin_valueChanged(int arg1)
{
	if (!FxItem::exists(cur_fx)) return;
	arg1--;

	if (cur_fx->hookedChannel() != arg1) {
		cur_fx->hookToChannel(arg1);
		cur_fxa->setModified(true);
		emit modified();
	}
}

void FxItemPropertyWidget::on_seqStopOtherCheck_clicked(bool checked)
{
	if (!FxItem::exists(cur_fxseq)) return;

	if (cur_fxseq->stopOtherSeqOnStart != checked) {
		cur_fxseq->stopOtherSeqOnStart = checked;
		emit modified();
	}
}

void FxItemPropertyWidget::on_seqBlackOtherCheck_clicked(bool checked)
{
	if (!FxItem::exists(cur_fxseq)) return;

	if (cur_fxseq->blackOtherSeqOnStart != checked) {
		cur_fxseq->blackOtherSeqOnStart = checked;
		emit modified();
	}
}
